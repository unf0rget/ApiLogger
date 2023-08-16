#include"c.h"
#include"Hook.h"
#include<stdlib.h>
#include<stdio.h>

//extern"C"
//{
//
//}


extern UINT_PTR GetReturnAddress();
extern UINT_PTR PushRCX();
extern UINT_PTR PopRCX();
// void  CallBack()
//{
//	UINT_PTR RetAddr;
//	RetAddr= GetReturnAddress();
//	RetAddr -= 12;
//	PushRCX();
//	PrintInfo(RetAddr);
//	PopRCX();
//	UINT_PTR Ret;
//	Ret= ((UINT_PTR(*)())RetAddr)();
//	InstallHook(RetAddr);
//	return Ret;
//}

int main(int argc,char* argv[])
{


	if (argc < 2)
		return 0;

	long PEFileSize;								// 偏移字节数
	FILE* fp = fopen(argv[1], "rb");					// 尝试读取(r)一个二进制(b)文件,成功则返回一个指向文件结构的指针，失败返回空指针
	if (fp == NULL)
	{
		printf("文件读取失败！\n");
	}
	fseek(fp, 0, SEEK_END);							// 设置文件流指针指向PE文件的结尾处
	PEFileSize = ftell(fp);							// 得到文件流指针当前位置相对于文件头部的偏移字节数，即获取到了PE文件大小（字节）

	char* buf = VirtualAlloc(0, PEFileSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	memset(buf, 0, PEFileSize);						// buf指针指向内存中数组的开始位置
													// 在这里用0初始化一块PEFileSize大小的内存，即为数组分配内存
	fseek(fp, 0, SEEK_SET);							// 将文件流指针指向PE文件头部
	fread(buf, 1, PEFileSize, fp);					// 从给定输入流fp中读取PEFILESize大小个数据项保存到buf字符数组中，每项大小为1字节
													// 这样将PE文件读入内存实际上就让buf指向了PE文件的基地址ImageBase
	fclose(fp);

	
	Install();
	
	((void(*)())buf)();

	free(fp);


	



}