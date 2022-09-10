/*
	@name:      Inode.h
	@function:	Inode & DiskInode ������
	@author:    Ray
	@version:   1.0
	@problem:	None
	@timestamp: 2022/05/10
	@notice:	Inode ͨ�� DiskInode �Ļָ�/����ʹ�� memcpy ����,
				���ǰ 64 �ֽڵ����ݳ�Ա����һһ��Ӧ;
*/

#pragma once
#ifndef INODE_H
#define INODE_H

#include "Define.h"

using namespace std;

// �ڴ������ڵ�
class Inode {
public:
	Inode();		//���캯��
	~Inode();		//��������

	void Clean();	//���Inode������

	unsigned int	i_mode;			//�ļ�������ʽ
	unsigned int	i_nlink;		//�ļ�������
	short			i_uid;			//����User��id
	short			i_gid;			//����Group��id
	unsigned int	i_size;			//�ļ���С(�ֽ�)
	unsigned int	i_addr[10];		//�ļ��߼���ź�������ת��������
	unsigned int	i_atime;		//������ʱ��
	unsigned int	i_mtime;		//����޸�ʱ��

	int				i_number;		//DiskInode���еı��
};

// ��������ڵ�(64�ֽ�)
class DiskInode {
public:
	DiskInode();	//���캯��
	~DiskInode();	//��������

	unsigned int	d_mode;			//�ļ�������ʽ
	unsigned int	d_nlink;		//�ļ�������
	short			d_uid;			//����User��id
	short			d_gid;			//����Group��id
	unsigned int	d_size;			//�ļ���С(�ֽ�)
	unsigned int	d_addr[10];		//�ļ��߼���ź�������ת��������
	unsigned int	d_atime;		//������ʱ��
	unsigned int	d_mtime;		//����޸�ʱ��
};

#endif // !INODE_H