/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 * vim:expandtab:autoindent:tabstop=4:shiftwidth=4:filetype=c:cindent:textwidth=0:
 *
 * Copyright (C) 2005 Dell Inc.
 *  by Michael Brown <Michael_E_Brown@dell.com>
 * Licensed under the Open Software License version 2.1
 *
 * Alternatively, you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.

 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 */

#define LIBSMBIOS_C_SOURCE

// Include compat.h first, then system headers, then public, then private
#include "smbios_c/compat.h"

// system
#include <alloca.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>  // flock
#include <sys/ioctl.h> // ioctl
#include <errno.h>

// public
#include "smbios_c/obj/smi.h"
#include "smbios_c/types.h"
#include "libsmbios_c_intlize.h"
#include "internal_strl.h"
#include "common_internal.h"

// kernel
#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,15,0)
#include <linux/wmi.h>
#else
#include "wmi.h"
#endif

// private
#include "smi_impl.h"

// forward declarations

const char *wmi_char                  = "/dev/wmi/dell-smbios";
const char *sysfs_basedir             = "/sys/devices/platform/dcdbas/";
const char *smi_data_fn               = "smi_data";
const char *smi_data_buf_phys_addr_fn = "smi_data_buf_phys_addr";
const char *smi_data_buf_size_fn      = "smi_data_buf_size";
const char *smi_request_fn            = "smi_request";

#define bufsize 256

// not in header file. for private use by unit tests.
void set_basedir(const char *newdir)
{
    sysfs_basedir = newdir;
}

#define allocate_path(a, b) (                               \
    {                                                       \
        char *fn_;                                          \
        size_t sz_ = strlen(a) + strlen(b) + 1;             \
        fn_ = alloca(sz_);                                  \
        if (fn_)                                            \
        {                                                   \
            strcpy(fn_, (a));                               \
            strcat(fn_, (b));                               \
        }                                                   \
        fn_;                                                \
    })

u32 __hidden get_phys_buf_addr()
{
    char *fn = NULL;
    FILE *fd = 0;
    u32 physaddr = 0;
    char linebuf[bufsize] = {0,};
    int ret;

    fnprintf("\n");

    fn = allocate_path(sysfs_basedir, smi_data_buf_phys_addr_fn);
    if (!fn)
        goto out;

    fd = fopen(fn, "rb");
    if (!fd)
        goto out;

    ret = fseek(fd, 0L, 0);
    if (ret < 0)
        goto out_close;
    size_t numBytes = fread(linebuf, 1, bufsize, fd);
    if (!numBytes)
        goto out_close;

    physaddr = strtoll(linebuf, NULL, 16);

out_close:
    if (fd)
        fclose(fd);
    fflush(NULL);

out:
    return physaddr;
}

// returns physaddr
u32 __hidden set_phys_buf_size(u32 newsize)
{
    char *fn;
    FILE *fd = NULL;
    char linebuf[bufsize] = {0,};
    u32 phys_buf_addr=0;

    fnprintf("\n");

    fn = allocate_path(sysfs_basedir, smi_data_buf_size_fn);
    if (!fn)
        goto out;

    fd = fopen(fn, "w+b");
    if (!fd)
        goto out;

    snprintf(linebuf, bufsize, "%d", newsize);
    size_t recs = fwrite(linebuf, strlen(linebuf)+1, 1, fd);
    if (recs != 1)
        goto out;

    phys_buf_addr = get_phys_buf_addr();

out:
    if (fd)
        fclose(fd);
    fflush(NULL);

    return phys_buf_addr;
}

void __hidden write_smi_data(u8 *buffer, size_t size)
{
    char *fn;
    FILE *fd = 0;

    fnprintf("\n");

    fn = allocate_path(sysfs_basedir, smi_data_fn);
    if (!fn)
        goto out;

    fnprintf("open data file: '%s'\n", fn);

    fd = fopen(fn, "w+b");
    if (!fd)
        goto out;

    size_t recs = fwrite(buffer, 1, size, fd);
    UNREFERENCED_PARAMETER(recs);
    fnprintf("wrote %zd recs\nclose()\n", recs);
    fclose(fd);
    fnprintf("fflush()\n");
    fflush(NULL);
out:
    fnprintf("return\n");
    return;
}

void __hidden trigger_smi(FILE *fd)
{
    fnprintf("\n");

    if (!fd)
        goto out;

    size_t recs = fwrite("1", 1, 2, fd);
    UNREFERENCED_PARAMETER(recs);
    fflush(NULL);

out:
    return;
}

void __hidden get_smi_results(u8 *buffer, size_t size)
{
    char *fn;
    FILE *fd = 0;

    fnprintf("\n");

    fn = allocate_path(sysfs_basedir, smi_data_fn);
    if (!fn)
        goto out;

    fd = fopen(fn, "rb");
    if (!fd)
        goto out;

    size_t recs = fread(buffer, size, 1, fd);
    UNREFERENCED_PARAMETER(recs);
    fclose(fd);
out:
    return;
}

FILE *open_request_file()
{
    char *fn;
    FILE *fd = 0;
    int ret;

    fn = allocate_path(sysfs_basedir, smi_request_fn);
    if (!fn)
        return NULL;

    fnprintf("open request file: '%s'\n", fn);
    fd = fopen(fn, "wb");
    if (!fd)
        return NULL;

    flock( fileno(fd), LOCK_EX );
    ret = fwrite("0", 1, 1, fd);

    fnprintf("got fd for request file: %p\n", fd);
    UNREFERENCED_PARAMETER(ret);
    return fd;
}


#define TO_KERNEL_BUF true
#define FROM_KERNEL_BUF false

void copy_phys_bufs_smi(struct dell_smi_obj *this, struct callintf_cmd *kernel_buf, u32 physaddr, bool direction)
{
    u32 curoffset = sizeof(this->smi_buf) + sizeof(struct callintf_cmd);

    u8 *dest = 0;
    u8 *source = 0;

    fnprintf(" sizeof(this->smi_buf)==%zd   sizeof(struct callintf_cmd)==%zd  \n", sizeof(this->smi_buf), sizeof(struct callintf_cmd));
    fnprintf(" kernel_buf %p\n", kernel_buf);

    for (int i=0;i<4;i++)
    {
        if (! this->physical_buffer[i])
            continue;

        fnprintf("copy buffer[%d] size==%zd\n", i, this->physical_buffer_size[i]);
        fnprintf("current offset==%d\n", curoffset);

        source = this->physical_buffer[i];
        dest = (u8*)kernel_buf + curoffset;
        if (direction == FROM_KERNEL_BUF)
        {
            source = (u8*)kernel_buf + curoffset;
            dest = this->physical_buffer[i];
        }

        this->smi_buf.arg[i] = curoffset + physaddr;
        memcpy(dest, source, this->physical_buffer_size[i]);
        curoffset += this->physical_buffer_size[i];
    }
}

void copy_phys_bufs_wmi(struct dell_smi_obj *this, struct dell_wmi_smbios_buffer *buf, bool direction)
{
    // starts at offset of data
    u32 curoffset = (u8*) &buf->ext.data - (u8*) &buf->std;

    u8 *dest = 0;
    u8 *source = 0;

    for (int i=0;i<4;i++)
    {
        if (! this->physical_buffer[i])
            continue;

        /* mark which attributes have buffer offsets */
        if (direction == FROM_KERNEL_BUF) {
            source = (u8*)&buf->std + curoffset;
            dest = this->physical_buffer[i];
        } else {
            buf->ext.argattrib |= 1 << (i * 8);
            buf->ext.blength += this->physical_buffer_size[i];
            source = this->physical_buffer[i];
            dest = (u8*)&buf->std + curoffset;
        }

        memcpy(dest, source, this->physical_buffer_size[i]);
        buf->std.input[i] = curoffset;
        curoffset += this->physical_buffer_size[i];
    }
}

int __hidden wmi_supported()
{
    if (access(wmi_char, F_OK) != -1)
        return 1;
    return 0;
}

int __hidden LINUX_dell_wmi_obj_execute(struct dell_smi_obj *this)
{
    struct dell_wmi_smbios_buffer *buffer;
    u64 length;
    FILE *f;
    int fd;
    int ret;

    // setup buffer size
    f = fopen(wmi_char, "rb");
    if (!f)
        return -EINVAL;
    ret = fread(&length, sizeof(__u64), 1, f);
    fnprintf("length: %llu\n", length);
    fclose(f);
    if (ret <= 0 || length > 65536)
        return -EIO;
    buffer = calloc(1, length);
    if (!buffer)
        return -ENOMEM;
    buffer->length = length;

    // update our buf
    memcpy(&buffer->std, &(this->smi_buf), sizeof(this->smi_buf));

    // copy in each physical addr buf
    copy_phys_bufs_wmi(this, buffer, TO_KERNEL_BUF);

    // perform command
    fd = open(wmi_char, O_NONBLOCK);
    if (fd < 0) {
        ret = -EIO;
        goto out_wmi;
    }
    ret = ioctl(fd, DELL_WMI_SMBIOS_CMD, buffer);
    close(fd);
    if (ret)
        goto out_wmi;

    // copy result out
    memcpy(&(this->smi_buf), &buffer->std, sizeof(this->smi_buf));

    // update smi buffer
    copy_phys_bufs_wmi(this, buffer, FROM_KERNEL_BUF);

out_wmi:
    free(buffer);
    return ret;
}

int __hidden LINUX_dell_smi_obj_execute(struct dell_smi_obj *this)
{
    struct callintf_cmd *kernel_buf;
    size_t alloc_size = sizeof(struct callintf_cmd) + sizeof(this->smi_buf);
    int retval = -1;
    u8 *buffer = 0;

    fnprintf("\n");

    // calculate buffer space needed
    for(int i=0; i<4; i++)
        alloc_size += this->physical_buffer_size[i];

    // allocate buffer
    fnprintf(" allocate buffer: %zd\n", alloc_size);
    buffer = calloc(1, alloc_size);
    kernel_buf = (struct callintf_cmd *)buffer;

    // LOCK
    fnprintf(" open_request_file()\n");
    FILE *fd = open_request_file();
    if (!fd)
        goto err_out;

    // set phys buf size
    fnprintf(" set buffer size\n");
    u32 physaddr = set_phys_buf_size(alloc_size);

    // setup kernel args
    kernel_buf->magic = KERNEL_SMI_MAGIC_NUMBER;
    kernel_buf->ebx = 0;
    kernel_buf->ecx   = DELL_CALLINTF_SMI_MAGIC_NUMBER;
    kernel_buf->command_address = this->command_address;
    kernel_buf->command_code = this->command_code;

    // copy in each physical addr buf
    copy_phys_bufs_smi(this, kernel_buf, physaddr, TO_KERNEL_BUF);

    // setup std smi args
    memcpy(kernel_buf->command_buffer_start, &(this->smi_buf), sizeof(this->smi_buf));

    // write the whole thing to the smi file
    fnprintf(" write smi data\n");
    write_smi_data(buffer, alloc_size);

    // trigger smi
    fnprintf(" trigger smi\n");
    trigger_smi(fd);

    // copy results back
    fnprintf(" read smi results\n");
    get_smi_results(buffer, alloc_size);

    // unlock
    flock( fileno(fd), LOCK_UN );

    // update our physical address bufs
    memcpy(&(this->smi_buf), kernel_buf->command_buffer_start, sizeof(this->smi_buf));

    // update smi buffer
    copy_phys_bufs_smi(this, kernel_buf, physaddr, FROM_KERNEL_BUF);

    fclose(fd);

    retval = 0;
    goto out;

err_out:
    fnprintf(" err_out\n");
    strlcpy( this->errstring, _("There was an error trying to perform the smi execute() cmd. Is the 'dcdbas' kernel module loaded?"), ERROR_BUFSIZE);
    strlcat(this->errstring, _("\nThe OS Error string was: "), ERROR_BUFSIZE);
    fixed_strerror(errno, this->errstring, ERROR_BUFSIZE);

out:
    free(buffer);
    fnprintf("retval: %d\n", retval);
    return retval;
}

int __hidden init_dell_smi_obj(struct dell_smi_obj *this)
{
    if (wmi_supported())
        this->execute = LINUX_dell_wmi_obj_execute;
    else
        this->execute = LINUX_dell_smi_obj_execute;
    return init_dell_smi_obj_std(this);
}
