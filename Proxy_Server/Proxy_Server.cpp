// Proxy_Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Proxy_Server.h"
#include "afxsock.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: change error code to suit your needs
			_tprintf(_T("Fatal Error: MFC initialization failed\n"));
			nRetCode = 1;
		}
		else
		{
			// TODO: code your application's behavior below.

			if(!AfxSocketInit()){
				cout << "Error init Socket Lib";
				return FALSE;
			}
			CSocket Proxy;
			if (!Proxy.Create(8888, SOCK_STREAM, NULL)){
				cout << "Not create a socket";
				cout << Proxy.GetLastError();
				return FALSE;
			}
			else {
				if (!Proxy.Listen(1)){
					cout << "This port is not listen";
					Proxy.Close();
					return FALSE;
				}
			}
			// code trên là khởi tạo thư viện và tạo 1 socket tên proxy có cổng 8888
			cout << "Dang cho ket noi";

			Proxy.Close();
			

			// TODO: code your application's behavior above.
		}
	}
	else
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: GetModuleHandle failed\n"));
		nRetCode = 1;
	}

	return nRetCode;
}
