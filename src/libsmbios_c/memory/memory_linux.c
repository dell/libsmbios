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

#define LIBSMBIOS_SOURCE

// Include compat.h first, then system headers, then public, then private
#include "smbios_c/compat.h"

#include <stdarg.h>     // va_list
#include <stdlib.h>
#include <stdio.h>
#include <string.h>     // memcpy
#include <errno.h>
#include <sys/mman.h>   // mmap

#include "smbios_c/obj/memory.h"
#include "smbios_c/types.h"
#include "memory_impl.h"

struct linux_data
{
    char *filename;
    FILE *fd;
    int mem_errno;
    int rw;
    void *lastMapping;
    u64 lastMappedOffset;
    u64 mappingSize;
};

#define READ_MMAP 0
#define WRITE_MMAP 1

static int copy_mmap(const struct memory_access_obj *this, u8 *buffer, u64 offset, size_t length, int rw)
{
    struct linux_data *private_data = (struct linux_data *)this->private_data;
    private_data->mem_errno = errno = 0;
    int retval = -1;
    int flags = rw ? PROT_WRITE : PROT_READ;
    char *openMode = rw ? "r+b": "rb";

    size_t bytesCopied = 0;

    fnprintf("buffer(%p) offset(%lld) length(%zd) rw(%d)\n", buffer, offset, length, rw);
    fnprintf("mappingSize(%lld)\n", private_data->mappingSize);

    if(!private_data->rw || !private_data->fd)
    {
        if(private_data->fd)
            fclose(private_data->fd);

        private_data->rw = rw;
        private_data->lastMapping = 0;
        private_data->lastMappedOffset = -1;
        private_data->fd = fopen( private_data->filename, openMode ); // re-open for write
        if(!private_data->fd)
            goto err_out;
    }

    while( bytesCopied < length )
    {
        off_t mmoff = offset % private_data->mappingSize;
        fnprintf("\tLOOP: bytesCopied(%ld) mmoff(%ld)\n", bytesCopied, mmoff);

        if ((offset-mmoff) != private_data->lastMappedOffset)
        {
            private_data->lastMappedOffset = offset-mmoff;
            if (private_data->lastMapping)
            {
                fnprintf("\t\tmunmap(%p)\n", private_data->lastMapping);
                munmap(private_data->lastMapping, private_data->mappingSize);
            }
            fnprintf("\t\tlastMappedOffset(%lld)\n", private_data->lastMappedOffset);
            private_data->lastMapping = mmap( 0, private_data->mappingSize, flags, MAP_SHARED, fileno(private_data->fd), private_data->lastMappedOffset);
            if ((private_data->lastMapping) == (void *)-1)
                goto err_out;
        }

        unsigned long toCopy = length - bytesCopied;
        if( toCopy + mmoff > (private_data->mappingSize) )
            toCopy = (private_data->mappingSize) - mmoff;

        fnprintf("\t\tCOPYING(%lu)\n", toCopy);
        if (rw)
            memcpy(((u8 *)(private_data->lastMapping) + mmoff),
                    buffer + bytesCopied, toCopy);
        else
            memcpy(buffer + bytesCopied,
                    ((const u8 *)(private_data->lastMapping) + mmoff), toCopy);

#ifdef DEBUG_MEMORY_C
        fnprintf("BUFFER: '");
        for(int i=0;i<toCopy;++i)
            dprintf("%c", buffer[bytesCopied + i]);
        dprintf("'\n");

        fnprintf("MEMORY: '");
        for(int i=0;i<toCopy;++i)
            dprintf("%c", (((const u8 *)(private_data->lastMapping))[mmoff + i]));
        dprintf("'\n");
#endif

        offset += toCopy;
        bytesCopied += toCopy;
    }

    retval = 0;
    goto out;

err_out:
    fnprintf("%s - ERR_OUT: %d \n", __PRETTY_FUNCTION__, errno);
    private_data->mem_errno = errno;
    if (private_data->lastMapping == (void*)-1)
        private_data->lastMapping = 0;

out:
    // close on error, or if close hint
    if (private_data->fd && (memory_obj_should_close(this) || retval))
    {
        fnprintf("out/close\n");
        if (private_data->lastMapping)
            munmap(private_data->lastMapping, private_data->mappingSize);

        private_data->lastMapping = 0;
        private_data->lastMappedOffset = -1;

        fclose(private_data->fd);
        private_data->fd = 0;
    }
    return retval;
}

static int linux_read_fn(const struct memory_access_obj *this, u8 *buffer, u64 offset, size_t length)
{
    return copy_mmap(this, buffer, offset, length, READ_MMAP);
}

static int linux_write_fn(const struct memory_access_obj *this, u8 *buffer, u64 offset, size_t length)
{
    fnprintf(" BUFFER: %s\n", (char *)buffer);
    return copy_mmap(this, buffer, offset, length, WRITE_MMAP);
}

static void linux_free(struct memory_access_obj *this)
{
    struct linux_data *private_data = (struct linux_data *)this->private_data;
    if (private_data->filename)
    {
        free(private_data->filename);
        private_data->filename = 0;
    }

    if (private_data->lastMapping)
    {
        munmap(private_data->lastMapping, private_data->mappingSize);
        private_data->lastMapping=0;
    }

    if (private_data->fd)
        fclose(private_data->fd);

    free(private_data);
    this->private_data = 0;
    this->initialized=0;
}

static void linux_cleanup(struct memory_access_obj *this)
{
    struct linux_data *private_data = (struct linux_data *)this->private_data;

    if (private_data->lastMapping)
    {
        munmap(private_data->lastMapping, private_data->mappingSize);
        private_data->lastMapping=0;
    }

    if (private_data->fd)
    {
        fclose(private_data->fd);
        private_data->fd = 0;
    }
    private_data->mem_errno = 0;
    private_data->rw = 0;
}

__internal void init_mem_struct_filename(struct memory_access_obj *m, const char *fn)
{
    struct linux_data *priv_ut = (struct linux_data *)calloc(1, sizeof(struct linux_data));
    priv_ut->mappingSize = getpagesize() * 16;;
    priv_ut->lastMappedOffset = -1;
    priv_ut->filename = (char *)calloc(1, strlen(fn) + 1);
    priv_ut->rw = 0;
    strcat(priv_ut->filename, fn);

    m->private_data = priv_ut;
    m->free = linux_free;
    m->read_fn = linux_read_fn;
    m->write_fn = linux_write_fn;
    m->cleanup = linux_cleanup;
    m->close = 1;
    m->initialized = 1;
}

__internal void init_mem_struct(struct memory_access_obj *m)
{
   init_mem_struct_filename(m, "/dev/mem");
}




