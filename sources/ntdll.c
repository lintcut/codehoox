#if defined(_WIN32) && defined(_WIN32_WINNT) && _WIN32_WINNT>0x500

#include "ntdll.h"
#include "image.h"
#include "process.h"

//
//  Declare ntdll.dll undocumented APIs
//

typedef
NTSTATUS
(NTAPI* NtClose_t)(
    HANDLE Handle
    );

typedef
NTSTATUS
(NTAPI* NtCreateFile_t)(
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

typedef
NTSTATUS
(NTAPI* NtOpenFile_t)(
    PHANDLE FileHandle,
    ACCESS_MASK DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    PIO_STATUS_BLOCK IoStatusBlock,
    ULONG ShareAccess,
    ULONG OpenOptions
    );

typedef
NTSTATUS
(NTAPI* NtRenameKey_t)(
    HANDLE KeyHandle,
    PUNICODE_STRING NewName
    );

typedef
NTSTATUS
(NTAPI* NtNotifyChangeMultipleKeys_t)(
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

typedef
NTSTATUS
(NTAPI* NtQueryMultipleValueKey_t)(
    HANDLE KeyHandle,
    PKEY_VALUE_ENTRY ValueEntries,
    ULONG EntryCount,
    PVOID ValueBuffer,
    PULONG BufferLength,
    PULONG RequiredBufferLength
    );

typedef
NTSTATUS
(NTAPI* NtSetInformationKey_t)(
    HANDLE KeyHandle,
    KEY_SET_INFORMATION_CLASS KeySetInformationClass,
    PVOID KeySetInformation,
    ULONG KeySetInformationLength
    );

typedef
NTSTATUS
(NTAPI* NtDeviceIoControlFile_t)(
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

typedef
NTSTATUS
(NTAPI* NtWaitForSingleObject_t)(
    HANDLE Handle,
    BOOLEAN Alertable,
    PLARGE_INTEGER Timeout
    );

typedef
BOOLEAN
(NTAPI* RtlIsNameLegalDOS8Dot3_t)(
    PUNICODE_STRING Name,
    POEM_STRING OemName,
    PBOOLEAN NameContainsSpaces
    );

typedef
ULONG
(NTAPI* RtlNtStatusToDosError_t)(
    NTSTATUS Status
    );

typedef
NTSTATUS
(NTAPI* NtQueryInformationProcess_t)(
    HANDLE ProcessHandle,
    PROCESSINFOCLASS ProcessInformationClass,
    PVOID ProcessInformation,
    ULONG ProcessInformationLength,
    PULONG ReturnLength
    );

typedef
NTSTATUS
(NTAPI* NtQueryInformationThread_t)(
    HANDLE ThreadHandle,
    THREADINFOCLASS ThreadInformationClass,
    PVOID ThreadInformation,
    ULONG ThreadInformationLength,
    PULONG ReturnLength
    );

typedef
NTSTATUS
(NTAPI* NtQueryObject_t)(
    HANDLE Handle,
    OBJECT_INFORMATION_CLASS ObjectInformationClass,
    PVOID ObjectInformation,
    ULONG ObjectInformationLength,
    PULONG ReturnLength
    );

typedef
NTSTATUS
(NTAPI* NtQuerySystemInformation_t)(
    SYSTEM_INFORMATION_CLASS SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength
    );

typedef
NTSTATUS
(NTAPI* NtQuerySystemTime_t)(
    PLARGE_INTEGER SystemTime
);

typedef
NTSTATUS
(NTAPI* RtlLocalTimeToSystemTime_t)(
    PLARGE_INTEGER LocalTime,
    PLARGE_INTEGER SystemTime
    );

typedef
BOOLEAN
(NTAPI* RtlTimeToSecondsSince1970_t)(
    PLARGE_INTEGER Time,
    PULONG ElapsedSeconds
    );

typedef
VOID
(NTAPI* RtlFreeAnsiString_t)(
    PANSI_STRING AnsiString
    );

typedef
VOID
(NTAPI* RtlFreeUnicodeString_t)(
    PUNICODE_STRING UnicodeString
    );

typedef
VOID
(NTAPI* RtlFreeOemString_t)(
    POEM_STRING OemString
    );

typedef
VOID
(NTAPI* RtlInitString_t)(
    PSTRING DestinationString,
    PCSZ SourceString
    );

typedef
NTSTATUS
(NTAPI* RtlInitStringEx_t)(
    PSTRING DestinationString,
    PCSZ SourceString
    );

typedef
VOID
(NTAPI* RtlInitAnsiString_t)(
    PANSI_STRING DestinationString,
    PCSZ SourceString
    );

typedef
NTSTATUS
(NTAPI* RtlInitAnsiStringEx_t)(
    PANSI_STRING DestinationString,
    PCSZ SourceString
    );

typedef
VOID
(NTAPI* RtlInitUnicodeString_t)(
    PUNICODE_STRING DestinationString,
    PCWSTR SourceString
    );

typedef
NTSTATUS
(NTAPI* RtlAnsiStringToUnicodeString_t)(
    PUNICODE_STRING DestinationString,
    PCANSI_STRING SourceString,
    BOOLEAN AllocateDestinationString
    );

typedef
NTSTATUS
(NTAPI* RtlUnicodeStringToAnsiString_t)(
    PANSI_STRING DestinationString,
    PCUNICODE_STRING SourceString,
    BOOLEAN AllocateDestinationString
    );

typedef
NTSTATUS
(NTAPI* RtlUnicodeStringToOemString_t)(
    POEM_STRING DestinationString,
    PCUNICODE_STRING SourceString,
    BOOLEAN AllocateDestinationString
    );

typedef
NTSTATUS
(NTAPI* RtlUnicodeToMultiByteSize_t)(
    PULONG BytesInMultiByteString,
    PWCH UnicodeString,
    ULONG BytesInUnicodeString
    );

typedef
NTSTATUS
(NTAPI* RtlCharToInteger_t)(
    PCSZ String,
    ULONG Base,
    PULONG Value
    );

typedef
NTSTATUS
(NTAPI* RtlConvertSidToUnicodeString_t)(
    PUNICODE_STRING UnicodeString,
    PSID Sid,
    BOOLEAN AllocateDestinationString
    );

typedef
ULONG
(NTAPI* RtlUniform_t)(
    PULONG Seed
    );


//
//
//

typedef struct _NTDLL {
    HMODULE h;
    NtClose_t FpNtClose;
    NtCreateFile_t FpNtCreateFile;
    NtOpenFile_t FpNtOpenFile;
    NtRenameKey_t FpNtRenameKey;
    NtNotifyChangeMultipleKeys_t FpNtNotifyChangeMultipleKeys;
    NtQueryMultipleValueKey_t FpNtQueryMultipleValueKey;
    NtSetInformationKey_t FpNtSetInformationKey;
    NtDeviceIoControlFile_t FpNtDeviceIoControlFile;
    NtWaitForSingleObject_t FpNtWaitForSingleObject;
    RtlIsNameLegalDOS8Dot3_t FpRtlIsNameLegalDOS8Dot3;
    RtlNtStatusToDosError_t FpRtlNtStatusToDosError;
    NtQueryInformationProcess_t FpNtQueryInformationProcess;
    NtQueryInformationThread_t FpNtQueryInformationThread;
    NtQueryObject_t FpNtQueryObject;
    NtQuerySystemInformation_t FpNtQuerySystemInformation;
    NtQuerySystemTime_t FpNtQuerySystemTime;
    RtlLocalTimeToSystemTime_t FpRtlLocalTimeToSystemTime;
    RtlTimeToSecondsSince1970_t FpRtlTimeToSecondsSince1970;
    RtlFreeAnsiString_t FpRtlFreeAnsiString;
    RtlFreeUnicodeString_t FpRtlFreeUnicodeString;
    RtlFreeOemString_t FpRtlFreeOemString;
    RtlInitString_t FpRtlInitString;
    RtlInitStringEx_t FpRtlInitStringEx;
    RtlInitAnsiString_t FpRtlInitAnsiString;
    RtlInitAnsiStringEx_t FpRtlInitAnsiStringEx;
    RtlInitUnicodeString_t FpRtlInitUnicodeString;
    RtlAnsiStringToUnicodeString_t FpRtlAnsiStringToUnicodeString;
    RtlUnicodeStringToAnsiString_t FpRtlUnicodeStringToAnsiString;
    RtlUnicodeStringToOemString_t FpRtlUnicodeStringToOemString;
    RtlUnicodeToMultiByteSize_t FpRtlUnicodeToMultiByteSize;
    RtlCharToInteger_t FpRtlCharToInteger;
    RtlConvertSidToUnicodeString_t FpRtlConvertSidToUnicodeString;
    RtlUniform_t FpRtlUniform;
} NTDLL;

static NTDLL ntdll = {
    NULL, // HMODULE h;
    NULL, // NtClose_t FpNtClose;
    NULL, // NtCreateFile_t FpNtCreateFile;
    NULL, // NtOpenFile_t FpNtOpenFile;
    NULL, // NtRenameKey_t FpNtRenameKey;
    NULL, // NtNotifyChangeMultipleKeys_t FpNtNotifyChangeMultipleKeys;
    NULL, // NtQueryMultipleValueKey_t FpNtQueryMultipleValueKey;
    NULL, // NtSetInformationKey_t FpNtSetInformationKey;
    NULL, // NtDeviceIoControlFile_t FpNtDeviceIoControlFile;
    NULL, // NtWaitForSingleObject_t FpNtWaitForSingleObject;
    NULL, // RtlIsNameLegalDOS8Dot3_t FpRtlIsNameLegalDOS8Dot3;
    NULL, // RtlNtStatusToDosError_t FpRtlNtStatusToDosError;
    NULL, // NtQueryInformationProcess_t FpNtQueryInformationProcess;
    NULL, // NtQueryInformationThread_t FpNtQueryInformationThread;
    NULL, // NtQueryObject_t FpNtQueryObject;
    NULL, // NtQuerySystemInformation_t FpNtQuerySystemInformation;
    NULL, // NtQuerySystemTime_t FpNtQuerySystemTime;
    NULL, // RtlLocalTimeToSystemTime_t FpRtlLocalTimeToSystemTime;
    NULL, // RtlTimeToSecondsSince1970_t FpRtlTimeToSecondsSince1970;
    NULL, // RtlFreeAnsiString_t FpRtlFreeAnsiString;
    NULL, // RtlFreeUnicodeString_t FpRtlFreeUnicodeString;
    NULL, // RtlFreeOemString_t FpRtlFreeOemString;
    NULL, // RtlInitString_t FpRtlInitString;
    NULL, // RtlInitStringEx_t FpRtlInitStringEx;
    NULL, // RtlInitAnsiString_t FpRtlInitAnsiString;
    NULL, // RtlInitAnsiStringEx_t FpRtlInitAnsiStringEx;
    NULL, // RtlInitUnicodeString_t FpRtlInitUnicodeString;
    NULL, // RtlAnsiStringToUnicodeString_t FpRtlAnsiStringToUnicodeString;
    NULL, // RtlUnicodeStringToAnsiString_t FpRtlUnicodeStringToAnsiString;
    NULL, // RtlUnicodeStringToOemString_t FpRtlUnicodeStringToOemString;
    NULL, // RtlUnicodeToMultiByteSize_t FpRtlUnicodeToMultiByteSize;
    NULL, // RtlCharToInteger_t FpRtlCharToInteger;
    NULL, // RtlConvertSidToUnicodeString_t FpRtlConvertSidToUnicodeString;
    NULL, // RtlUniform_t FpRtlUniform;
};

NTSTATUS
NTAPI
ChxNtdllInit(
    )
{
    SecureZeroMemory(&ntdll, sizeof(ntdll));
    ntdll.h = ChxGetProcessModuleHandle(L"ntdll.dll");
    if (NULL == ntdll.h)
        return STATUS_DLL_NOT_FOUND;

    ntdll.FpNtClose = ChxGetProcAddress(ntdll.h, "NtClose");
    ntdll.FpNtCreateFile = ChxGetProcAddress(ntdll.h, "NtCreateFile");
    ntdll.FpNtOpenFile = ChxGetProcAddress(ntdll.h, "NtOpenFile");
    ntdll.FpNtRenameKey = ChxGetProcAddress(ntdll.h, "NtRenameKey");
    ntdll.FpNtNotifyChangeMultipleKeys = ChxGetProcAddress(ntdll.h, "NtNotifyChangeMultipleKeys");
    ntdll.FpNtQueryMultipleValueKey = ChxGetProcAddress(ntdll.h, "NtQueryMultipleValueKey");
    ntdll.FpNtSetInformationKey = ChxGetProcAddress(ntdll.h, "NtSetInformationKey");
    ntdll.FpNtDeviceIoControlFile = ChxGetProcAddress(ntdll.h, "NtDeviceIoControlFile");
    ntdll.FpNtWaitForSingleObject = ChxGetProcAddress(ntdll.h, "NtWaitForSingleObject");
    ntdll.FpRtlIsNameLegalDOS8Dot3 = ChxGetProcAddress(ntdll.h, "RtlIsNameLegalDOS8Dot3");
    ntdll.FpRtlNtStatusToDosError = ChxGetProcAddress(ntdll.h, "RtlNtStatusToDosError");
    ntdll.FpNtQueryInformationProcess = ChxGetProcAddress(ntdll.h, "NtQueryInformationProcess");
    ntdll.FpNtQueryInformationThread = ChxGetProcAddress(ntdll.h, "NtQueryInformationThread");
    ntdll.FpNtQueryObject = ChxGetProcAddress(ntdll.h, "NtQueryObject");
    ntdll.FpNtQuerySystemInformation = ChxGetProcAddress(ntdll.h, "NtQuerySystemInformation");
    ntdll.FpNtQuerySystemTime = ChxGetProcAddress(ntdll.h, "NtQuerySystemTime");
    ntdll.FpRtlLocalTimeToSystemTime = ChxGetProcAddress(ntdll.h, "RtlLocalTimeToSystemTime");
    ntdll.FpRtlTimeToSecondsSince1970 = ChxGetProcAddress(ntdll.h, "RtlTimeToSecondsSince1970");
    ntdll.FpRtlFreeAnsiString = ChxGetProcAddress(ntdll.h, "RtlFreeAnsiString");
    ntdll.FpRtlFreeUnicodeString = ChxGetProcAddress(ntdll.h, "RtlFreeUnicodeString");
    ntdll.FpRtlFreeOemString = ChxGetProcAddress(ntdll.h, "RtlFreeOemString");
    ntdll.FpRtlInitString = ChxGetProcAddress(ntdll.h, "RtlInitString");
    ntdll.FpRtlInitStringEx = ChxGetProcAddress(ntdll.h, "RtlInitStringEx");
    ntdll.FpRtlInitAnsiString = ChxGetProcAddress(ntdll.h, "RtlInitAnsiString");
    ntdll.FpRtlInitAnsiStringEx = ChxGetProcAddress(ntdll.h, "RtlInitAnsiStringEx");
    ntdll.FpRtlInitUnicodeString = ChxGetProcAddress(ntdll.h, "RtlInitUnicodeString");
    ntdll.FpRtlAnsiStringToUnicodeString = ChxGetProcAddress(ntdll.h, "RtlAnsiStringToUnicodeString");
    ntdll.FpRtlUnicodeStringToAnsiString = ChxGetProcAddress(ntdll.h, "RtlUnicodeStringToAnsiString");
    ntdll.FpRtlUnicodeStringToOemString = ChxGetProcAddress(ntdll.h, "RtlUnicodeStringToOemString");
    ntdll.FpRtlUnicodeToMultiByteSize = ChxGetProcAddress(ntdll.h, "RtlUnicodeToMultiByteSize");
    ntdll.FpRtlCharToInteger = ChxGetProcAddress(ntdll.h, "RtlCharToInteger");
    ntdll.FpRtlConvertSidToUnicodeString = ChxGetProcAddress(ntdll.h, "RtlConvertSidToUnicodeString");
    ntdll.FpRtlUniform = ChxGetProcAddress(ntdll.h, "RtlUniform");
    return STATUS_SUCCESS;
}

NTSTATUS
NTAPI
ChxNtClose (
    HANDLE Handle
    )
{
    return (NULL == ntdll.FpNtClose)
        ? STATUS_PROCEDURE_NOT_FOUND
        : ntdll.FpNtClose(Handle);
}

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
    )
{
    return (NULL == ntdll.FpNtCreateFile)
        ? STATUS_PROCEDURE_NOT_FOUND
        : ntdll.FpNtCreateFile(
            FileHandle,
            DesiredAccess,
            ObjectAttributes,
            IoStatusBlock,
            AllocationSize,
            FileAttributes,
            ShareAccess,
            CreateDisposition,
            CreateOptions,
            EaBuffer,
            EaLength);
}

NTSTATUS
NTAPI
ChxNtOpenFile (
    PHANDLE FileHandle,
    ACCESS_MASK DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    PIO_STATUS_BLOCK IoStatusBlock,
    ULONG ShareAccess,
    ULONG OpenOptions
    )
{
    return (NULL == ntdll.FpNtOpenFile)
        ? STATUS_PROCEDURE_NOT_FOUND
        : ntdll.FpNtOpenFile(
            FileHandle,
            DesiredAccess,
            ObjectAttributes,
            IoStatusBlock,
            ShareAccess,
            OpenOptions
            );
}

NTSTATUS
NTAPI
ChxNtRenameKey (
    HANDLE KeyHandle,
    PUNICODE_STRING NewName
    )
{
    return (NULL == ntdll.FpNtRenameKey)
        ? STATUS_PROCEDURE_NOT_FOUND
        : ntdll.FpNtRenameKey(KeyHandle, NewName);
}

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
    )
{
    return (NULL == ntdll.FpNtNotifyChangeMultipleKeys)
        ? STATUS_PROCEDURE_NOT_FOUND
        : ntdll.FpNtNotifyChangeMultipleKeys(
            MasterKeyHandle,
            Count,
            SubordinateObjects,
            Event,
            ApcRoutine,
            ApcContext,
            IoStatusBlock,
            CompletionFilter,
            WatchTree,
            Buffer,
            BufferSize,
            Asynchronous
            );
}

NTSTATUS
NTAPI
ChxNtQueryMultipleValueKey (
    HANDLE KeyHandle,
    PKEY_VALUE_ENTRY ValueEntries,
    ULONG EntryCount,
    PVOID ValueBuffer,
    PULONG BufferLength,
    PULONG RequiredBufferLength
    )
{
    return (NULL == ntdll.FpNtQueryMultipleValueKey)
        ? STATUS_PROCEDURE_NOT_FOUND
        : ntdll.FpNtQueryMultipleValueKey(
            KeyHandle,
            ValueEntries,
            EntryCount,
            ValueBuffer,
            BufferLength,
            RequiredBufferLength
            );
}

NTSTATUS
NTAPI
ChxNtSetInformationKey (
    HANDLE KeyHandle,
    KEY_SET_INFORMATION_CLASS KeySetInformationClass,
    PVOID KeySetInformation,
    ULONG KeySetInformationLength
    )
{
    return (NULL == ntdll.FpNtSetInformationKey)
        ? STATUS_PROCEDURE_NOT_FOUND
        : ntdll.FpNtSetInformationKey(
            KeyHandle,
            KeySetInformationClass,
            KeySetInformation,
            KeySetInformationLength
            );
}

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
    )
{
    return (NULL == ntdll.FpNtDeviceIoControlFile)
        ? STATUS_PROCEDURE_NOT_FOUND
        : ntdll.FpNtDeviceIoControlFile(
            FileHandle,
            Event,
            ApcRoutine,
            ApcContext,
            IoStatusBlock,
            IoControlCode,
            InputBuffer,
            InputBufferLength,
            OutputBuffer,
            OutputBufferLength
            );
}

NTSTATUS
NTAPI
ChxNtWaitForSingleObject (
    HANDLE Handle,
    BOOLEAN Alertable,
    PLARGE_INTEGER Timeout
    )
{
    return (NULL == ntdll.FpNtWaitForSingleObject)
        ? STATUS_PROCEDURE_NOT_FOUND
        : ntdll.FpNtWaitForSingleObject(Handle, Alertable, Timeout);
}

BOOLEAN
NTAPI
ChxRtlIsNameLegalDOS8Dot3 (
    PUNICODE_STRING Name,
    POEM_STRING OemName,
    PBOOLEAN NameContainsSpaces
    )
{
    return (NULL == ntdll.FpRtlIsNameLegalDOS8Dot3)
        ? FALSE
        : ntdll.FpRtlIsNameLegalDOS8Dot3(Name, OemName, NameContainsSpaces);
}

ULONG
NTAPI
ChxRtlNtStatusToDosError (
   NTSTATUS Status
   )
{
    return (NULL == ntdll.FpRtlNtStatusToDosError)
        ? 0
        : ntdll.FpRtlNtStatusToDosError(Status);
}

NTSTATUS
NTAPI
ChxNtQueryInformationProcess (
    HANDLE ProcessHandle,
    PROCESSINFOCLASS ProcessInformationClass,
    PVOID ProcessInformation,
    ULONG ProcessInformationLength,
    PULONG ReturnLength
    )
{
    return (NULL == ntdll.FpNtQueryInformationProcess)
        ? STATUS_PROCEDURE_NOT_FOUND
        : ntdll.FpNtQueryInformationProcess(
            ProcessHandle,
            ProcessInformationClass,
            ProcessInformation,
            ProcessInformationLength,
            ReturnLength
            );
}

NTSTATUS
NTAPI
ChxNtQueryInformationThread (
    HANDLE ThreadHandle,
    THREADINFOCLASS ThreadInformationClass,
    PVOID ThreadInformation,
    ULONG ThreadInformationLength,
    PULONG ReturnLength
    )
{
    return (NULL == ntdll.FpNtQueryInformationThread)
        ? STATUS_PROCEDURE_NOT_FOUND
        : ntdll.FpNtQueryInformationThread(
            ThreadHandle,
            ThreadInformationClass,
            ThreadInformation,
            ThreadInformationLength,
            ReturnLength
            );
}

NTSTATUS
NTAPI
ChxNtQueryObject (
    HANDLE Handle,
    OBJECT_INFORMATION_CLASS ObjectInformationClass,
    PVOID ObjectInformation,
    ULONG ObjectInformationLength,
    PULONG ReturnLength
    )
{
    return (NULL == ntdll.FpNtQueryObject)
        ? STATUS_PROCEDURE_NOT_FOUND
        : ntdll.FpNtQueryObject(
            Handle,
            ObjectInformationClass,
            ObjectInformation,
            ObjectInformationLength,
            ReturnLength
            );
}

NTSTATUS
NTAPI
ChxNtQuerySystemInformation (
    SYSTEM_INFORMATION_CLASS SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength
    )
{
    return (NULL == ntdll.FpNtQuerySystemInformation)
        ? STATUS_PROCEDURE_NOT_FOUND
        : ntdll.FpNtQuerySystemInformation(
            SystemInformationClass,
            SystemInformation,
            SystemInformationLength,
            ReturnLength
            );
}

NTSTATUS
NTAPI
ChxNtQuerySystemTime (
    PLARGE_INTEGER SystemTime
    )
{
    return (NULL == ntdll.FpNtQuerySystemTime)
        ? STATUS_PROCEDURE_NOT_FOUND
        : ntdll.FpNtQuerySystemTime(SystemTime);
}

NTSTATUS
NTAPI
ChxRtlLocalTimeToSystemTime (
    PLARGE_INTEGER LocalTime,
    PLARGE_INTEGER SystemTime
    )
{
    return (NULL == ntdll.FpRtlLocalTimeToSystemTime)
        ? STATUS_PROCEDURE_NOT_FOUND
        : ntdll.FpRtlLocalTimeToSystemTime(LocalTime, SystemTime);
}

BOOLEAN
NTAPI
ChxRtlTimeToSecondsSince1970 (
    PLARGE_INTEGER Time,
    PULONG ElapsedSeconds
    )
{
    return (NULL == ntdll.FpRtlTimeToSecondsSince1970)
        ? FALSE
        : ntdll.FpRtlTimeToSecondsSince1970(Time, ElapsedSeconds);
}

VOID
NTAPI
ChxRtlFreeAnsiString (
    PANSI_STRING AnsiString
    )
{
    if (NULL == ntdll.FpRtlFreeAnsiString)
        ntdll.FpRtlFreeAnsiString(AnsiString);
}

VOID
NTAPI
ChxRtlFreeUnicodeString (
    PUNICODE_STRING UnicodeString
    )
{
    if (NULL == ntdll.FpRtlFreeUnicodeString)
        ntdll.FpRtlFreeUnicodeString(UnicodeString);
}

VOID
NTAPI
ChxRtlFreeOemString(
    POEM_STRING OemString
    )
{
    if (NULL == ntdll.FpRtlFreeOemString)
        ntdll.FpRtlFreeOemString(OemString);
}

VOID
NTAPI
ChxRtlInitString (
    PSTRING DestinationString,
    PCSZ SourceString
    )
{
    if (NULL == ntdll.FpRtlInitString)
        ntdll.FpRtlInitString(DestinationString, SourceString);
}

NTSTATUS
NTAPI
ChxRtlInitStringEx (
    PSTRING DestinationString,
    PCSZ SourceString
    )
{
    return (NULL == ntdll.FpRtlInitStringEx)
        ? STATUS_PROCEDURE_NOT_FOUND
        : ntdll.FpRtlInitStringEx(DestinationString, SourceString);
}

VOID
NTAPI
ChxRtlInitAnsiString (
    PANSI_STRING DestinationString,
    PCSZ SourceString
    )
{
    if (NULL == ntdll.FpRtlInitAnsiString)
        ntdll.FpRtlInitAnsiString(DestinationString, SourceString);
}

NTSTATUS
NTAPI
ChxRtlInitAnsiStringEx (
    PANSI_STRING DestinationString,
    PCSZ SourceString
    )
{
    return (NULL == ntdll.FpRtlInitAnsiStringEx)
        ? STATUS_PROCEDURE_NOT_FOUND
        : ntdll.FpRtlInitAnsiStringEx(DestinationString, SourceString);
}

VOID
NTAPI
ChxRtlInitUnicodeString (
    PUNICODE_STRING DestinationString,
    PCWSTR SourceString
    )
{
    if (NULL == ntdll.FpRtlInitUnicodeString)
        ntdll.FpRtlInitUnicodeString(DestinationString, SourceString);
}

NTSTATUS
NTAPI
ChxRtlAnsiStringToUnicodeString (
    PUNICODE_STRING DestinationString,
    PCANSI_STRING SourceString,
    BOOLEAN AllocateDestinationString
    )
{
    return (NULL == ntdll.FpRtlAnsiStringToUnicodeString)
        ? STATUS_PROCEDURE_NOT_FOUND
        : ntdll.FpRtlAnsiStringToUnicodeString(DestinationString, SourceString, AllocateDestinationString);
}

NTSTATUS
NTAPI
ChxRtlUnicodeStringToAnsiString (
    PANSI_STRING DestinationString,
    PCUNICODE_STRING SourceString,
    BOOLEAN AllocateDestinationString
    )
{
    return (NULL == ntdll.FpRtlUnicodeStringToAnsiString)
        ? STATUS_PROCEDURE_NOT_FOUND
        : ntdll.FpRtlUnicodeStringToAnsiString(DestinationString, SourceString, AllocateDestinationString);
}

NTSTATUS
NTAPI
ChxRtlUnicodeStringToOemString(
    POEM_STRING DestinationString,
    PCUNICODE_STRING SourceString,
    BOOLEAN AllocateDestinationString
    )
{
    return (NULL == ntdll.FpRtlUnicodeStringToOemString)
        ? STATUS_PROCEDURE_NOT_FOUND
        : ntdll.FpRtlUnicodeStringToOemString(DestinationString, SourceString, AllocateDestinationString);
}

NTSTATUS
NTAPI
ChxRtlUnicodeToMultiByteSize(
    PULONG BytesInMultiByteString,
    PWCH UnicodeString,
    ULONG BytesInUnicodeString
    )
{
    return (NULL == ntdll.FpRtlUnicodeToMultiByteSize)
        ? STATUS_PROCEDURE_NOT_FOUND
        : ntdll.FpRtlUnicodeToMultiByteSize(BytesInMultiByteString, UnicodeString, BytesInUnicodeString);
}

NTSTATUS
NTAPI
ChxRtlCharToInteger (
    PCSZ String,
    ULONG Base,
    PULONG Value
    )
{
    return (NULL == ntdll.FpRtlCharToInteger)
        ? STATUS_PROCEDURE_NOT_FOUND
        : ntdll.FpRtlCharToInteger(String, Base, Value);
}

NTSTATUS
NTAPI
ChxRtlConvertSidToUnicodeString (
    PUNICODE_STRING UnicodeString,
    PSID Sid,
    BOOLEAN AllocateDestinationString
    )
{
    return (NULL == ntdll.FpRtlConvertSidToUnicodeString)
        ? STATUS_PROCEDURE_NOT_FOUND
        : ntdll.FpRtlConvertSidToUnicodeString(UnicodeString, Sid, AllocateDestinationString);
}

ULONG
NTAPI
ChxRtlUniform (
    PULONG Seed
    )
{
    return (NULL == ntdll.FpRtlUniform)
        ? 0
        : ntdll.FpRtlUniform(Seed);
}



#endif  // _WIN32
