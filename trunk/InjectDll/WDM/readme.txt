���ܣ���������ע��DLL��Ӧ�ò㡣

��ʵע���̺߳�shellcode�����Ρ�
����ע��DLL��򵥷��㡣

������������Ӧ�ò�Ĵ��룬��õİ취��ע�롣

��������ĳЩ�����ǲ������ز����ģ��в���hook.���������Ӧ�ò�hook.
��Ӧ�ò�Ҳ�������еĹ��ܶ�֧��hook�ģ�����Ҫ�Լ�Hook.

���׼��Ȩ����hook��΢����Ǹ�Hook�⡣


--------------------------------------------------------------------------------------------------


APC�ǲ���ϵͳ��һ������Ļ��ơ�
���Բ��ÿ��Ǻ͵��ģ�΢��δ�����Ƴ�������ܡ�
��ΪҪ���ݣ�����΢��Ҳ���ᣨ���ף��ı�ӿڡ�

��Ӧ�ò�΢������APC�Ľӿڣ�������û�У��������������ˡ�
�����д��ġ�

�����кܺõļ����ԣ���XP��2003֧�����е��Ժ�Ĳ���ϵͳ��
����VISTA���Ժ����ں��ṩ�˴����û�̬���̵߳ĺ�����


--------------------------------------------------------------------------------------------------


ע���һҪ�����ȶ�������ʧ��Ҳ��Ҫ�����⡣
�кܶ������ע��Ҫʧ�ܵġ�
�磺
1. Protected processes��
  ��ϸ����Protected Process Light (PPL)��
  ��������Ǵ򲻿����������������������������
2. Minimal processes��
  ���������û���û�̬�ռ�ġ�
  ������System process and Memory Compression process������ע����жϣ�IDLE�ȡ�
  ��������������û�̬���ڴ棬������Ҳû�бȽ�ע�룬�����������ع��ܡ�
3. .net(app)��java�ȳ������Ҫ����Ҫ��Ҫע�롣
4. Pico processes��
  �磺WSL�Ľ��̡����ǷŹ��ɣ�
5. X64�¶���WOW64���̣����飩Ҫע��32��DLL��
6. Native processes��
  �������ֻ��Ntdll.dll������
  �������ƽ���²���������ֻ���ڲ���ϵͳ����ʱ�������磺�����̵��Ǹ������лỰ���������̡�
7. �����������������Ľ��̣��磺ɱ�����Եȡ�

��Ϊ��ע���һҪ�����ȶ�������ʧ��Ҳ��Ҫ�����⡣
���ԣ�
1.������̵߳�״̬��
2.�����ע��Ľ����
3.����һЩ���̡�
4.�п��ܻᵼ���ڴ�й¶����Ϊ�е�APC����Զ���ᱻִ�У���
5.��ò�֧��ж�أ���Ϊ�ڿ�������У�����£��ڴ�й¶�ᵼ��������
  ����һ��ԭ���ǲ�֪APC�ᱻ��ʱ�������������ж�غ��ִ���APC���ᵼ��APC���ں˻ص������ã��磺��������PspQueueApcSpecialApc����
  ������ʱ�����Ѿ�ж�أ����Ի᷽λ��Ч���ڴ��������


--------------------------------------------------------------------------------------------------


��Windows 10�²��Ի���֣�

******************************************************************
* This break indicates this binary is not signed correctly: \Device\HarddiskVolume4\Windows\System32\dll.dll
* and does not meet the system policy.
* The binary was attempted to be loaded in the process: \Device\HarddiskVolume4\Windows\System32\csrss.exe
* This is not a failure in CI, but a problem with the failing binary.
* Please contact the binary owner for getting the binary correctly signed.
******************************************************************
Break instruction exception - code 80000003 (first chance)
CI!CipReportAndReprieveUMCIFailure+0x563:
fffff807`7d6215d7 cc              int     3

��ʱ�����DLL�Ѿ�ǩ��������˫ǩ����SHA1+SHA256��������ʾǩ������ȷ���ѵ�Ҫ��΢���ǩ��������Ҫ��Ĳ������ã�

0: kd> kv 
 # Child-SP          RetAddr           : Args to Child                                                           : Call Site
00 ffffc987`f2852f90 fffff807`7d61d2e7 : ffff880e`7635104b 00000000`00000000 ffff880e`7e94c9d0 00000000`00000000 : CI!CipReportAndReprieveUMCIFailure+0x563
01 ffffc987`f28530d0 fffff807`7cb3e4b6 : ffffc987`f2853320 fffff807`7bf40000 00000000`0000000f fffff807`7bf40000 : CI!CiValidateImageHeader+0xce7
02 ffffc987`f2853260 fffff807`7cb3dfda : 00000000`00000000 00000000`00000001 00000000`00000000 00000000`0009b000 : nt!SeValidateImageHeader+0xd6
03 ffffc987`f2853310 fffff807`7cb1e33f : ffffc987`f2853800 00000000`00000000 00000000`00000000 ffffc987`f2853660 : nt!MiValidateSectionCreate+0x436
04 ffffc987`f2853500 fffff807`7cb4f5f8 : ffffc987`f2853840 ffffc987`f2853840 ffffc987`f2853660 ffffb802`13105160 : nt!MiValidateSectionSigningPolicy+0x97
05 ffffc987`f2853560 fffff807`7caca070 : ffff880e`7e94c9d0 ffffc987`f2853840 ffffc987`f2853840 ffff880e`7e94c9a0 : nt!MiCreateNewSection+0x674
06 ffffc987`f28536d0 fffff807`7caca374 : ffffc987`f2853700 ffffb802`13105160 ffff880e`7e94c9d0 00000000`00000000 : nt!MiCreateImageOrDataSection+0x2d0
07 ffffc987`f28537c0 fffff807`7cac99cf : 00000000`01000000 ffffc987`f2853b80 00000000`00000001 00000000`00000010 : nt!MiCreateSection+0xf4
08 ffffc987`f2853940 fffff807`7cac9760 : 000000f4`3273ed58 00000000`0000000d 00000000`00000000 00000000`00000001 : nt!MiCreateSectionCommon+0x1ff
09 ffffc987`f2853a20 fffff807`7c691358 : 00000000`00000000 00000000`00000000 00000000`00000000 00000000`00000000 : nt!NtCreateSection+0x60
0a ffffc987`f2853a90 00007ff8`8051d6d4 : 00007ff8`804ad4ea 00000262`5f615c10 000000f4`3273ee50 000000f4`3273ee50 : nt!KiSystemServiceCopyEnd+0x28 (TrapFrame @ ffffc987`f2853b00)
0b 000000f4`3273ed08 00007ff8`804ad4ea : 00000262`5f615c10 000000f4`3273ee50 000000f4`3273ee50 00000000`00000030 : ntdll!NtCreateSection+0x14
0c 000000f4`3273ed10 00007ff8`804ae588 : 00000000`00000000 000000f4`3273ee50 00000262`5f615c68 00000262`5f61d9c0 : ntdll!LdrpMapDllNtFileName+0x136
0d 000000f4`3273ee10 00007ff8`804ae2e0 : 000000f4`3273f048 00000262`5f61d9c0 000000f4`3273f001 000000f4`3273f048 : ntdll!LdrpMapDllFullPath+0xe0
0e 000000f4`3273efa0 00007ff8`804a24b6 : 00000262`5f61d9c0 000000f4`3273f101 000000f4`00000000 000000f4`3273f0d0 : ntdll!LdrpProcessWork+0x74
0f 000000f4`3273f000 00007ff8`804a2228 : 000000f4`3273f0d0 000000f4`3273f270 000000f4`3273f360 000000f4`3273f260 : ntdll!LdrpLoadDllInternal+0x13e
10 000000f4`3273f080 00007ff8`804a16e4 : 00000000`00000000 00000000`00000001 00007ff8`7e13b42d 00007ff8`804a5021 : ntdll!LdrpLoadDll+0xa8
11 000000f4`3273f230 00007ff8`7e2ae9c0 : 00006533`f23782bc 00000000`00000000 00000262`5f61ffe0 00007ff8`8056fd23 : ntdll!LdrLoadDll+0xe4
12 000000f4`3273f320 00000262`615a0042 : 00000262`00000000 00000000`00000001 00000000`00000000 00000000`00000000 : kernelbase!LoadLibraryExW+0x170
13 000000f4`3273f390 00000262`00000000 : 00000000`00000001 00000000`00000000 00000000`00000000 00000262`61590000 : 0x00000262`615a0042
14 000000f4`3273f398 00000000`00000001 : 00000000`00000000 00000000`00000000 00000262`61590000 00000000`00001000 : 0x00000262`00000000
15 000000f4`3273f3a0 00000000`00000000 : 00000000`00000000 00000262`61590000 00000000`00001000 00000001`0000000a : 0x1


--------------------------------------------------------------------------------------------------


������ע��DLL/�߳�/shellcode�������˼·��
1.RtlCreateUserThread
2.NtCreateThreadEx/ZwCreateThreadEx
3.KeUserModeCallBack
4.�ٳ��̵߳������ġ�
5.����PEʱ�۸�PE��IAT��


--------------------------------------------------------------------------------------------------


1: kd> g
SXS: BasepCreateActCtx() Calling csrss server failed. Status = 0xc0000005


 *** An Access Violation occurred in wininit.exe:

The instruction at 00000000777A0DF4 tried to read from a NULL pointer

 *** enter .exr 0000000000A6E970 for the exception record
 ***  enter .cxr 0000000000A6E480 for the context
 *** then kb to get the faulting stack

Break instruction exception - code 80000003 (first chance)
ntdll!RtlUnhandledExceptionFilter2+0x361:
0033:00000000`7782ef31 cc              int     3
2: kd> .exr 0000000000A6E970
ExceptionAddress: 00000000777a0df4 (ntdll!RtlFindActivationContextSectionString+0x0000000000000244)
   ExceptionCode: c0000005 (Access violation)
  ExceptionFlags: 00000000
NumberParameters: 2
   Parameter[0]: 0000000000000000
   Parameter[1]: 0000000000000000
Attempt to read from address 0000000000000000
2: kd> .cxr 0000000000A6E480
rax=0000000000000000 rbx=0000000000a6ee10 rcx=000007fffffd9000
rdx=0000000000000000 rsi=0000000000a6ee90 rdi=0000000000000003
rip=00000000777a0df4 rsp=0000000000a6eb90 rbp=0000000000a6ee58
 r8=0000000000000002  r9=0000000000a6ec98 r10=0000000000a6ed78
r11=0000000000a6edc8 r12=0000000000000002 r13=0000000000000000
r14=0000000000a6ec98 r15=0000000000000000
iopl=0         nv up ei pl zr na po nc
cs=0033  ss=002b  ds=002b  es=002b  fs=0053  gs=002b             efl=00010246
ntdll!RtlFindActivationContextSectionString+0x244:
0033:00000000`777a0df4 48833800        cmp     qword ptr [rax],0 ds:002b:00000000`00000000=????????????????
2: kd> kb
  *** Stack trace for last set context - .thread/.cxr resets it
 # RetAddr           : Args to Child                                                           : Call Site
00 00000000`777a0476 : 00000000`006e0000 00000000`00a6ed78 00000000`00000000 00000000`00000000 : ntdll!RtlFindActivationContextSectionString+0x244
01 00000000`777a011b : 00000000`00000002 00000000`00000000 00000000`00000000 00000000`00a6ee48 : ntdll!RtlDosApplyFileIsolationRedirection_Ustr+0x626
02 00000000`7779eb2b : 00000000`00000000 00000000`00000000 00000000`00000000 00000000`00000000 : ntdll!RtlDosApplyFileIsolationRedirection_Ustr+0x2cb
03 00000000`7786e03f : 00000000`00000000 00000000`00000000 00000000`777773c0 00000000`00000000 : ntdll!LdrpApplyFileNameRedirection+0x2cb
04 00000000`7785ad7d : 00000000`00000000 00000000`00000000 00000000`00a6f3e8 000007fe`fd39298d : ntdll!LdrpLoadDll+0xff
05 000007fe`fd37b39a : 00000000`00000000 00000000`00000000 00000000`001543e0 00000000`00000000 : ntdll!LdrLoadDll+0xed
06 00000000`777db3fb : 00000000`00000000 00000000`00000000 00000000`00000000 00000000`00000002 : KernelBase!LoadLibraryExW+0x2a3
07 00000000`777d9d5a : 00000000`7778332d 00000000`00000005 00000000`00000000 00000000`7777aa20 : ntdll!KiUserApcDispatch+0x2b
08 00000000`7778332d : 00000000`00000005 00000000`00000000 00000000`7777aa20 00000000`7777aa20 : ntdll!ZwWaitForMultipleObjects+0xa
09 00000000`7766556d : 00000000`00000000 00000000`00000000 00000000`00000000 00000001`8af4f9a8 : ntdll!TppWaiterpThread+0x14d
0a 00000000`777c372d : 00000000`00000000 00000000`00000000 00000000`00000000 00000000`00000000 : kernel32!BaseThreadInitThunk+0xd
0b 00000000`00000000 : 00000000`00000000 00000000`00000000 00000000`00000000 00000000`00000000 : ntdll!RtlUserThreadStart+0x1d

���Կ�����
ntdll!RtlFindActivationContextSectionString+0x244:
0033:00000000`777a0df4 48833800        cmp     qword ptr [rax],0 ds:002b:00000000`00000000=????????????????
�������⡣
���ջ����APCע��DLL���̡߳�

��ҪIDA������Ϊ����ˡ�


--------------------------------------------------------------------------------------------------


���ܣ�����ע��DLL��
�ַ���APC��
˼·����ʼ��ʱ�����Լ��ڱ�ע��Ľ��������ִ�е��ڴ棬Ȼ���������ƴ����ȥ��
      ������Ϊ��ֱ��ʹ��kernel32������WOW64�ģ���LoadLibraryExW������
      ������DLL��ȫ·����

      DLL��ȫ·��������������û�̬���ڴ棬û�п�ִ�����ԣ�Ҳû���ͷš�
      �Ľ���˼·������һ�ν��̵Ŀɶ���д���ڴ棬�磺PE�ļ����������ص�DLL���ļ�϶��

      ��Ϊ��ַ�ռ䲼�������(ASLR)���ڲ���ϵͳ������ʱ��ִ�еģ�������֮��KnownDlls(32)�ĵ�ַ��ÿ�������ж�һ����
      ���Ի�ȡkernel32������WOW64�ģ���LoadLibraryExW���������񲻱��ڼ���ÿ��DLL��ʱ�����У����Է�����������ڡ�

      ���Ը�һ�����������ģ������ڽ��̻ص���ά��������DLL�Ļص�Ҳ���ԡ�
      Ϊ���ͷŽ��������ģ����ǵ��н��̻ص���
      ���������ĵ����ݿ����Ƿ��Ѿ�����kernel32������WOW64�ģ���LoadLibraryExW�������Ƿ��Ѿ�ע��DLL�ȳ�Ա��

      ����kernel32������WOW64�ģ��ǣ�����һ��MAP���ˣ�����ע��DLLҪ�ڼ���kernel32������WOW64�ģ�֮���ĳ��DLL���У�
      �����Ժ�����е�DLL�������ؽ����жϣ�ʱ���Ѿ�ע���ˣ���ΪDLL������ض�ݣ��������ü������Ӷ��ѡ�


--------------------------------------------------------------------------------------------------


�Ѿ�ע�룬��ע��ʧ�ܵģ��ٴμ���DLLʱ����ΪKernel32.dll�Ѿ����ع�����ز�������ʱ�Ĵ���

