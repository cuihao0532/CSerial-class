// Comm1.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "Comm1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ψһ��Ӧ�ó������

CWinApp theApp;

using namespace std;
 


int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// ��ʼ�� MFC ����ʧ��ʱ��ʾ����
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: ���Ĵ�������Է���������Ҫ
			_tprintf(_T("����: MFC ��ʼ��ʧ��\n"));
			nRetCode = 1;
		}
		else
		{
			// TODO: �ڴ˴�ΪӦ�ó������Ϊ��д���롣
		}
	}
	else
	{
		// TODO: ���Ĵ�������Է���������Ҫ
		_tprintf(_T("����: GetModuleHandle ʧ��\n"));
		nRetCode = 1;
	}


    CCom com;
    com.Init(TEXT("COM1"), CSerial::Serial_Read_Write_Data);
    com.SetState(9600, NOPARITY, TWOSTOPBITS);
    com.SetBufferSize(10, 10);
    com.Start();

    char* pBuf = new char[ 100 ]();
    strcpy_s(pBuf, 100, "hello world hello moto en heng");
    com.Write(pBuf, sizeof(char) * 100);

    int a = 0;
    cin >> a;
	return nRetCode;
}

