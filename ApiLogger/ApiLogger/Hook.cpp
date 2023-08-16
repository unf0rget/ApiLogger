// 同时遍历PE文件的导入表、导出表的函数名
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

	//定位DOS头;
	PIMAGE_DOS_HEADER DosHeader = (PIMAGE_DOS_HEADER)pBuf;
	//定位NT头；
	PIMAGE_NT_HEADERS Nt_Header = (PIMAGE_NT_HEADERS)(pBuf + DosHeader->e_lfanew);

	DWORD tmp = 0;
	VirtualProtect(pBuf, Nt_Header->OptionalHeader.SizeOfImage, PAGE_EXECUTE_READWRITE, &tmp);

	//定位数据目录表
	PIMAGE_OPTIONAL_HEADER OptionHeader = &(Nt_Header->OptionalHeader);
	//定位导出表数据结构
	PIMAGE_EXPORT_DIRECTORY ExportTable = (PIMAGE_EXPORT_DIRECTORY)(OptionHeader->DataDirectory[0].VirtualAddress + pBuf);
	//获取目标DLL的名称
	//获取导出函数的起始序号Base
	WORD Base = ExportTable->Base;
	//获取导出函数的总数
	DWORD NumOfFunc = ExportTable->NumberOfFunctions;
	//获取以名字导出的函数的总数
	DWORD NumOfName = ExportTable->NumberOfNames;
	//获取导出函数地址表的RVA，并且将至转换成为RAW
	PDWORD AddressOfFun = (PDWORD)(ExportTable->AddressOfFunctions + pBuf);
	//获取函数名称地址表
	PDWORD AddressOfName = (PDWORD)(ExportTable->AddressOfNames + pBuf);
	//获取函数序号地址表
	PWORD AddressOfNameOrdinals = (PWORD)(ExportTable->AddressOfNameOrdinals + pBuf);
	//接下来开始遍历导出表
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