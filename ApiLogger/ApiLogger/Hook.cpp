// ͬʱ����PE�ļ��ĵ����������ĺ�����
#include<Windows.h>
#include<stdio.h>
#include<map>
#include<vector>
#include<string>
std::map<UINT_PTR, std::string>		  NameList;
std::map<UINT_PTR, std::vector<BYTE>> RestoreList;

extern"C"
{
#include"c.h"
}

std::vector<std::string> HookApi = {"LoadLibraryA","LoadLibraryW","GetProcAddress","connect"};

BYTE ShellCode[12] = { 0x48,0xB8,0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11,0xFF,0xD0 };

extern"C" void InstallHook(UINT_PTR Addr)
{
	BYTE* pData = (BYTE*)Addr;
	std::vector<BYTE> RestoreData;
	for (int i = 0; i < sizeof(ShellCode); i++)
	{
		RestoreData.push_back(pData[i]);
	}

	RestoreList[Addr] = RestoreData;
	
	memcpy((void*)Addr, ShellCode, sizeof(ShellCode));

	return;
}

void RestoreHook(UINT_PTR Addr)
{
	BYTE RestoreData[sizeof(ShellCode)] = {};
	for (int i = 0; i < RestoreList[Addr].size(); i++)
	{
		RestoreData[i] = RestoreList[Addr][i];
	}
	memcpy((void*)Addr, RestoreData, sizeof(ShellCode));
}

void FindExportTable(const char* DllName)
{
	HMODULE hMod = LoadLibraryA(DllName);
	char* pBuf = (char*)hMod;

	//��λDOSͷ;
	PIMAGE_DOS_HEADER DosHeader = (PIMAGE_DOS_HEADER)pBuf;
	//��λNTͷ��
	PIMAGE_NT_HEADERS Nt_Header = (PIMAGE_NT_HEADERS)(pBuf + DosHeader->e_lfanew);

	DWORD tmp = 0;
	VirtualProtect(pBuf, Nt_Header->OptionalHeader.SizeOfImage, PAGE_EXECUTE_READWRITE, &tmp);

	//��λ����Ŀ¼��
	PIMAGE_OPTIONAL_HEADER OptionHeader = &(Nt_Header->OptionalHeader);
	//��λ���������ݽṹ
	PIMAGE_EXPORT_DIRECTORY ExportTable = (PIMAGE_EXPORT_DIRECTORY)(OptionHeader->DataDirectory[0].VirtualAddress + pBuf);
	//��ȡĿ��DLL������
	//��ȡ������������ʼ���Base
	WORD Base = ExportTable->Base;
	//��ȡ��������������
	DWORD NumOfFunc = ExportTable->NumberOfFunctions;
	//��ȡ�����ֵ����ĺ���������
	DWORD NumOfName = ExportTable->NumberOfNames;
	//��ȡ����������ַ���RVA�����ҽ���ת����ΪRAW
	PDWORD AddressOfFun = (PDWORD)(ExportTable->AddressOfFunctions + pBuf);
	//��ȡ�������Ƶ�ַ��
	PDWORD AddressOfName = (PDWORD)(ExportTable->AddressOfNames + pBuf);
	//��ȡ������ŵ�ַ��
	PWORD AddressOfNameOrdinals = (PWORD)(ExportTable->AddressOfNameOrdinals + pBuf);
	//��������ʼ����������
	for (int i = 0; i < NumOfFunc; i++)
	{
		if (AddressOfFun[i] == 0)
			continue;
		int j = 0;
		UINT_PTR FuncAddress = AddressOfFun[i] + (UINT_PTR)pBuf;
		for (; j < NumOfName; j++)
		{
			if (AddressOfNameOrdinals[j] == i)
			{
				char* FuncName = (char*)((AddressOfName[j]) + pBuf);

				for (auto str : HookApi)
				{
					if (str == FuncName)
					{

						NameList[FuncAddress] = FuncName;
						InstallHook(FuncAddress);
					}
				}


				break;
			}
		}
		if (j == NumOfName)
		{
			//char TmpStr[64] = {};
			//_itoa_s(Base + i, TmpStr, sizeof(TmpStr), 10);
			//std::string OrdinalName = "Ordinal";
			//OrdinalName += TmpStr;
			//NameList[FuncAddress] = OrdinalName;
			//InstallHook(FuncAddress);
		}
	}
}

extern"C" void  Install()
{
	PUINT_PTR pWrite = (PUINT_PTR)(ShellCode + 2);
	*pWrite = (UINT_PTR)CallBack;

	std::vector<std::string> DllVector;
	DllVector.push_back("ntdll.dll");
	DllVector.push_back("ws2_32.dll");
	DllVector.push_back("kernel32.dll");

	for (auto dll : DllVector)
	{
		FindExportTable(dll.c_str());
	}


}

extern"C" void  PrintInfo(UINT_PTR Address)
{

	RestoreHook(Address);

	printf("CALL %s\n", NameList[Address].c_str());
	
	


}