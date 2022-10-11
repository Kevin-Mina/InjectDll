#include "Resource.h"
#include "..\libdrv\inc\lib.h"


UNICODE_STRING g_DllDosFullPath;//��ע��DLL��ȫ·��(DOS��ʽ)��
UNICODE_STRING g_DllDosFullPathWow64;


PUNICODE_STRING g_RegistryPath;


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
    UNICODE_STRING SystemRoot = RTL_CONSTANT_STRING(L"\\SystemRoot"); 
    UNICODE_STRING SystemRootNtPath = {0};
    UNICODE_STRING SystemRootDosPath = {0};
    GetSystemRootName(&SystemRoot, &SystemRootNtPath, &SystemRootDosPath);

    UNICODE_STRING DllNtFullPath = {0};    
    UNICODE_STRING Dll = RTL_CONSTANT_STRING(L"\\System32\\hook.dll");
    DllNtFullPath.MaximumLength = SystemRootNtPath.MaximumLength + Dll.MaximumLength;
    NTSTATUS Status = AllocateUnicodeString(&DllNtFullPath);
    ASSERT(NT_SUCCESS(Status));
    RtlCopyUnicodeString(&DllNtFullPath, &SystemRootNtPath);
    ASSERT(NT_SUCCESS(Status));
    Status = RtlAppendUnicodeStringToString(&DllNtFullPath, &Dll);
    ASSERT(NT_SUCCESS(Status));

    g_DllDosFullPath.MaximumLength = SystemRootDosPath.MaximumLength + Dll.MaximumLength;
    Status = AllocateUnicodeString(&g_DllDosFullPath);
    ASSERT(NT_SUCCESS(Status));
    RtlCopyUnicodeString(&g_DllDosFullPath, &SystemRootDosPath);
    ASSERT(NT_SUCCESS(Status));
    Status = RtlAppendUnicodeStringToString(&g_DllDosFullPath, &Dll);
    ASSERT(NT_SUCCESS(Status));

    /*
    ���԰Ѱ�DLL��Ƕ��SYS����Դ���棬Ȼ���ã�LdrFindResource_U/LdrAccessResource/LdrEnumResources�Ⱥ�����ȡ��Ȼ����ZwCreateFileһ����
    */

#ifdef _WIN64
    UNICODE_STRING DllWow64 = RTL_CONSTANT_STRING(L"\\SysWOW64\\hook.dll");

    UNICODE_STRING DllNtFullPathWow64 = {0};
    DllNtFullPathWow64.MaximumLength = SystemRootNtPath.MaximumLength + DllWow64.MaximumLength;
    Status = AllocateUnicodeString(&DllNtFullPathWow64);
    ASSERT(NT_SUCCESS(Status));
    RtlCopyUnicodeString(&DllNtFullPathWow64, &SystemRootNtPath);
    ASSERT(NT_SUCCESS(Status));
    Status = RtlAppendUnicodeStringToString(&DllNtFullPathWow64, &DllWow64);
    ASSERT(NT_SUCCESS(Status));

    g_DllDosFullPathWow64.MaximumLength = SystemRootDosPath.MaximumLength + DllWow64.MaximumLength;
    Status = AllocateUnicodeString(&g_DllDosFullPathWow64);
    ASSERT(NT_SUCCESS(Status));
    RtlCopyUnicodeString(&g_DllDosFullPathWow64, &SystemRootDosPath);
    ASSERT(NT_SUCCESS(Status));
    Status = RtlAppendUnicodeStringToString(&g_DllDosFullPathWow64, &DllWow64);
    ASSERT(NT_SUCCESS(Status));
#endif  
    
#ifdef _WIN64
    ExtraFile("test.sys", RT_RCDATA, 5009, &DllNtFullPath);
    ExtraFile("test.sys", RT_RCDATA, 5010, &DllNtFullPathWow64);
#else
    ExtraFile("test.sys", RT_RCDATA, 5010, &DllNtFullPath);
#endif    

    FreeUnicodeString(&DllNtFullPath);
    FreeUnicodeString(&SystemRootNtPath);
    FreeUnicodeString(&SystemRootDosPath);

#ifdef _WIN64
    FreeUnicodeString(&DllNtFullPathWow64);
#endif  
}
