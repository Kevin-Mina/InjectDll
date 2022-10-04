#include "Resource.h"
#include "..\libdrv\inc\lib.h"


WCHAR g_FullDllPathName[MAX_PATH];
UNICODE_STRING g_us_FullDllPathName;//��ע��DLL��ȫ·����

WCHAR g_FullDllPathNameWow64[MAX_PATH];
UNICODE_STRING g_us_FullDllPathNameWow64;


PUNICODE_STRING g_RegistryPath;


BOOL set_dll_full_path(PUNICODE_STRING dll_full_path_name)
/*
���ܣ���ȡsvchost.exe���������ļ���ȫ·����
      ·����ʽ��\Device\HarddiskVolume1\WINDOWS\system32\DLL.DLL
      ֮������NTʽ������DOSʽ������Ϊ�ļ����������з��صĴ����NTʽ��
      ����Ҫ��IoQueryFileDosDeviceName������Ĺ��ܡ�
ע�⣺��64λϵͳ�л���:"\\KnownDlls32\\KnownDllPath".

csrss.exe��·��������������ȡ���������û����ݹ�����PID��
*/
{
    ULONG ActualLength;
    HANDLE LinkHandle;
    WCHAR DosPath[MAX_PATH];
    OBJECT_ATTRIBUTES ObjectAttributes;
    UNICODE_STRING LinkString, NameString;
    WCHAR pszDest[8]; //����L"\\??\\%c:"�ĳ��ȡ�
    LPCWSTR pszFormat = L"\\??\\%c:";//L"\\DosDevices\\%c:"  L"\\??\\%c:"
    NTSTATUS status = STATUS_SUCCESS;

    //����һ����ȡL"\\KnownDlls\\KnownDllPath"��ֵ���磺C:\WINDOWS\system32
    LinkString.Buffer = DosPath;
    LinkString.MaximumLength = sizeof(DosPath);
    RtlZeroMemory(DosPath, sizeof(DosPath));
    RtlInitUnicodeString(&NameString, L"\\KnownDlls\\KnownDllPath");//��������//,��Ȼ��ZwOpenSymbolicLinkObject����ʧ��.���ǵõ��ľ��Ϊ0.
    InitializeObjectAttributes(&ObjectAttributes, &NameString, OBJ_KERNEL_HANDLE, NULL, NULL);
    status = ZwOpenSymbolicLinkObject(&LinkHandle, SYMBOLIC_LINK_QUERY, &ObjectAttributes);
    if (!NT_SUCCESS(status)) {
        return FALSE;
    }
    status = ZwQuerySymbolicLinkObject(LinkHandle, &LinkString, &ActualLength);//LinkString������Ҫ��ֵ.
    if (!NT_SUCCESS(status)) {
        return FALSE;
    }
    //KdPrint(("KnownDllPath: %wZ \n",&LinkString));//���磺C:\WINDOWS\system32
    ZwClose(LinkHandle);

    {
        g_us_FullDllPathName.Buffer = g_FullDllPathName;
        g_us_FullDllPathName.Length = MAX_PATH * sizeof(wchar_t);
        g_us_FullDllPathName.MaximumLength = g_us_FullDllPathName.Length;
        RtlCopyUnicodeString(&g_us_FullDllPathName, &LinkString);
        g_us_FullDllPathName.MaximumLength = MAX_PATH * sizeof(wchar_t);
        status = RtlAppendUnicodeToString(&g_us_FullDllPathName, L"\\dll.dll");
        ASSERT(NT_SUCCESS(status));
    }

    //���������ȡ���ֵ���磺C��Ӧ��"\Device\HarddiskVolume1"��
    status = RtlStringCbPrintfW(pszDest, 8 * sizeof(WCHAR), pszFormat, LinkString.Buffer[0]);//��ʽ���ַ�����
    RtlInitUnicodeString(&NameString, pszDest);//ע���ʽ��L"\\??\\c:"
    InitializeObjectAttributes(&ObjectAttributes, &NameString, OBJ_KERNEL_HANDLE, NULL, NULL);
    status = ZwOpenSymbolicLinkObject(&LinkHandle, SYMBOLIC_LINK_QUERY | GENERIC_READ, &ObjectAttributes);
    if (!NT_SUCCESS(status)) {
        return FALSE;
    }
    status = ZwQuerySymbolicLinkObject(LinkHandle, dll_full_path_name, &ActualLength);
    if (!NT_SUCCESS(status)) {
        ZwClose(LinkHandle);
        return FALSE;
    }
    //KdPrint(("%wZ \r\n",&LinkString));//�õ���ֵ���磺"\Device\HarddiskVolume1"��
    ZwClose(LinkHandle);

    //�����������·����    
    status = RtlAppendUnicodeToString(dll_full_path_name, &LinkString.Buffer[2]);//������C:\WINDOWS\system32��ǰ���ַ����磺x:��
    if (!NT_SUCCESS(status)) {
        return FALSE;
    }
    status = RtlAppendUnicodeToString(dll_full_path_name, L"\\dll.dll");
    if (!NT_SUCCESS(status)) {
        return FALSE;
    }
    //KdPrint(("%wZ \r\n",&NtString));

    return TRUE;
}


BOOL set_dll_full_path_wow64(PUNICODE_STRING dll_full_path_name)
/*
���ܣ���ȡsvchost.exe���������ļ���ȫ·����
      ·����ʽ��\Device\HarddiskVolume1\WINDOWS\system32\DLL.DLL
      ֮������NTʽ������DOSʽ������Ϊ�ļ����������з��صĴ����NTʽ��
      ����Ҫ��IoQueryFileDosDeviceName������Ĺ��ܡ�
ע�⣺��64λϵͳ�л���:"\\KnownDlls32\\KnownDllPath".

csrss.exe��·��������������ȡ���������û����ݹ�����PID��
*/
{
    ULONG ActualLength;
    HANDLE LinkHandle;
    WCHAR DosPath[MAX_PATH];
    OBJECT_ATTRIBUTES ObjectAttributes;
    UNICODE_STRING LinkString, NameString;
    WCHAR pszDest[8]; //����L"\\??\\%c:"�ĳ��ȡ�
    LPCWSTR pszFormat = L"\\??\\%c:";//L"\\DosDevices\\%c:"  L"\\??\\%c:"
    NTSTATUS status = STATUS_SUCCESS;

    //����һ����ȡL"\\KnownDlls\\KnownDllPath"��ֵ���磺C:\WINDOWS\system32
    LinkString.Buffer = DosPath;
    LinkString.MaximumLength = sizeof(DosPath);
    RtlZeroMemory(DosPath, sizeof(DosPath));
    RtlInitUnicodeString(&NameString, L"\\KnownDlls32\\KnownDllPath");//��������//,��Ȼ��ZwOpenSymbolicLinkObject����ʧ��.���ǵõ��ľ��Ϊ0.
    InitializeObjectAttributes(&ObjectAttributes, &NameString, OBJ_KERNEL_HANDLE, NULL, NULL);
    status = ZwOpenSymbolicLinkObject(&LinkHandle, SYMBOLIC_LINK_QUERY, &ObjectAttributes);
    if (!NT_SUCCESS(status)) {
        return FALSE;
    }
    status = ZwQuerySymbolicLinkObject(LinkHandle, &LinkString, &ActualLength);//LinkString������Ҫ��ֵ.
    if (!NT_SUCCESS(status)) {
        return FALSE;
    }
    //KdPrint(("KnownDllPath: %wZ \n",&LinkString));//���磺C:\WINDOWS\system32
    ZwClose(LinkHandle);

    {
        g_us_FullDllPathNameWow64.Buffer = g_FullDllPathNameWow64;
        g_us_FullDllPathNameWow64.Length = MAX_PATH * sizeof(wchar_t);
        g_us_FullDllPathNameWow64.MaximumLength = g_us_FullDllPathNameWow64.Length;
        RtlCopyUnicodeString(&g_us_FullDllPathNameWow64, &LinkString);
        g_us_FullDllPathNameWow64.MaximumLength = MAX_PATH * sizeof(wchar_t);
        status = RtlAppendUnicodeToString(&g_us_FullDllPathNameWow64, L"\\dll.dll");
        ASSERT(NT_SUCCESS(status));
    }

    //���������ȡ���ֵ���磺C��Ӧ��"\Device\HarddiskVolume1"��
    status = RtlStringCbPrintfW(pszDest, 8 * sizeof(WCHAR), pszFormat, LinkString.Buffer[0]);//��ʽ���ַ�����
    RtlInitUnicodeString(&NameString, pszDest);//ע���ʽ��L"\\??\\c:"
    InitializeObjectAttributes(&ObjectAttributes, &NameString, OBJ_KERNEL_HANDLE, NULL, NULL);
    status = ZwOpenSymbolicLinkObject(&LinkHandle, SYMBOLIC_LINK_QUERY | GENERIC_READ, &ObjectAttributes);
    if (!NT_SUCCESS(status)) {
        return FALSE;
    }
    status = ZwQuerySymbolicLinkObject(LinkHandle, dll_full_path_name, &ActualLength);
    if (!NT_SUCCESS(status)) {
        ZwClose(LinkHandle);
        return FALSE;
    }
    //KdPrint(("%wZ \r\n",&LinkString));//�õ���ֵ���磺"\Device\HarddiskVolume1"��
    ZwClose(LinkHandle);

    //�����������·����    
    status = RtlAppendUnicodeToString(dll_full_path_name, &LinkString.Buffer[2]);//������C:\WINDOWS\system32��ǰ���ַ����磺x:��
    if (!NT_SUCCESS(status)) {
        return FALSE;
    }
    status = RtlAppendUnicodeToString(dll_full_path_name, L"\\dll.dll");
    if (!NT_SUCCESS(status)) {
        return FALSE;
    }
    //KdPrint(("%wZ \r\n",&NtString));

    return TRUE;
}


void get_sys_path(UNICODE_STRING * sys_full_path_name)
{
    NTSTATUS status;
    OBJECT_ATTRIBUTES attributes;
    HANDLE driverRegKey = NULL;
    UNICODE_STRING valueName;
    ULONG ResultLength;
    PKEY_VALUE_PARTIAL_INFORMATION pkvpi;
    UNICODE_STRING test;
    UNICODE_STRING directory = RTL_CONSTANT_STRING(L"\\DosDevices");//\\C:\\test.sys

    InitializeObjectAttributes(&attributes, g_RegistryPath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
    status = ZwOpenKey(&driverRegKey, KEY_READ, &attributes);
    ASSERT(NT_SUCCESS(status));

    RtlInitUnicodeString(&valueName, L"ImagePath");
    status = ZwQueryValueKey(driverRegKey, &valueName, KeyValuePartialInformation, NULL, 0, &ResultLength);
    ASSERT(status == STATUS_BUFFER_TOO_SMALL || status == STATUS_BUFFER_OVERFLOW);
    pkvpi = ExAllocatePoolWithTag(NonPagedPool, ResultLength, TAG);
    ASSERT(pkvpi != NULL);
    status = ZwQueryValueKey(driverRegKey, &valueName, KeyValuePartialInformation, pkvpi, ResultLength, &ResultLength);
    ASSERT(NT_SUCCESS(status));

    //sys_full_path_name->Length = (USHORT)pkvpi->DataLength;
    //RtlCopyMemory(sys_full_path_name->Buffer, pkvpi->Data, pkvpi->DataLength);
    //"\??\C:\Documents and Settings\Administrator\����\test.sys"

    //test.Buffer = (wchar_t *)((char *)pkvpi->Data + 6);
    //test.Length = (USHORT)pkvpi->DataLength - 6;
    //test.MaximumLength = test.Length ;
    //RtlCopyUnicodeString(sys_full_path_name, &directory);
    //status = RtlAppendUnicodeStringToString(sys_full_path_name, &test);
    //ASSERT (NT_SUCCESS(status)) ;

    ExFreePoolWithTag(pkvpi, TAG);
    status = ZwClose(driverRegKey);
}


//BOOLEAN create_dll_XXX(IN UNICODE_STRING * FileName, IN UNICODE_STRING * newFileName) //CONST
//    /*
//    ��������ʽ�ǣ�"\Device\HarddiskVolume1\XXX����\\??\\c:\\WINDOWS\\system32\\config\\SAM��
//
//    ���ܣ����ƶ�ռʽ�ĺͱ�������ZwLockFile�����ļ���
//
//    ˵����
//    1.IoCreateFileEx������IO_IGNORE_SHARE_ACCESS_CHECK���ܣ�����This routine is available starting with Windows Vista.
//    2.����ר�Ÿ��Ʊ���ռʽʹ�õ��ļ������ҳ�ļ�������ʹ�õ�pagefile.sys���͸��ֱ�����ʹ��HIVE�ļ�.
//    3.��չ���ܣ���ɾ���ļ����򿪵�ʱ�����ɾ�������ԣ�FILE_DELETE_ON_CLOSE ������Ҳ���Եģ�������÷���IRP����������ʽ�ϡ�
//
//    ���ڵ�ȱ����:
//    1.û�и����ļ�������,��:�ļ��������ߵ���Ϣ.
//    */
//{
//    BOOLEAN b = FALSE;
//    NTSTATUS status = STATUS_UNSUCCESSFUL;
//    OBJECT_ATTRIBUTES ob;
//    HANDLE FileHandle = 0;
//    HANDLE DestinationFileHandle = 0;
//    IO_STATUS_BLOCK  IoStatusBlock = {0};
//    PVOID Buffer = 0;
//    SIZE_T Length = 0;
//    ULONG CreateDisposition = 0;
//    FILE_STANDARD_INFORMATION fsi = {0};
//    LARGE_INTEGER ByteOffset = {0};
//    LARGE_INTEGER AllocationSize = {0};
//    LARGE_INTEGER file_size = {0};
//    FILE_FULL_EA_INFORMATION ffai = {0};
//    HANDLE SectionHandle = 0;
//    PVOID BaseAddress = 0;
//    SIZE_T ViewSize = 0;
//    int i;
//    PVOID BaseAddress2 = 0;
//    PVOID p;
//
//    InitializeObjectAttributes(&ob, FileName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, 0, 0);
//    //status = ZwOpenFile(&FileHandle, GENERIC_READ | SYNCHRONIZE, &ob, &IoStatusBlock, FILE_SHARE_READ, FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT);
//    //if (!NT_SUCCESS (status)) 
//    //{
//    //    //KdPrint(("ZwOpenFile fail with 0x%x.\n", status));
//    //    if ( status == STATUS_OBJECT_NAME_NOT_FOUND)  {
//    //        KdPrint(("file does not exist\n"));
//    //    }
//    //    if (IoStatusBlock.Information == FILE_DOES_NOT_EXIST ) {
//    //        KdPrint(("file does not exist\n"));
//    //    }
//    //    return b;
//    //}
//    status = IoCreateFileSpecifyDeviceObjectHint(&FileHandle,
//                                                 GENERIC_READ | SYNCHRONIZE,
//                                                 &ob,
//                                                 &IoStatusBlock,
//                                                 &AllocationSize,
//                                                 FILE_ATTRIBUTE_NORMAL,
//                                                 /*
//                                                 Specifies the type of share access to the file that the caller would like, as zero, or one, or a combination of the following flags.
//                                                 To request exclusive access, set this parameter to zero.
//                                                 If the IO_IGNORE_SHARE_ACCESS_CHECK flag is specified in the Options parameter, the I/O manager ignores this parameter.
//                                                 However, the file system might still perform access checks.
//                                                 Thus, it is important to specify the sharing mode you would like for this parameter, even when using the IO_IGNORE_SHARE_ACCESS_CHECK flag.
//                                                 For the greatest chance of avoiding sharing violation errors, specify all of the following share access flags.
//                                                 */
//                                                 FILE_SHARE_VALID_FLAGS,
//                                                 FILE_OPEN,
//                                                 FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT,
//                                                 &ffai,
//                                                 sizeof(FILE_FULL_EA_INFORMATION),
//                                                 CreateFileTypeNone,//��ʵ�����ܵ����ʼ���Ҳ�����ˡ�
//                                                 NULL,
//                                                 /*
//                                                 Indicates that the I/O manager should not perform share-access checks on the file object after it is created.
//                                                 However, the file system might still perform these checks.
//                                                 */
//                                                 IO_IGNORE_SHARE_ACCESS_CHECK,
//                                                 /*
//                                                 A pointer to the device object to which the create request is to be sent.
//                                                 The device object must be a filter or file system device object in the file system driver stack for the volume on which the file or directory resides.
//                                                 This parameter is optional and can be NULL. If this parameter is NULL, the request will be sent to the device object at the top of the driver stack.
//                                                 */
//                                                 NULL
//    );
//    if (!NT_SUCCESS(status)) {
//        //KdPrint(("ZwOpenFile fail with 0x%x.\n", status));
//        if (status == STATUS_OBJECT_NAME_NOT_FOUND) {
//            KdPrint(("file does not exist\n"));
//        }
//        if (IoStatusBlock.Information == FILE_DOES_NOT_EXIST) {
//            KdPrint(("file does not exist\n"));
//        }
//        return b;
//    }
//
//    //���Կ�����������ļ�����,����,���ñ�Ĳ�����д��.ZwLockFile,�����ʵ���ʱ�����:ZwUnlockFile.
//    //����This routine is available in Windows 7 and later versions of the Windows operating system.
//    //����NtLockFile��NtUnlockFile��XP�µ���������ʹ�á�
//
//    status = ZwQueryInformationFile(FileHandle, &IoStatusBlock, &fsi, sizeof(FILE_STANDARD_INFORMATION), FileStandardInformation);
//    if (!NT_SUCCESS(status)) {
//        KdPrint(("ZwQueryInformationFile fail with 0x%x.\n", status));
//        ZwClose(FileHandle);
//        return b;;
//    }
//
//    //�½��ļ�.
//    CreateDisposition = FILE_OVERWRITE_IF;
//    InitializeObjectAttributes(&ob, newFileName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, 0, 0);
//    status = ZwCreateFile(&DestinationFileHandle,
//                          FILE_ALL_ACCESS | SYNCHRONIZE,
//                          &ob,
//                          &IoStatusBlock,
//                          &AllocationSize,
//                          FILE_ATTRIBUTE_NORMAL,
//                          FILE_SHARE_WRITE,
//                          CreateDisposition,
//                          FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT,
//                          NULL,
//                          0);
//    if (!NT_SUCCESS(status)) {
//        //KdPrint(("ZwCreateFile fail with 0x%x.\n", status));
//        ZwClose(FileHandle);
//        if (status == STATUS_OBJECT_NAME_COLLISION) {//-1073741771 ((NTSTATUS)0xC0000035L) Object Name already exists.
//            b = TRUE;
//        }
//        return b;
//    }
//
//    //�ļ���СΪ�㣬�ͽ����ˡ�
//    if (fsi.EndOfFile.QuadPart == 0) {
//        ZwClose(FileHandle);
//        ZwClose(DestinationFileHandle);
//        return TRUE;
//    }
//
//    //���������4G���ļ���
//    if (fsi.EndOfFile.HighPart != 0) {
//        ZwClose(FileHandle);
//        ZwClose(DestinationFileHandle);
//        return TRUE;
//    }
//
//    file_size = fsi.EndOfFile;
//    //Length = PAGE_SIZE;//����ר�á�
//    //Buffer = ExAllocatePoolWithTag(NonPagedPool, Length, TAG);//Length == 0ʱ��������֤���������������
//    //if (Buffer == NULL) { 
//    //    status = STATUS_UNSUCCESSFUL;
//    //    DbgPrint("����������ļ�Ϊ:%s, ������Ϊ:%d\n", __FILE__, __LINE__);
//    //    ZwClose(FileHandle);
//    //    ZwClose(DestinationFileHandle);
//    //    return b;
//    //}
//
//    InitializeObjectAttributes(&ob, NULL, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, 0, 0);//���Բ���������������·����
//    status = ZwCreateSection(&SectionHandle,
//                             SECTION_MAP_READ | SECTION_QUERY,
//                             &ob,
//                             &fsi.EndOfFile,
//                             PAGE_READONLY,
//                             SEC_COMMIT,
//                             FileHandle);
//    if (!NT_SUCCESS(status)) {
//        KdPrint(("ZwCreateSection fail with 0x%x.\n", status));
//        //ExFreePoolWithTag(Buffer, TAG);
//        ZwClose(FileHandle);
//        ZwClose(DestinationFileHandle);
//        return b;
//    }
//
//    /*
//    ����һҳ����һҳ���ݵĶ�ȡ�ġ�
//    ����������ȫ����ȡ�ˡ�
//    */
//    //for ( ;ByteOffset.QuadPart < file_size.QuadPart ; ) 
//    {
//        ULONG_PTR ResourceIdPath[3];
//        PIMAGE_RESOURCE_DATA_ENTRY ResourceDataEntry = NULL;
//        PVOID  MessageData;
//        ULONG Size = 0;
//
//        //RtlZeroMemory(Buffer, Length);
//
//        //status = ZwReadFile(FileHandle, NULL, NULL, NULL, &IoStatusBlock, Buffer, Length, &ByteOffset, NULL);
//        //if (!NT_SUCCESS (status)) //���ڴ������ļ��Ĵ򿪻�ʧ�ܡ�
//        //{
//        //    KdPrint(("ZwReadFile fail with 0x%x.\n", status));
//        //    ExFreePoolWithTag(Buffer, TAG);
//        //    ZwClose(FileHandle);
//        //    ZwClose(DestinationFileHandle);
//        //    return b;
//        //}
//
//        //ע�⣺�����Ȩ�޺������Ȩ��Ҫ��Ӧ��
//        status = ZwMapViewOfSection(SectionHandle,
//                                    ZwCurrentProcess(),
//                                    &BaseAddress,
//                                    0,
//                                    0,
//                                    NULL,
//                                    &ViewSize,
//                                    ViewShare,
//                                    0,
//                                    PAGE_READONLY);
//        if (!NT_SUCCESS(status)) {
//            KdPrint(("ZwMapViewOfSection fail with 0x%x.\n", status));
//            //ExFreePoolWithTag(Buffer, TAG);
//            ZwClose(SectionHandle);
//            ZwClose(FileHandle);
//            ZwClose(DestinationFileHandle);
//            return b;
//        }
//
//        i = *(int *)BaseAddress;
//        BaseAddress2 = GetImageBase("test.sys");//�����ZwMapViewOfSection�ȼ���������û���õġ�
//
//        //��ȡDLL�����ݡ�
//        //ResourceDataEntry = (PIMAGE_RESOURCE_DATA_ENTRY)ExAllocatePoolWithTag( NonPagedPool, fsi.EndOfFile.LowPart, TAG);//ntdll.dll�е�LdrFindResource_U���������������Ҫ���룬�����쳣��
//        //ASSERT(ResourceDataEntry);
//        //RtlZeroMemory(ResourceDataEntry, fsi.EndOfFile.LowPart);
//        ResourceIdPath[0] = 10;//RT_RCDATA RCDATA 10
//        ResourceIdPath[1] = 0x1391;//0x1391 == 5009
//        ResourceIdPath[2] = 0;
//        status = LdrFindResource_U(BaseAddress2, ResourceIdPath, 3, &ResourceDataEntry);//��ZwMapViewOfSection����c000008a
//        ASSERT(NT_SUCCESS(status));
//        status = LdrAccessResource(BaseAddress2, ResourceDataEntry, &MessageData, &Size);
//        ASSERT(NT_SUCCESS(status));
//
//        //���Ҫ�������4G��������Ӹ�ѭ������������4G������Ҳ����ӳ��ɹ���
//        status = ZwWriteFile(DestinationFileHandle, NULL, NULL, NULL, &IoStatusBlock, MessageData, Size, &ByteOffset, NULL);
//        if (!NT_SUCCESS(status)) {
//            KdPrint(("ZwWriteFile fail with 0x%x.\n", status));
//            ExFreePoolWithTag(ResourceDataEntry, TAG);
//            //ExFreePoolWithTag(Buffer, TAG);
//            ZwUnmapViewOfSection(NtCurrentProcess(), BaseAddress);
//            ZwClose(SectionHandle);
//            ZwClose(FileHandle);
//            ZwClose(DestinationFileHandle);
//            return b;
//        }
//
//        //ByteOffset.QuadPart += IoStatusBlock.Information;
//
//        //ExFreePoolWithTag(ResourceDataEntry, TAG );//�������Ҫ�ͷţ������������÷�����ExpInitializeExecutive����Դ�롣
//        ZwUnmapViewOfSection(NtCurrentProcess(), BaseAddress);
//    }
//
//    //ExFreePoolWithTag(Buffer, TAG);
//    ZwClose(SectionHandle);
//    ZwClose(FileHandle);
//    ZwClose(DestinationFileHandle);
//
//    return TRUE;
//}


void BuildDLL()
{
    UNICODE_STRING sys_full_path_name = {0};
    UNICODE_STRING dll_full_path_name = {0};
    UNICODE_STRING dll_full_path_name_wow64 = {0};

    /*
    ���԰Ѱ�DLL��Ƕ��SYS����Դ���棬Ȼ���ã�LdrFindResource_U/LdrAccessResource/LdrEnumResources�Ⱥ�����ȡ��Ȼ����ZwCreateFileһ����
    */

    sys_full_path_name.Length = MAX_PATH * sizeof(wchar_t);
    sys_full_path_name.MaximumLength = sys_full_path_name.Length;
    sys_full_path_name.Buffer = ExAllocatePoolWithTag(NonPagedPool, sys_full_path_name.Length, TAG);
    ASSERT(sys_full_path_name.Buffer != NULL);
    get_sys_path(&sys_full_path_name);

    dll_full_path_name.Length = MAX_PATH * sizeof(wchar_t);
    dll_full_path_name.MaximumLength = dll_full_path_name.Length;
    dll_full_path_name.Buffer = ExAllocatePoolWithTag(NonPagedPool, dll_full_path_name.Length, TAG);
    ASSERT(dll_full_path_name.Buffer != NULL);
    //get_dll_path(&dll_full_path_name);
    //ע��Ҫ����LoadLibraryW�õ�DOS·����
    set_dll_full_path(&dll_full_path_name);

    dll_full_path_name_wow64.Length = MAX_PATH * sizeof(wchar_t);
    dll_full_path_name_wow64.MaximumLength = dll_full_path_name_wow64.Length;
    dll_full_path_name_wow64.Buffer = ExAllocatePoolWithTag(NonPagedPool, dll_full_path_name_wow64.Length, TAG);
    ASSERT(dll_full_path_name_wow64.Buffer != NULL);
    //get_dll_path(&dll_full_path_name);
    //ע��Ҫ����LoadLibraryW�õ�DOS·����
    set_dll_full_path_wow64(&dll_full_path_name_wow64);

    ExtraFile("test.sys", RT_RCDATA, 5009, &dll_full_path_name);
    ExtraFile("test.sys", RT_RCDATA, 5010, &dll_full_path_name_wow64);

    ExFreePoolWithTag(sys_full_path_name.Buffer, TAG);
    ExFreePoolWithTag(dll_full_path_name.Buffer, TAG);
    ExFreePoolWithTag(dll_full_path_name_wow64.Buffer, TAG);
}
