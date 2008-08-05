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


#include "smbios_c/memory.h"
#include "smbios_c/types.h"
#include "memory_impl.h"

#if defined(DEBUG_MEMORY_C)
#   define dprintf(format, args...) do { fprintf(stdout , format , ## args);  } while(0)
#else
#   define dprintf(format, args...) do {} while(0)
#endif

#if LIBSMBIOS_C_USE_MEMORY_MMAP
#define MEM_INIT_FUNCTION init_mem_struct_filename
#endif

#define __hidden __attribute__((visibility("hidden")))
#define __internal __attribute__((visibility("internal")))

void __internal init_mem_struct(struct memory *m);
void __internal MEM_INIT_FUNCTION(struct memory *m, const char *fn);

struct ut_data
{
    char *filename;
    FILE *fd;
    int mem_errno;
    int rw;
    void *lastMapping;
    u64 lastMappedOffset;
    u64 mappingSize;
};

static __internal int copy_mmap(struct memory *this, u8 *buffer, u64 offset, size_t length, int fromMem)
{
    struct ut_data *private_data = (struct ut_data *)this->private_data;
    private_data->mem_errno = errno = 0;
    int retval = -1;
    int flags = fromMem ? PROT_READ : PROT_WRITE;
    char *openMode = fromMem ? "r+b": "rb";

    size_t bytesCopied = 0;

    dprintf("copy_mmap: buffer(%p) offset(%ld) length(%ld) fromMem(%d)\n", buffer, offset, length, fromMem);
    dprintf("         : mappingSize(%d)\n", private_data->mappingSize);

    if(!private_data->rw || !private_data->fd)
    {
        if(private_data->fd)
            fclose(private_data->fd);

        private_data->lastMapping = 0;
        private_data->lastMappedOffset = -1;
        private_data->fd = fopen( private_data->filename, openMode ); // re-open for write
        if(!private_data->fd)
            goto err_out;

        if (!fromMem)
            private_data->rw=1;
    }

    while( bytesCopied < length )
    {
        off_t mmoff = offset % private_data->mappingSize;
        dprintf("\tLOOP: bytesCopied(%ld) mmoff(%ld)\n", bytesCopied, mmoff);

        if ((offset-mmoff) != private_data->lastMappedOffset)
        {
            private_data->lastMappedOffset = offset-mmoff;
            if (private_data->lastMapping)
            {
                dprintf("\t\tmunmap(%ld)\n", private_data->lastMapping);
                munmap(private_data->lastMapping, private_data->mappingSize);
            }
            dprintf("\t\tlastMappedOffset(%ld)\n", private_data->lastMappedOffset);
            private_data->lastMapping = mmap( 0, private_data->mappingSize, flags, MAP_PRIVATE, fileno(private_data->fd), private_data->lastMappedOffset);
            if ((private_data->lastMapping) == (void *)-1)
                goto err_out;
        }

        unsigned long toCopy = length - bytesCopied;
        if( toCopy + mmoff > (private_data->mappingSize) )
            toCopy = (private_data->mappingSize) - mmoff;

        dprintf("\t\tCOPYING(%d)\n", toCopy);
        if (fromMem)
            memcpy(buffer + bytesCopied, 
                    ((const u8 *)(private_data->lastMapping) + mmoff), toCopy);
        else
            memcpy(((u8 *)(private_data->lastMapping) + mmoff), 
                    buffer + bytesCopied, 
                    toCopy);

        offset += toCopy;
        bytesCopied += toCopy;
    }

    retval = 0;

err_out:
    private_data->mem_errno = errno;
    if (private_data->lastMapping == (void*)-1)
        private_data->lastMapping = 0;

out:
    // close on error, or if close hint
    if (private_data->fd && (this->close || retval))
    {
        if (private_data->lastMapping)
            munmap(private_data->lastMapping, private_data->mappingSize);

        private_data->lastMapping = 0;
        private_data->lastMappedOffset = -1;

        fclose(private_data->fd);
        private_data->fd = 0;
    }
    return retval;
}

static __internal int linux_read_fn(struct memory *this, u8 *buffer, u64 offset, size_t length)
{
    return copy_mmap(this, buffer, offset, length, 1);
}

static __internal int linux_write_fn(struct memory *this, u8 *buffer, u64 offset, size_t length)
{
    return copy_mmap(this, buffer, offset, length, 0);
}

static __internal void linux_free(struct memory *this)
{
    struct ut_data *private_data = (struct ut_data *)this->private_data;
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
    {
        fclose(private_data->fd);
    }
    free(private_data);
    this->private_data = 0;
    this->initialized=0;
}


static __internal void linux_cleanup(struct memory *this)
{
    struct ut_data *private_data = (struct ut_data *)this->private_data;

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

__internal void MEM_INIT_FUNCTION(struct memory *m, const char *fn)
{
    struct ut_data *priv_ut = (struct ut_data *)calloc(1, sizeof(struct ut_data));
    priv_ut->mappingSize = getpagesize() * 16;;
    priv_ut->lastMappedOffset = -1;
    priv_ut->filename = (char *)calloc(1, strlen(fn) + 1);
    strcat(priv_ut->filename, fn);

    m->private_data = priv_ut;
    m->free = linux_free;
    m->read_fn = linux_read_fn;
    m->write_fn = linux_write_fn;
    m->cleanup = linux_cleanup;
    m->close = 0;
    m->initialized = 1;
}

__internal void init_mem_struct(struct memory *m)
{
   init_mem_struct_filename(m, "/dev/mem"); 
}




