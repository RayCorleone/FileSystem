/*
	@name:      File.h
	@function:	File & DirectoryEntry ������
	@author:    Ray
	@version:   1.0
	@problem:	None
	@timestamp: 2022/05/10
	@notice:	File �� f_inode.i_uid(i_gid)�Ǵ�����(��)�� id, ������
				����(��)�� f_uid(f_gid)����������Ȩ��;
*/

#pragma once
#ifndef FILE_H
#define FILE_H

#include "Define.h"

using namespace std;

// �ļ�
class File {
public:
	File();		//���캯��
	~File();	//��������

	short			f_uid;		//���ļ����û�id(ģ��OpenFile)
	short			f_gid;		//���ļ����û���id(ģ��OpenFile)
	Inode*			f_inode;	//ָ���ļ���Ӧ��Inodeָ��
	unsigned int	f_offset;	//�ļ���дָ���λ��
};

// Ŀ¼���
class DirectoryEntry {
public:
	DirectoryEntry();	//���캯��
	~DirectoryEntry();	//��������

	int		m_ino;				//Ŀ¼����Inode���
	char	m_name[NAME_SIZE];	//Ŀ¼����·������
};

#endif // !FILE_H
