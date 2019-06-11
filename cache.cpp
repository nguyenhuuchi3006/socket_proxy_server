#include "cache.h"
int main()
{
	// cái mô phỏng cách dùng

	string data;
	data = openCacheFile("home");
	cout << data;
	string request = "www.adkdsdd";
	string dataReturn = cache(request); //lấy dữ liệu trong file
	if (dataReturn == "0")
		//tạo mới vì request chưa tồn tại
		createNewCacheFile(data, request);
	string data2 = openCacheFile(request);
	return 0;
}
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
string cache(string request)// kiểm tra request đã tồn tại hay chưa, chưa trả về char 0, đã tồn tại thì đọc tập tin tương ứng 
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