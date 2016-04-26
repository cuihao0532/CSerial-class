// Comm1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Comm1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 唯一的应用程序对象

CWinApp theApp;

using namespace std;
 


int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// 初始化 MFC 并在失败时显示错误
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: 更改错误代码以符合您的需要
			_tprintf(_T("错误: MFC 初始化失败\n"));
			nRetCode = 1;
		}
		else
		{
			// TODO: 在此处为应用程序的行为编写代码。
		}
	}
	else
	{
		// TODO: 更改错误代码以符合您的需要
		_tprintf(_T("错误: GetModuleHandle 失败\n"));
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

