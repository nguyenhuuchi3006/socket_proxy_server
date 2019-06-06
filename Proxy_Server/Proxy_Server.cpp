// Proxy_Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Proxy_Server.h"
#include "afxsock.h"
#include "string"
#include <iostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;
//
//string getIpByHostName(const char* host){//	struct hostent *hent;//	int iplen = 15; //XXX.XXX.XXX.XXX//	char *ip = new char(iplen + 1);//	memset(ip, 0, iplen + 1);//	if ((hent = gethostbyname(host)) == NULL){//		return "NULL";//	}//	if (inet_ntop(AF_INET, (void *)hent->h_addr_list[0], ip, iplen) == NULL){//		return "NULL";//	}//	return string(ip);//}
//
//wchar_t *convertCharArrayToLPCWSTR(const char* charArray)
//{
//	wchar_t* wString = new wchar_t[4096];
//	MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 4096);
//	return wString;
//}

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

			// khởi tạo thư viện và tạo 1 socket tên proxy có cổng 8888
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
			
			// tạo socket kết nối proxy với client
			cout << "Dang cho ket noi" << endl;

			CSocket connectorClient;
			if (!Proxy.Accept(connectorClient)){
				cout << "Not accept this client" << endl;
				return FALSE;
			}
			cout << "da ket noi" << endl;

			// nhận request của client
			int size = 100000;
			char *tempRequest = new char[size + 1];
			size = connectorClient.Receive(tempRequest, size, 0);
			tempRequest[size + 1] = '\0';
			string request = string(tempRequest,size);
			cout << request << endl;

			// lấy host của chuỗi request, http 1.1 luôn có host, còn http 1.0 thì cái có cái không, nên tách ra lấy ở dòng đầu

			// từ cái host đã có lấy ra ip của nó (có hàm ở trên rồi)
			string ip = "";

			// tạo socket kết nối với remote server
			/*CSocket connectorRemoteServer;
			if (!connectorRemoteServer.Create()){
				cout << "Not create this socket" << endl;
				return FALSE;
			}
			if (!connectorRemoteServer.Connect(convertCharArrayToLPCWSTR(ip.c_str()), 80)){
				cout << "Not connect this remote server" << endl;
				return FALSE;
			}
			cout << "Da ket noi duoc voi remote server nay" << endl;
*/
			// nhận respond của remote server


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
