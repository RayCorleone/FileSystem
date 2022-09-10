/*
	@name:      Buffer.h
	@function:	Buf & BufferManager 类声明
	@author:    Ray
	@version:   1.0
	@problem:	None
	@timestamp: 2022/05/10
	@notice:	只有一个读写设备DiskDevice, 所以选择在BufferManager里添加
				它的唯一指针，同时设置含参数的初始化函数
*/

#pragma once
#ifndef BUFFER_H
#define BUFFER_H

#include "Define.h"

using namespace std;

// 缓存块
class Buf {
public:
	// 缓存控制块队列勾连指针
	Buf* b_forw;	//设备队列(DiskDeviced的bDiskList)
	Buf* b_back;	//设备队列(DiskDeviced的bDiskList)
	Buf* av_forw;	//自由队列、I/O请求队列(DiskDeviced的bDiskList)
	Buf* av_back;	//自由队列、I/O请求队列(DiskDeviced的bDiskList)

	char* b_addr;	//指向该缓存控制块管理的缓冲区首地址
	unsigned int	b_blkno;	//读写的磁盘逻辑块号
	unsigned int	b_flags;	//缓存控制块标志位
};

// 缓存管理
class BufferManager {
private:
	Buf bFreeList;					//自由队列控制块
	DiskDevice* disk;				//磁盘设备
	Buf m_Buf[NBUF];				//缓存控制块数组
	char Buffer[NBUF][BUFFER_SIZE];	//缓存区数组

public:
	BufferManager();	//构造函数
	~BufferManager();	//析构函数

	void Initialize(DiskDevice* disk);	//缓存控制块队列的初始化, 将缓存控制块中b_addr指向相应缓冲区首地址, 同时载入磁盘设备
	Buf* GetBlk(unsigned int blkno);	//申请一块缓存块, 用于Disk读写blkno号块
	void Brelse(Buf* bp);				//释放缓存控制块
	Buf* Bread(unsigned int blkno);		//读一个盘块, blkno为目标磁盘块逻辑块号
	void Bwrite(Buf* bp);				//写一个盘块, blkno为目标磁盘块逻辑块号
	void Bdwirte(Buf* bp);				//设置延迟写标志
};

#endif // !BUFFER_H
