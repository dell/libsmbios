// vim:expandtab:autoindent:tabstop=4:shiftwidth=4:filetype=c:
/*
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


#ifndef MINIDDK_H
#define MINIDDK_H

#include <windows.h>
#include <stdio.h>


//From #include "ntdef.h"
typedef LONG NTSTATUS;

//
// Generic test for success on any status value (non-negative numbers
// indicate success).
//

#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)
//
// Unicode strings are counted 16-bit character strings. If they are
// NULL terminated, Length does not include trailing NULL.
//

typedef struct _UNICODE_STRING
{
    USHORT Length;
    USHORT MaximumLength;
#ifdef MIDL_PASS

    [size_is(MaximumLength / 2), length_is((Length) / 2) ] USHORT * Buffer;
#else // MIDL_PASS

    PWSTR  Buffer;
#endif // MIDL_PASS
}
UNICODE_STRING;
typedef UNICODE_STRING *PUNICODE_STRING;
typedef const UNICODE_STRING *PCUNICODE_STRING;

#if defined(_MSC_VER)
#define UNICODE_NULL ((WCHAR)0) // winnt
#endif

//
// Valid values for the Attributes field
//

#define OBJ_INHERIT             0x00000002L
#define OBJ_PERMANENT           0x00000010L
#define OBJ_EXCLUSIVE           0x00000020L
#define OBJ_CASE_INSENSITIVE    0x00000040L
#define OBJ_OPENIF              0x00000080L
#define OBJ_OPENLINK            0x00000100L
#define OBJ_KERNEL_HANDLE       0x00000200L
#define OBJ_VALID_ATTRIBUTES    0x000003F2L

//
// Object Attributes structure
//

typedef struct _OBJECT_ATTRIBUTES
{
    ULONG Length;
    HANDLE RootDirectory;
    PUNICODE_STRING ObjectName;
    ULONG Attributes;
    PVOID SecurityDescriptor;        // Points to type SECURITY_DESCRIPTOR
    PVOID SecurityQualityOfService;  // Points to type SECURITY_QUALITY_OF_SERVICE
}
OBJECT_ATTRIBUTES;
typedef OBJECT_ATTRIBUTES *POBJECT_ATTRIBUTES;

//++
//
// VOID
// InitializeObjectAttributes(
//     OUT POBJECT_ATTRIBUTES p,
//     IN PUNICODE_STRING n,
//     IN ULONG a,
//     IN HANDLE r,
//     IN PSECURITY_DESCRIPTOR s
//     )
//
//--

#define InitializeObjectAttributes( p, n, a, r, s ) { \
    (p)->Length = sizeof( OBJECT_ATTRIBUTES );          \
    (p)->RootDirectory = r;                             \
    (p)->Attributes = a;                                \
    (p)->ObjectName = n;                                \
    (p)->SecurityDescriptor = s;                        \
    (p)->SecurityQualityOfService = NULL;               \
    }

//
// Physical address.
//

typedef LARGE_INTEGER PHYSICAL_ADDRESS, *PPHYSICAL_ADDRESS;

//
// From winnt.h
//
//
// Section Information Structures.
//

typedef enum _SECTION_INHERIT {
    ViewShare = 1,
    ViewUnmap = 2
} SECTION_INHERIT;

//
// Section Access Rights.
//

// begin_winnt
#if defined(_MSC_VER)
#define SECTION_QUERY       0x0001
#define SECTION_MAP_WRITE   0x0002
#define SECTION_MAP_READ    0x0004
#define SECTION_MAP_EXECUTE 0x0008
#define SECTION_EXTEND_SIZE 0x0010

#define SECTION_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED|SECTION_QUERY|\
                            SECTION_MAP_WRITE |      \
                            SECTION_MAP_READ |       \
                            SECTION_MAP_EXECUTE |    \
                            SECTION_EXTEND_SIZE)
#endif

// From ntddk.h
#define NtCurrentProcess() ( (HANDLE) -1 )

// NtDll Func decls same as ZwOpenSection
typedef NTSTATUS (__stdcall *NtOpenSectionPtr) (
    OUT PHANDLE             SectionHandle,
    IN ACCESS_MASK          DesiredAccess,
    IN POBJECT_ATTRIBUTES   ObjectAttributes );

// Same as ZwClose
typedef NTSTATUS (__stdcall *NtClosePtr) (
    IN HANDLE  Handle );

// Same as ZwMapViewOfSection
typedef NTSTATUS (__stdcall *NtMapViewOfSectionPtr) (
    IN HANDLE               SectionHandle,
    IN HANDLE               ProcessHandle,
    IN OUT PVOID            *BaseAddress OPTIONAL,
    IN ULONG                ZeroBits OPTIONAL,
    IN ULONG                CommitSize,
    IN OUT PLARGE_INTEGER   SectionOffset OPTIONAL,
    IN OUT PULONG           ViewSize,
    IN SECTION_INHERIT      InheritDisposition,
    IN ULONG                AllocationType OPTIONAL,
    IN ULONG                Protect );

// Same as ZwUnmapViewOfSection
typedef NTSTATUS (__stdcall *NtUnmapViewOfSectionPtr) (
    IN HANDLE               ProcessHandle,
    IN PVOID                BaseAddress );

typedef VOID (__stdcall *RtlInitUnicodeStringPtr) (
    IN OUT PUNICODE_STRING  DestinationString,
    IN PCWSTR  SourceString );


// For Debug Control
typedef enum _DEBUG_CONTROL_CODE {
    DebugGetTraceInformation = 1,
    DebugSetInternalBreakpoint,
    DebugSetSpecialCall,
    DebugClearSpecialCalls,
    DebugQuerySpecialCalls,
    DebugDbgBreakPoint,
    DebugMaximum,
    DebugSysReadPhysicalMemory = 10,
    DebugSysReadIoSpace = 14,
    DebugSysWriteIoSpace = 15
} DEBUG_CONTROL_CODE;


typedef NTSTATUS (__stdcall *ZwSystemDebugControlPtr) (
    IN DEBUG_CONTROL_CODE  ControlCode,
    IN PVOID  InputBuffer  OPTIONAL,
    IN ULONG  InputBufferLength,
    OUT PVOID  OutputBuffer  OPTIONAL,
    IN ULONG  OutputBufferLength,
    OUT PULONG  ReturnLength  OPTIONAL );

typedef NTSTATUS (__stdcall *EnumSystemFirmwareTablesPtr) (
    IN DWORD iFirmwareTableProviderSignature,
    OUT PVOID  InputBuffer,
    IN DWORD  InputBufferLength );

typedef NTSTATUS (__stdcall *GetSystemFirmwareTablePtr) (
    IN DWORD iFirmwareTableProviderSignature,
    IN DWORD  FirmwareTableID,
    OUT PVOID  InputBuffer,
    IN DWORD  InputBufferLength );

// See
typedef struct _IO_STRUCT
{
    DWORD  Addr;
    DWORD  Reserved1;
    PVOID  pBuf;
    DWORD  NumBytes;
    DWORD  Reserved4;
    DWORD  Reserved5;
    DWORD  Reserved6;
    DWORD  Reserved7;
}
IO_STRUCT;

#ifdef LIBSMBIOS_WIN_USE_WMI

// Define the WMI SMBIOS Information Structure

typedef struct _WMISMBIOSINFO {
    u8  majorVersion;
    u8  minorVersion;
    u32 bufferSize;
    u8  *buffer;
} WMISMBIOSINFO;

#endif

#ifdef _MSC_VER
#pragma pack(push,1)
#endif
typedef struct MEM_STRUCT
{
    DWORD Addr;
    DWORD Reserved1;
    void *pBuf;
    DWORD NumBytes;
} LIBSMBIOS_PACKED_ATTR
MEM_STRUCT;
#ifdef _MSC_VER
#pragma pack(pop,1)
#endif

#endif /* MINIDDK_H */
