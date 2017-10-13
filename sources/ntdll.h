#pragma once

/*
  Load internal ntdll.dll APIs
*/

#if defined(_WIN32) && defined(_WIN32_WINNT) && (_WIN32_WINNT>0x500)
// Only support Windows XP and above
#include <windows.h>
#include <winternl.h>

#ifdef __cplusplus
extern "C" {
#endif

#define STATUS_SUCCESS                  ((LONG)0x00000000)
#define STATUS_PROCEDURE_NOT_FOUND      ((LONG)0xC000007A)

NTSTATUS
NTAPI
ChxNtdllInit(
    );

NTSTATUS
NTAPI
ChxNtClose (
    HANDLE Handle
    );

NTSTATUS
NTAPI
ChxNtCreateFile (
    PHANDLE FileHandle,
    ACCESS_MASK DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    PIO_STATUS_BLOCK IoStatusBlock,
    PLARGE_INTEGER AllocationSize,
    ULONG FileAttributes,
    ULONG ShareAccess,
    ULONG CreateDisposition,
    ULONG CreateOptions,
    PVOID EaBuffer,
    ULONG EaLength
    );

NTSTATUS
NTAPI
ChxNtOpenFile (
    PHANDLE FileHandle,
    ACCESS_MASK DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    PIO_STATUS_BLOCK IoStatusBlock,
    ULONG ShareAccess,
    ULONG OpenOptions
    );

NTSTATUS
NTAPI
ChxNtRenameKey (
    HANDLE KeyHandle,
    PUNICODE_STRING NewName
    );

NTSTATUS
NTAPI
ChxNtNotifyChangeMultipleKeys (
    HANDLE MasterKeyHandle,
    ULONG Count,
    OBJECT_ATTRIBUTES SubordinateObjects[],
    HANDLE Event,
    PIO_APC_ROUTINE ApcRoutine,
    PVOID ApcContext,
    PIO_STATUS_BLOCK IoStatusBlock,
    ULONG CompletionFilter,
    BOOLEAN WatchTree,
    PVOID Buffer,
    ULONG BufferSize,
    BOOLEAN Asynchronous
    );

NTSTATUS
NTAPI
ChxNtQueryMultipleValueKey (
    HANDLE KeyHandle,
    PKEY_VALUE_ENTRY ValueEntries,
    ULONG EntryCount,
    PVOID ValueBuffer,
    PULONG BufferLength,
    PULONG RequiredBufferLength
    );

NTSTATUS
NTAPI
ChxNtSetInformationKey (
    HANDLE KeyHandle,
    KEY_SET_INFORMATION_CLASS KeySetInformationClass,
    PVOID KeySetInformation,
    ULONG KeySetInformationLength
    );

NTSTATUS
NTAPI
ChxNtDeviceIoControlFile (
    HANDLE FileHandle,
    HANDLE Event,
    PIO_APC_ROUTINE ApcRoutine,
    PVOID ApcContext,
    PIO_STATUS_BLOCK IoStatusBlock,
    ULONG IoControlCode,
    PVOID InputBuffer,
    ULONG InputBufferLength,
    PVOID OutputBuffer,
    ULONG OutputBufferLength
    );

NTSTATUS
NTAPI
ChxNtWaitForSingleObject (
    HANDLE Handle,
    BOOLEAN Alertable,
    PLARGE_INTEGER Timeout
    );

BOOLEAN
NTAPI
ChxRtlIsNameLegalDOS8Dot3 (
    PUNICODE_STRING Name,
    POEM_STRING OemName,
    PBOOLEAN NameContainsSpaces
    );

ULONG
NTAPI
ChxRtlNtStatusToDosError (
   NTSTATUS Status
   );

NTSTATUS
NTAPI
ChxNtQueryInformationProcess (
    HANDLE ProcessHandle,
    PROCESSINFOCLASS ProcessInformationClass,
    PVOID ProcessInformation,
    ULONG ProcessInformationLength,
    PULONG ReturnLength
    );

NTSTATUS
NTAPI
ChxNtQueryInformationThread (
    HANDLE ThreadHandle,
    THREADINFOCLASS ThreadInformationClass,
    PVOID ThreadInformation,
    ULONG ThreadInformationLength,
    PULONG ReturnLength
    );

NTSTATUS
NTAPI
ChxNtQueryObject (
    HANDLE Handle,
    OBJECT_INFORMATION_CLASS ObjectInformationClass,
    PVOID ObjectInformation,
    ULONG ObjectInformationLength,
    PULONG ReturnLength
    );

NTSTATUS
NTAPI
ChxNtQuerySystemInformation (
    SYSTEM_INFORMATION_CLASS SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength
    );

NTSTATUS
NTAPI
ChxNtQuerySystemTime (
    PLARGE_INTEGER SystemTime
    );

NTSTATUS
NTAPI
ChxRtlLocalTimeToSystemTime (
    PLARGE_INTEGER LocalTime,
    PLARGE_INTEGER SystemTime
    );

BOOLEAN
NTAPI
ChxRtlTimeToSecondsSince1970 (
    PLARGE_INTEGER Time,
    PULONG ElapsedSeconds
    );

VOID
NTAPI
ChxRtlFreeAnsiString (
    PANSI_STRING AnsiString
    );

VOID
NTAPI
ChxRtlFreeUnicodeString (
    PUNICODE_STRING UnicodeString
    );

VOID
NTAPI
ChxRtlFreeOemString(
    POEM_STRING OemString
    );

VOID
NTAPI
ChxRtlInitString (
    PSTRING DestinationString,
    PCSZ SourceString
    );

NTSTATUS
NTAPI
ChxRtlInitStringEx (
    PSTRING DestinationString,
    PCSZ SourceString
    );

VOID
NTAPI
ChxRtlInitAnsiString (
    PANSI_STRING DestinationString,
    PCSZ SourceString
    );

NTSTATUS
NTAPI
ChxRtlInitAnsiStringEx (
    PANSI_STRING DestinationString,
    PCSZ SourceString
    );

VOID
NTAPI
ChxRtlInitUnicodeString (
    PUNICODE_STRING DestinationString,
    PCWSTR SourceString
    );

NTSTATUS
NTAPI
ChxRtlAnsiStringToUnicodeString (
    PUNICODE_STRING DestinationString,
    PCANSI_STRING SourceString,
    BOOLEAN AllocateDestinationString
    );

NTSTATUS
NTAPI
ChxRtlUnicodeStringToAnsiString (
    PANSI_STRING DestinationString,
    PCUNICODE_STRING SourceString,
    BOOLEAN AllocateDestinationString
    );

NTSTATUS
NTAPI
ChxRtlUnicodeStringToOemString(
    POEM_STRING DestinationString,
    PCUNICODE_STRING SourceString,
    BOOLEAN AllocateDestinationString
    );

NTSTATUS
NTAPI
ChxRtlUnicodeToMultiByteSize(
    PULONG BytesInMultiByteString,
    PWCH UnicodeString,
    ULONG BytesInUnicodeString
    );

NTSTATUS
NTAPI
ChxRtlCharToInteger (
    PCSZ String,
    ULONG Base,
    PULONG Value
    );

NTSTATUS
NTAPI
ChxRtlConvertSidToUnicodeString (
    PUNICODE_STRING UnicodeString,
    PSID Sid,
    BOOLEAN AllocateDestinationString
    );

ULONG
NTAPI
ChxRtlUniform (
    PULONG Seed
    );


#ifdef __cplusplus
}
#endif

#endif  // _WIN32
