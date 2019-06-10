// Proxy_Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Proxy_Server.h"
#include "afxsock.h"
#include "string"
#include <iostream>
#include <fstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

char *get_ip(const char *host);
bool checkBlackList(string request, char * nameFile);

// The one and only application object

CWinApp theApp;

using namespace std;

wchar_t *convertCharArrayToLPCWSTR(const char* charArray)
{
	wchar_t* wString = new wchar_t[4096];
	MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 4096);
	return wString;
}

// hàm kiểm tra blacklist
bool checkBlackList(string request, ifstream &blacklist){
	// đọc file tên nameFile vào mảng
	// kiểm tra request có trùng trong mảng không
	string s;
	while (!blacklist.eof())
		getline(blacklist, s);
	
	if (s.length()==request.length() ){	//trùng
		for (int i = 0; i < s.length(); i++)
		{
			if (s[i] != request[i])			//ifstream file("blacklist.txt", ios_base::in);	t dùng mở file bằng c++ nên khi m gọi hàm thì làm như vậy nè
			return false;					//checkBlackList(request,file);		
		}									//checkBlackList.close();
		return true;
	}
	return false;							
}


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

			// lấy host của chuỗi request

			string host;
			int indexSearched = request.find("Host: ");
			int indexEndHost;
			if (indexSearched == -1){
				indexEndHost = request.find("\ ");
				host = request.substr(11, indexEndHost - 11);
				cout << "La http 1.0" << endl;
			}
			else {
				cout << "La http 1.1" << endl;
				indexEndHost = request.find("\r\n", indexSearched);
				host = request.substr(indexSearched + 6, indexEndHost - indexSearched - 6);
				cout << host << endl;
			}

			if (checkBlackList(request, "backlist.txt")){
				connectorClient.Send("Trung trong blacklist",100,0);

				Proxy.Close();
			}


			// lấy ra ip của remote server

			string ip = string(get_ip(host.c_str()));
			cout << ip << endl;

			// tạo socket kết nối với remote server

			CSocket connectorRemoteServer;
			if (!connectorRemoteServer.Create()){
				cout << "Not create this socket" << endl;
				return FALSE;
			}
			if (!connectorRemoteServer.Connect(convertCharArrayToLPCWSTR(ip.c_str()), 80)){
				cout << "Not connect this remote server" << endl;
				return FALSE;
			}
			cout << "Da ket noi duoc voi remote server nay" << endl;

			// gửi request giùm và nhận respond của remote server

			int tmpRes = 0;
			tmpRes = connectorRemoteServer.Send(request.c_str(), request.size());
			char bufReceive[10000];
			string response = "";
			if (tmpRes > 0){
				while (tmpRes > 0){
					tmpRes = connectorRemoteServer.Receive(bufReceive, 10000, 0);
					response += string(bufReceive, tmpRes);
				}
			}
			else{
				cout << "Not send request!!!" << endl;
				return 0;
			}

			//Gửi lại respond cho client

			connectorClient.Send(response.c_str(), response.size());

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


char *get_ip(const char *host)
{
	struct hostent *hent;
	int iplen = 15; //XXX.XXX.XXX.XXX
	char *ip = (char *)malloc(iplen + 1);
	memset(ip, 0, iplen + 1);
	if ((hent = gethostbyname(host)) == NULL)
	{
		perror("Can't get IP");
		exit(1);
	}
	if (inet_ntop(AF_INET, (void *)hent->h_addr_list[0], ip, iplen) == NULL)
	{
		perror("Can't resolve host");
		exit(1);
	}
	return ip;
}