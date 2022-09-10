/*
	@name:      UserInterface.h
	@function:	UserInterface ������
	@author:    Ray
	@version:   1.0
	@problem:	None
	@timestamp: 2022/05/20
	@notice:	�˽�����0��ͷ, chmod��Ҫ����������ǰ˽���;
*/

#pragma once
#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include "Define.h"

using namespace std;

// �û�����
class UserInterface {
private:
	FileSystem fs;		//�ļ�ϵͳ
	string local_host;	//������

	int cur_uid;		//�û�id
	int cur_gid;		//�û���id
	bool is_login;		//�Ƿ��¼
	string cur_user;	//�û���
	string cur_group;	//�û�����
	string cur_dir;		//��ǰ·��

	int cur_file_cnt;				//��ǰ�ļ���
	File* cur_flist[FLIST_SIZE];	//��ǰ���ļ���
	int cur_file_mode[FLIST_SIZE];	//��ǰ���ļ�Ȩ�ޱ�

protected:
	// ������ָ��(��ȥ����ָ�����exit/cd/cls/show)
	void _ftest();									//���ɲ���ָ��
	void _fhelp();									//��ʾ����ָ�����
	void _fformat();								//��ʽ���ļ�ϵͳ
	void _flist();									//�г��ļ�Ŀ¼
	int _fsize(const int fd);						//�ļ���С(�����ļ���С)
	void _dcreat(const char* name, const int mode);	//������Ŀ¼
	void _ddelete(const char* name);				//ɾ��Ŀ¼
	int _fcreat(const char* name, const int mode);	//�������ļ�(�����ļ���)
	void _fdelete(const char* name);				//ɾ���ļ�(�޷���)
	int _fopen(const char* name, const int mode);	//���ļ�(�����ļ���)
	void _fclose(const int fd);						//�ر��ļ�(�޷���)
	int _fread(const int fd, char* buf, int size);	//��ȡ�ļ�(���ض���������)
	int _fwrite(const int fd, const char* buf, int size);//д���ļ�(����д��������)
	int _flseek(const int fd, const int offset, const int ptrname);	//��λ�ļ���дָ��(����λ��)
	int _fcseek(const int fd);						//�ļ�ָ�뵱ǰλ��(���ص�ǰλ��)

	// ������ָ��
	void help();							//��ʾ����
	void cls();								//���������
	void disk();							//��ʾ����ʹ�����
	void format();							//��ʽ���ļ�ϵͳ
	void ls();								//��ʾ��ǰ�ļ����б�
	void path();							//��ʾ��ǰ·��
	void cd(const char* name);				//�����ļ���
	void mkdir(const char* name);			//�½��ļ���
	void rmdir(const char* name);			//ɾ���ļ���
	void touch(const char* name);			//�½��ļ�
	void remove(const char* name);			//ɾ���ļ�
	void chmod(const char* name, int mode);	//�޸��ļ�(��)Ȩ��
	bool login(string name, string passwd);	//��¼
	void logout();							//�ǳ�
	void addu(string uname, string gname, string passwd);	//����û�
	void delu(string uname);				//ɾ���û�
	void addg(string uname);				//����û���
	void delg(string uname);				//ɾ���û���
	void show(const char* name);			//��ʾ�ļ�
	void cp(const char* ori_fname, const char* tar_fname);		//fs���ļ�����
	void win2fs(const char* win_fname, const char* fs_fname);	//��win�����ļ���fs
	void fs2win(const char* fs_fname, const char* win_fname);	//��fs�����ļ���win

public:
	UserInterface();	//���캯��
	~UserInterface();	//��������

	void TestRun();		//����ָ����к���
	void BetaRun();		//����ָ����к���
	void Run();			//�����к���
};

#endif // !USER_INTERFACE_H
