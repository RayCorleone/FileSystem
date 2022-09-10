/*
	@name:      FileSystem.h
	@function:	FileSystem ������
	@author:    Ray
	@version:   1.0
	@problem:	None
	@timestamp: 2022/05/10
	@notice:	fformat��SuperBlockд���ļ�����Ҫ�ض�, ����Spb��ƥ��;
				fformat�й������ļ�ϵͳ�Ļ����ṹ�����������ļ��ĸ���;
				��Ŀ¼��Ȩ������Ϊ0777; �ļ���Ĭ��0764; �ļ�Ĭ��0664.
*/

#pragma once
#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "Define.h"

using namespace std;

// �ļ�ϵͳ
class FileSystem{
private:
	SuperBlock spb;			//�洢��Դ�������
	DiskDevice disk;		//�����豸����
	BufferManager buffer;	//����������

	void Initialize();		//��ʼ���ļ�ϵͳ

	//InodeManager
	Inode ILoad(int i_no);		//�����ض�Inode
	void  ISave(Inode inode);	//�����ض�Inode
	Inode IAlloc();				//����һ��Inode
	void  IFree(Inode inode);	//�ͷ�һ��Inode

	//BlockManager
	int  BAlloc();			//����һ���̿�
	void BFree(int no);		//�ͷ�һ���̿�

	//FileManager
	void  FDelete(Inode& cur_inode);		//����Inodeɾ���ļ�
	void  FDeleteall(Inode& cur_inode);		//����Inodeɾ������
	Inode FFind(Inode& dir_inode, const char* name);	//�����ļ�����λInode(��Ŀ¼����)
	void  FRead(Inode& cur_inode, char* buf, unsigned int offset, unsigned int len);		//����Inode��ȡ���ݵ�buf
	void  FWrite(Inode& cur_inode, const char* buf, unsigned int offset, unsigned int len);	//����Inodeдbuf���ݵ�����

public:
	FileSystem();	//���캯��
	~FileSystem();	//��������

	SuperBlock* GetSpb();	//���SuperBlock
	void SaveSpb();			//����SuperBlock
	void LoadSpb();			//����SuperBlock
	void fformat();			//��ʽ���ļ�ϵͳ

	int dcreate(const char* dir, short uid = 0, short gid = 0);	//������Ŀ¼
	int ddelete(const char* dir, short uid = 0, short gid = 0);	//ɾ��Ŀ¼
	bool enter(const char* dir, short uid = 0, short gid = 0);	//����Ŀ¼
	int fcreate(const char* dir, short uid = 0, short gid = 0);	//�������ļ�
	int fdelete(const char* dir, short uid = 0, short gid = 0);	//ɾ���ļ�
	File* fopen(const char* dir, short uid = 0, short gid = 0);	//���ļ�
	int fclose(File* fp);	//�ر��ļ�
	int fcseek(File* fp);	//����ļ���дָ��λ��
	int fseek(File* fp, int offset, int mode);	//�޸��ļ���дָ��
	int fread(char* buf, int size, File* fp);			//���ļ���buf
	int fwrite(const char* buf, int size, File* fp);	//дbuf���ļ�
	int freplace(const char* buf, int size, File* fp);	//дbuf���ļ�(�滻)

	int chmod(const char* dir, int mode, short uid = 0, short gid = 0);	//�޸�Ȩ��
	vector<string> flist(const char* dir, short uid = 0, short gid = 0);//�г��ļ���Ϣ
};

#endif // !FILESYSTEM_H
