/*
	@name:      main.cpp
	@function:	������
	@author:    Ray
	@version:   1.0
	@problem:	None
	@timestamp: 2022/05/22
	@notice:	None
*/

#include "Define.h"

using namespace std;

// ��window��ȡ�ļ���С
int get_fsize_from_windows(const char* fname) {
	FILE* winfp;
	fopen_s(&winfp, fname, "rb");
	if (winfp == NULL)
		throw("-!ERROR: ��windows�ļ�ʧ��.\n");

	fseek(winfp, 0, SEEK_END);
	unsigned int file_size = ftell(winfp);

	return file_size;
}

// ��window��ȡ�ļ�����
char* read_from_windows(const char* fname) {
	FILE* winfp;
	fopen_s(&winfp, fname, "rb");
	if (winfp == NULL)
		throw("-!ERROR: ��windows�ļ�ʧ��.\n");

	fseek(winfp, 0, SEEK_END);
	unsigned int file_size = ftell(winfp);

	fseek(winfp, 0, SEEK_SET);
	char* data = new char[file_size + 1]{ 0 };
	fread(data, file_size, 1, winfp);
	fclose(winfp);
	
	return data;
}

// ��ǰ��������ת��
string wstring2string(wstring wstr) {
	string result;

	//��ȡ��������С��������ռ䣬��������С�°��ֽڼ����  
	int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
	char* buffer = new char[len + 1];

	//���ֽڱ���ת���ɶ��ֽڱ���  
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, NULL, NULL);
	buffer[len] = '\0';

	//ɾ��������������ֵ  
	result.append(buffer);
	delete[] buffer;

	return result;
}

// �����ֶηָ�
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

// ������
int main() {
	try {
		UserInterface app;
		app.Run();
	}
	catch (...) {
		cout << "-!ERROR: δ֪�Ĵ���.\n";
	}
	return 0;
}
