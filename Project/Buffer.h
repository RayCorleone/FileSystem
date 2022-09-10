/*
	@name:      Buffer.h
	@function:	Buf & BufferManager ������
	@author:    Ray
	@version:   1.0
	@problem:	None
	@timestamp: 2022/05/10
	@notice:	ֻ��һ����д�豸DiskDevice, ����ѡ����BufferManager�����
				����Ψһָ�룬ͬʱ���ú������ĳ�ʼ������
*/

#pragma once
#ifndef BUFFER_H
#define BUFFER_H

#include "Define.h"

using namespace std;

// �����
class Buf {
public:
	// ������ƿ���й���ָ��
	Buf* b_forw;	//�豸����(DiskDeviced��bDiskList)
	Buf* b_back;	//�豸����(DiskDeviced��bDiskList)
	Buf* av_forw;	//���ɶ��С�I/O�������(DiskDeviced��bDiskList)
	Buf* av_back;	//���ɶ��С�I/O�������(DiskDeviced��bDiskList)

	char* b_addr;	//ָ��û�����ƿ����Ļ������׵�ַ
	unsigned int	b_blkno;	//��д�Ĵ����߼����
	unsigned int	b_flags;	//������ƿ��־λ
};

// �������
class BufferManager {
private:
	Buf bFreeList;					//���ɶ��п��ƿ�
	DiskDevice* disk;				//�����豸
	Buf m_Buf[NBUF];				//������ƿ�����
	char Buffer[NBUF][BUFFER_SIZE];	//����������

public:
	BufferManager();	//���캯��
	~BufferManager();	//��������

	void Initialize(DiskDevice* disk);	//������ƿ���еĳ�ʼ��, ��������ƿ���b_addrָ����Ӧ�������׵�ַ, ͬʱ��������豸
	Buf* GetBlk(unsigned int blkno);	//����һ�黺���, ����Disk��дblkno�ſ�
	void Brelse(Buf* bp);				//�ͷŻ�����ƿ�
	Buf* Bread(unsigned int blkno);		//��һ���̿�, blknoΪĿ����̿��߼����
	void Bwrite(Buf* bp);				//дһ���̿�, blknoΪĿ����̿��߼����
	void Bdwirte(Buf* bp);				//�����ӳ�д��־
};

#endif // !BUFFER_H
