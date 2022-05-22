/*
	@name:      main.cpp
	@function:	主函数
	@author:    Ray
	@version:   1.0
	@problem:	None
	@timestamp: 2022/05/22
	@notice:	None
*/

#include "Define.h"

using namespace std;

// 从window读取文件大小
int get_fsize_from_windows(const char* fname) {
	FILE* winfp;
	fopen_s(&winfp, fname, "rb");
	if (winfp == NULL)
		throw("-!ERROR: 打开windows文件失败.\n");

	fseek(winfp, 0, SEEK_END);
	unsigned int file_size = ftell(winfp);

	return file_size;
}

// 从window读取文件数据
char* read_from_windows(const char* fname) {
	FILE* winfp;
	fopen_s(&winfp, fname, "rb");
	if (winfp == NULL)
		throw("-!ERROR: 打开windows文件失败.\n");

	fseek(winfp, 0, SEEK_END);
	unsigned int file_size = ftell(winfp);

	fseek(winfp, 0, SEEK_SET);
	char* data = new char[file_size + 1]{ 0 };
	fread(data, file_size, 1, winfp);
	fclose(winfp);
	
	return data;
}

// 当前主机名称转化
string wstring2string(wstring wstr) {
	string result;

	//获取缓冲区大小，并申请空间，缓冲区大小事按字节计算的  
	int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
	char* buffer = new char[len + 1];

	//宽字节编码转换成多字节编码  
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, NULL, NULL);
	buffer[len] = '\0';

	//删除缓冲区并返回值  
	result.append(buffer);
	delete[] buffer;

	return result;
}

// 输入字段分割
vector<string> splitstr(string str, string pattern) {
	vector<string> res;
	char* pTmp = NULL;
	char* temp = strtok_s((char*)str.c_str(), pattern.c_str(), &pTmp);
	while (temp != NULL) {
		res.push_back(string(temp));
		temp = strtok_s(NULL, pattern.c_str(), &pTmp);
	}
	return res;
}

// 主函数
int main() {
	try {
		UserInterface app;
		app.Run();
	}
	catch (...) {
		cout << "-!ERROR: 未知的错误.\n";
	}
	return 0;
}
