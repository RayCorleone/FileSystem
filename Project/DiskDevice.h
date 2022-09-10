/*
	@name:      DiskDevice.h
	@function:	DiskDevice ������
	@author:    Ray
	@version:   1.0
	@problem:	None
	@timestamp: 2022/05/10
	@notice:	DiskDevice ���ļ�ϵͳΨһ��Ҫ��д���豸
*/

#pragma once
#ifndef DISK_DEVICE_H
#define DISK_DEVICE_H

#include "Define.h"

using namespace std;

// �����豸
class DiskDevice {
private:
	BufferManager* bm;			//�������

public:
	char disk_name[NAME_SIZE];	//������
	Buf bDiskList;	//�豸��I/O���п��ƿ�

	DiskDevice();	//���캯��
	~DiskDevice();	//��������

	void SetDisk(const char* name, BufferManager* bm);							//���ô��̲���
	void Dread(char* buf, unsigned int start_addr, unsigned int length);		//ͨ�������ȡ����
	void Dwrite(const char* buf, unsigned int start_addr, unsigned int length);	//ͨ������д�����(�����ӳ�д)
};

#endif // !DISK_DEVICE_H
