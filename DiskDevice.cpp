/*
	@name:      DiskDevice.cpp
	@function:	DiskDevice 类定义
	@author:    Ray
	@version:   1.0
	@problem:	None
	@timestamp: 2022/05/10
	@notice:	None
*/

#include "Define.h"

using namespace std;

// DiskDevice构造函数
DiskDevice::DiskDevice() {
	this->disk_name[0] = '\0';
	this->bm = NULL;

	bDiskList.b_blkno = -1;
	bDiskList.b_addr = NULL;
	bDiskList.b_flags = BufFlag::B_NONE;
	bDiskList.b_forw = &bDiskList;
	bDiskList.b_back = &bDiskList;
	bDiskList.av_forw = &bDiskList;
	bDiskList.av_back = &bDiskList;
}

// DiskDevice析构函数
DiskDevice::~DiskDevice() {
	//Nothing to do
}

// 设置磁盘参数
void DiskDevice::SetDisk(const char* name, BufferManager* bm) {
	strcpy_s(this->disk_name, name);
	this->bm = bm;

	bDiskList.b_blkno = -1;
	bDiskList.b_addr = NULL;
	bDiskList.b_flags = BufFlag::B_NONE;
	bDiskList.b_forw = &bDiskList;
	bDiskList.b_back = &bDiskList;
	bDiskList.av_forw = &bDiskList;
	bDiskList.av_back = &bDiskList;
}

// 通过缓存读取磁盘
void DiskDevice::Dread(char* buf, unsigned int start_addr, unsigned int length) {
	// buf 为空: 没有存入的对象
	if (buf == NULL)
		return;

	// 位置及块数计算: 除法向下取整
	unsigned int pos = 0;
	unsigned int start_blkno = start_addr / BUFFER_SIZE;
	unsigned int end_blkno = (start_addr + length - 1) / BUFFER_SIZE;

	// 循环写入 buf 对象: 忙等
	for (unsigned int blkno = start_blkno; blkno <= end_blkno; blkno++) {
		// blkno块读入缓存
		Buf* bp = bm->Bread(blkno);

		// 一块内读完
		if (blkno == start_blkno && blkno == end_blkno) {
			memcpy_s(buf + pos, length, bp->b_addr + start_addr % BUFFER_SIZE, length);
			pos += length;
		}
		// 一块读不完: 读第一块
		else if (blkno == start_blkno) {
			memcpy_s(buf + pos, (BUFFER_SIZE - 1) - (start_addr % BUFFER_SIZE) + 1, bp->b_addr + start_addr % BUFFER_SIZE, (BUFFER_SIZE - 1) - (start_addr % BUFFER_SIZE) + 1);
			pos += BUFFER_SIZE - (start_addr % BUFFER_SIZE);
		}
		// 一块读不完: 读最后一块
		else if (blkno == end_blkno) {
			memcpy_s(buf + pos, (start_addr + length - 1) % BUFFER_SIZE - 0 + 1, bp->b_addr, (start_addr + length - 1) % BUFFER_SIZE - 0 + 1);
			pos += (start_addr + length - 1) % BUFFER_SIZE - 0 + 1;
		}
		// 一块读不完: 读中间块
		else {
			memcpy_s(buf + pos, BUFFER_SIZE, bp->b_addr, BUFFER_SIZE);
			pos += BUFFER_SIZE;
		}
	}
}

// 通过缓存写入磁盘(设置延迟写)
void DiskDevice::Dwrite(const char* buf, unsigned int start_addr, unsigned int length) {
	//cout << "Dwrite" << endl;
	// 位置及块数计算: 除法向下取整
	unsigned int pos = 0;
	unsigned int start_blkno = start_addr / BUFFER_SIZE;
	unsigned int end_blkno = (start_addr + length - 1) / BUFFER_SIZE;

	// 循环存入缓存: 忙等
	for (unsigned int blkno = start_blkno; blkno <= end_blkno; blkno++) {
		// blkno块读入缓存
		Buf* bp = bm->Bread(blkno);

		// 一块内写完
		if (blkno == start_blkno && blkno == end_blkno) {
			memcpy_s(bp->b_addr + start_addr % BUFFER_SIZE, length, buf + pos, length);
			pos += length;
		}
		// 一块写不完: 写第一块
		else if (blkno == start_blkno) {
			memcpy_s(bp->b_addr + start_addr % BUFFER_SIZE, (BUFFER_SIZE - 1) - (start_addr % BUFFER_SIZE) + 1, buf + pos, (BUFFER_SIZE - 1) - (start_addr % BUFFER_SIZE) + 1);
			pos += BUFFER_SIZE - (start_addr % BUFFER_SIZE);
		}
		// 一块写不完: 写最后一块
		else if (blkno == end_blkno) {
			memcpy_s(bp->b_addr, (start_addr + length - 1) % BUFFER_SIZE - 0 + 1, buf + pos, (start_addr + length - 1) % BUFFER_SIZE - 0 + 1);
			pos += (start_addr + length - 1) % BUFFER_SIZE - 0 + 1;
		}
		// 一块写不完: 写中间块
		else {
			memcpy_s(bp->b_addr, BUFFER_SIZE, buf + pos, BUFFER_SIZE);
			pos += BUFFER_SIZE;
		}

		// 设置延迟写, 必要时在bm->Bread/GetBlk中写入磁盘
		bm->Bdwirte(bp);
	}
}