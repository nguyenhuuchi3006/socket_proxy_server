// Proxy_Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Proxy_Server.h"
#include "afxsock.h"
#include <string>
#include <iostream>
#include <fstream>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool get_ip(const char *host, char *ip, int iplen);
//bool checkBlackList(string request, ifstream &blacklist);
//bool comparerTwoString(string string1, string string2);
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
	bool check = true;
	while (!blacklist.eof())
	{
		getline(blacklist, s);

		if (s.length() == request.length()){	//trùng
			for (int i = 0; i < s.length(); i++)
			{
				if (s[i] != request[i])
					check = false;
			}
			if (check == true)
				return true;
		}
		check = true;

	}
	return false;
}


//so sánh hai chuỗi
bool comparerTwoString(string string1, string string2)//so sánh hai chuỗi
{
	int count = 0;
	int longStr1 = 0;
	int longStr2 = 0;
	while (string1[count] != '\0')
		count++;
	longStr1 = count;
	count = 0;
	while (string2[count] != '\0')
		count++;
	longStr2 = count;
	count = 0;
	if (longStr1 != longStr2)
		return false;
	else
	while (string1[count] != '\0')
	{
		if (string1[count] != string2[count])
			return false;
		count++;
	}
	return true;
}


// mở file để lấy nội dung tương ứng với file request mà clent gửi
string openCacheFile(string request)// mở file để lấy nội dung tương ứng với file request mà clent gửi
{
	string data;
	ifstream infile;
	char string[1000];
	int count = 0;
	while (request[count] != '\0')
	{
		string[count] = request[count];
		count++;
	}
	string[count + 0] = '.';
	string[count + 1] = 't';
	string[count + 2] = 'x';
	string[count + 3] = 't';
	string[count + 4] = '\0';
	infile.open(string);
	for (istreambuf_iterator<char, char_traits<char> > it(infile.rdbuf());
		it != istreambuf_iterator<char, char_traits<char> >(); it++) {

		data += *it; //append at end of string
	}
	return data;
}

void createNewCacheFile(string data, string newRequest)//cái request mà không tồn tại thì thêm vào file request và tạo file tương ứng lưu data phản hồi
{
	
	//lưu data vao request
	ofstream outfile;
	char string[1000];
	int count = 0;
	while (newRequest[count] != '\0')
	{
		string[count] = newRequest[count];
		count++;
	}
	string[count + 0] = '.';
	string[count + 1] = 't';
	string[count + 2] = 'x';
	string[count + 3] = 't';
	string[count + 4] = '\0';

	outfile.open(string);
	outfile << data << endl;
	outfile.close();
	//them du luu vao file luu request
	fstream insertNewRequest("request.txt", ios::app);
	insertNewRequest << newRequest << endl;
	insertNewRequest.close();
}

// kiểm tra request đã tồn tại hay chưa, chưa trả về char 0, đã tồn tại thì đọc tập tin tương ứng 
string cache(string request)
{
	ifstream cacheFile("request.txt");
	if (cacheFile.fail())
	{
		cout << "loi mo file";
		return "0";
	}
	else
	{
		string line;
		string data;
		int i = 0;
		char a[255];
		while (!cacheFile.eof()) {
			{
				char temp[255];
				cacheFile.getline(temp, 255);
				std::string line = temp;
				bool check = comparerTwoString(line, request);
				cout << "String ten file: " << line << endl;
				cout << "String request: " << request << endl;
				cout << "So sanh 2 chuoi: " << check << endl;
				/*if (line == request){
					cout << "YES\n";
					data = openCacheFile(request);
					return data;
				}*/
				if (check == true)
				{
					data = openCacheFile(request);
					return data;
				}
			}
		};
		return "0";
	}
}

string ResForbidden = "HTTP/1.0 403 Forbidden\r\n\Cache-Control: no-cache\r\n\Connection: close\r\n";

DWORD WINAPI handleThread(LPVOID param) {
	SOCKET *socketClient = (SOCKET*)param;
	CSocket connectorClient;
	connectorClient.Attach(*socketClient);

	// nhận request của client

	int size = 100000;
	char *tempRequest = new char[size + 1];
	size = connectorClient.Receive(tempRequest, size, 0);

	if (size <= 0) return -1;

	tempRequest[size + 1] = '\0';
	string request = string(tempRequest, size);
	cout << request << endl;

	// lấy host của chuỗi request

	string host;
	int indexSearched = request.find("Host: ");
	int indexEndHost;
	if (indexSearched == -1) {
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
	// check blacklist
	ifstream file("blacklist.txt", ios_base::in);
	if (checkBlackList(host, file)) {

		connectorClient.Send(ResForbidden.c_str(), ResForbidden.size(), 0);

		cout << "Trung blacklist" << endl;
		return FALSE;
	}
	file.close();
	cout << "Khong trung blacklist" << endl;
	// xong blacklist

	// cache

	string dataReturn = cache(host);
	if (dataReturn != "0") {
		cout << dataReturn << endl;
		cout << "da truy cap trang web nay \n";
		connectorClient.Send(dataReturn.c_str(), dataReturn.size(), 0);
		return FALSE;
	}
	cout << "chua truy cap trang web nay" << endl;
	cout << dataReturn << endl;




	// lấy ra ip của remote server
	int iplen = 15; //XXX.XXX.XXX.XXX
	char *ip = (char *)malloc(iplen + 1);
	memset(ip, 0, iplen + 1);

	if (!get_ip(host.c_str(), ip, iplen)) {
		return -1;
	}
	cout << "IP: " <<string(ip, ip + iplen) << endl;

	// tạo socket kết nối với remote server

	CSocket connectorRemoteServer;
	if (!connectorRemoteServer.Create()) {
		cout << "Not create this socket" << endl;
		return FALSE;
	}
	if (!connectorRemoteServer.Connect(convertCharArrayToLPCWSTR(ip), 80)) {
		cout << "Not connect this remote server" << endl;
		return FALSE;
	}
	cout << "Da ket noi duoc voi remote server nay" << endl;

	// gửi request giùm và nhận respond của remote server

	int tmpRes = 0;
	tmpRes = connectorRemoteServer.Send(request.c_str(), request.size());
	char bufReceive[10000];
	string response = "";
	if (tmpRes > 0) {
		while (tmpRes > 0) {
			tmpRes = connectorRemoteServer.Receive(bufReceive, 10000, 0);
			response += string(bufReceive, tmpRes);
		}
	}
	else {
		cout << "Not send request!!!" << endl;
		return 0;
	}

	//Gửi lại respond cho client
	// Trước khi gửi thì lưu nó vào cache
	createNewCacheFile(response, host);

	connectorClient.Send(response.c_str(), response.size());
	return 0;
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
			HANDLE threadStatus;
			DWORD threadID;

			while (true) {
				if (!Proxy.Accept(connectorClient)) {
					cout << "Not accept this client" << endl;
					return FALSE;
				}
				cout << "da ket noi" << endl;

				SOCKET *pSocketClient = new SOCKET();
				*pSocketClient = connectorClient.Detach();
				threadStatus = CreateThread(NULL, 0, handleThread, pSocketClient, 0, &threadID);
				cout << "New thread created with id: " << threadID << endl;
			}
			
<<<<<<< HEAD
			string dataReturn = cache(host);
			if (dataReturn != "0"){
				cout << dataReturn << endl;
				cout << "da truy cap trang web nay \n";
				connectorClient.Send(dataReturn.c_str(), dataReturn.size(), 0);
				Proxy.Close();
				return FALSE;
			}
			cout << "chua truy cap trang web nay" << endl;
			cout << dataReturn << endl;

			
			

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
			// Trước khi gửi thì lưu nó vào cache
			createNewCacheFile(response, host);
			//cout << "Respond ne: " << response << endl;

			connectorClient.Send(response.c_str(), response.size());

=======
>>>>>>> 0770cb0411a172aca680e147a166a0d7fe469255
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


bool get_ip(const char *host, char* ip, int iplen)
{
	struct hostent *hent;
	if ((hent = gethostbyname(host)) == NULL)
	{
		perror("Can't get IP");
		return false;
	}
	if (inet_ntop(AF_INET, (void *)hent->h_addr_list[0], ip, iplen) == NULL)
	{
		perror("Can't resolve host");
		return false;
	}

	return true;
}
