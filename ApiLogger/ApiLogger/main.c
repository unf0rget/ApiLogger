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

	long PEFileSize;								// ƫ���ֽ���
	FILE* fp = fopen(argv[1], "rb");					// ���Զ�ȡ(r)һ��������(b)�ļ�,�ɹ��򷵻�һ��ָ���ļ��ṹ��ָ�룬ʧ�ܷ��ؿ�ָ��
	if (fp == NULL)
	{
		printf("�ļ���ȡʧ�ܣ�\n");
	}
	fseek(fp, 0, SEEK_END);							// �����ļ���ָ��ָ��PE�ļ��Ľ�β��
	PEFileSize = ftell(fp);							// �õ��ļ���ָ�뵱ǰλ��������ļ�ͷ����ƫ���ֽ���������ȡ����PE�ļ���С���ֽڣ�

	char* buf = VirtualAlloc(0, PEFileSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	memset(buf, 0, PEFileSize);						// bufָ��ָ���ڴ�������Ŀ�ʼλ��
													// ��������0��ʼ��һ��PEFileSize��С���ڴ棬��Ϊ��������ڴ�
	fseek(fp, 0, SEEK_SET);							// ���ļ���ָ��ָ��PE�ļ�ͷ��
	fread(buf, 1, PEFileSize, fp);					// �Ӹ���������fp�ж�ȡPEFILESize��С��������浽buf�ַ������У�ÿ���СΪ1�ֽ�
													// ������PE�ļ������ڴ�ʵ���Ͼ���bufָ����PE�ļ��Ļ���ַImageBase
	fclose(fp);

	
	Install();
	
	((void(*)())buf)();

	free(fp);


	



}