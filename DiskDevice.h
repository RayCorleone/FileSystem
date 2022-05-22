/*
	@name:      DiskDevice.h
	@function:	DiskDevice 类声明
	@author:    Ray
	@version:   1.0
	@problem:	None
	@timestamp: 2022/05/10
	@notice:	DiskDevice 是文件系统唯一需要读写的设备
*/

#pragma once
#ifndef DISK_DEVICE_H
#define DISK_DEVICE_H

#include "Define.h"

using namespace std;

// 磁盘设备
class DiskDevice {
private:
	BufferManager* bm;			//缓存管理

public:
	char disk_name[NAME_SIZE];	//磁盘名
	Buf bDiskList;	//设备、I/O队列控制块

	DiskDevice();	//构造函数
	~DiskDevice();	//析构函数

	void SetDisk(const char* name, BufferManager* bm);							//设置磁盘参数
	void Dread(char* buf, unsigned int start_addr, unsigned int length);		//通过缓存读取磁盘
	void Dwrite(const char* buf, unsigned int start_addr, unsigned int length);	//通过缓存写入磁盘(设置延迟写)
};

#endif // !DISK_DEVICE_H
