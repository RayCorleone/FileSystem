/*
	@name:      SuperBlock.h
	@function:	SuperBlock ������
	@author:    Ray
	@version:   1.0
	@problem:	None
	@timestamp: 2022/05/10
	@notice:	padding[50] ������п���, ֮��� SuperBlock �ֿռ�ʱʹ��
				���Ǿ��Դ�С 2 * BLOCK_SIZE, ���� sizeof(SuperBlock());
*/

#pragma once
#ifndef SUPERBLOCK_H
#define SUPERBLOCK_H

#include "Define.h"

using namespace std;

// �洢��Դ�����
class SuperBlock {
public:
	SuperBlock();	//���캯��
	~SuperBlock();	//��������

	unsigned int s_inum;		//Inode�ܸ���
	unsigned int s_ifnum;		//����Inode�ܸ���
	unsigned int s_ninode;		//ֱ�ӹ���Ŀ������Inode��
	unsigned int s_inode[100];	//ֱ�ӹ���Ŀ������Inode������

	unsigned int s_bnum;		//�����̿�����
	unsigned int s_bfnum;		//�����̿�����
	unsigned int s_nfree;		//ֱ�ӹ���Ŀ����̿���
	unsigned int s_free[100];	//ֱ�ӹ���Ŀ����̿�������

	unsigned int padding[50];	//���SuperBlock��1024�ֽ�
};

#endif // !SUPERBLOCK_H