/*
	@name:      Buffer.cpp
	@function:	Buf & BufferManager 类定义
	@author:    Ray
	@version:   1.0
	@problem:	None
	@timestamp: 2022/05/10
	@notice:	None
*/

#include "Define.h"

using namespace std;

// BufferManager构造函数
BufferManager::BufferManager() {
	this->disk = NULL;
	memset(Buffer, 0, sizeof(Buffer));

	bFreeList.b_blkno = -1;
	bFreeList.b_addr = NULL;
	bFreeList.b_flags = BufFlag::B_NONE;
	bFreeList.b_forw = &bFreeList;
	bFreeList.b_back = &bFreeList;
	bFreeList.av_forw = &bFreeList;
	bFreeList.av_back = &bFreeList;

	Buf* bp;
	for (int i = 0; i < NBUF; i++){
		bp = &(this->m_Buf[i]);

		bp->b_blkno = -1;
		bp->b_addr = Buffer[i];
		bp->b_flags = BufFlag::B_NONE;

		//初始化自由队列
		Brelse(bp);
	}
}

// BufferManager析构函数
BufferManager::~BufferManager() {
	for (int i = 0; i < NBUF; i++)
		Brelse(&m_Buf[i]);
	memset(Buffer, 0, sizeof(Buffer));
}

// 缓存管理初始化: 缓存控制块队列的初始化,同时载入磁盘设备
void BufferManager::Initialize(DiskDevice* disk) {
	this->disk = disk;
	memset(Buffer, 0, sizeof(Buffer));

	bFreeList.b_blkno = -1;
	bFreeList.b_addr = NULL;
	bFreeList.b_flags = BufFlag::B_NONE;
	bFreeList.b_forw = &bFreeList;
	bFreeList.b_back = &bFreeList;
	bFreeList.av_forw = &bFreeList;
	bFreeList.av_back = &bFreeList;

	Buf* bp;
	for (int i = 0; i < NBUF; i++) {
		bp = &(this->m_Buf[i]);

		bp->b_blkno = -1;
		bp->b_addr = Buffer[i];
		bp->b_flags = BufFlag::B_NONE;

		//初始化自由队列
		Brelse(bp);
	}
}

// 申请缓存块
Buf* BufferManager::GetBlk(unsigned int blkno) {
	Buf* bp = NULL;

	// 1.先在设备的挂载队列上找之前有没有 Buf 读过该块
	for (bp = disk->bDiskList.b_back; bp != &(disk->bDiskList); bp = bp->b_back) {
		if (bp->b_blkno == blkno)
			return bp;
	}
	// 2.自由队列空
	if (bFreeList.av_back == &bFreeList) {
		// 使用忙等, I/O请求队列中最多有一个缓存，自由队列不可能为空
		throw("-!ERROR: 缓存自由队列为空, BufferManager初始化出错.\n");
	}
	// 3.从自由队列取一个缓存块
	else {
		//(1)从自由队列取出
		bp = bFreeList.av_back;
		bp->av_forw->av_back = bp->av_back;
		bp->av_back->av_forw = bp->av_forw;
		bp->b_forw->b_back = bp->b_back;
		bp->b_back->b_forw = bp->b_forw;

		//(2)含有延迟写
		if ((bp->b_flags & BufFlag::B_DELWRI) == BufFlag::B_DELWRI){
			Bwrite(bp);		//立即写入磁盘
			bp->b_flags = bp->b_flags & (~BufFlag::B_DELWRI);	//清除延迟写标志
		}

		//(3)清除所有标志
		bp->b_flags = BufFlag::B_NONE;

		//(4)填写blkno, 插入设备队列队头
		bp->b_blkno = blkno;
		bp->b_forw = &(disk->bDiskList);
		bp->b_back = disk->bDiskList.b_back;
		bp->b_forw->b_back = bp;
		bp->b_back->b_forw = bp;
	}
	return bp;
}

// 释放缓存块
void BufferManager::Brelse(Buf* bp) {
	//有延迟写标志的,  释放时写入磁盘
	if ((bp->b_flags & BufFlag::B_DELWRI) == BufFlag::B_DELWRI) {
		Bwrite(bp);		//立即写入磁盘
		bp->b_flags = bp->b_flags & (~BufFlag::B_DELWRI);	//清除延迟写标志
	}

	//清除所有标志
	bp->b_flags = BufFlag::B_NONE;

	//重新置入自由队列
	bp->av_forw = bFreeList.av_forw;
	bp->av_back = &bFreeList;
	bp->av_forw->av_back = bp;
	bp->av_back->av_forw = bp;

	//NODEV队列无需维护
	bp->b_forw = &bFreeList;
	bp->b_back = &bFreeList;
}

// 读一个盘块
Buf* BufferManager::Bread(unsigned int blkno){
	//获取一个blkno盘块缓存
	Buf* bp = GetBlk(blkno);

	//缓存块已经有数据,可以直接使用
	if ((bp->b_flags & BufFlag::B_DONE) == BufFlag::B_DONE)
		return bp;

	//缓存块不可以直接使用: 忙等读取
	else {
		//I/O读操作，送入I/O请求队列
		bp->av_forw = disk->bDiskList.av_forw;
		bp->av_back = &(disk->bDiskList);
		bp->av_forw->av_back = bp;
		bp->av_back->av_forw = bp;

		//开始读操作
		fstream fin;
		fin.open(disk->disk_name, ios::in | ios::binary);
		if (!fin.is_open())
			throw("-!ERROR: 磁盘映像输入文件流打开失败.\n");
		fin.seekg(streampos(blkno) * streampos(BUFFER_SIZE), ios::beg);
		fin.read(bp->b_addr, BUFFER_SIZE);
		fin.close();

		//更新标志
		bp->b_flags = BufFlag::B_DONE;
		
		//从I/O请求队列取出
		bp->av_forw->av_back = bp->av_back;
		bp->av_back->av_forw = bp->av_forw;

		//重新加入自由队列
		bp->av_forw = bFreeList.av_forw;
		bp->av_back = &bFreeList;
		bp->av_forw->av_back = bp;
		bp->av_back->av_forw = bp;
		
		return bp;
	}
}

// 写一个盘块
void BufferManager::Bwrite(Buf* bp) {
	//I/O写操作，送入I/O请求队列
	bp->av_forw = disk->bDiskList.av_forw;
	bp->av_back = &(disk->bDiskList);
	bp->av_forw->av_back = bp;
	bp->av_back->av_forw = bp;

	//开始写操作
	fstream fout;
	fout.open(disk->disk_name, ios::in | ios::out | ios::binary);
	if (!fout.is_open())
		throw("-!ERROR: 磁盘映像输出文件流打开失败.\n");
	fout.seekp(streampos(bp->b_blkno) * streampos(BUFFER_SIZE), ios::beg);
	fout.write((const char*)bp->b_addr, BUFFER_SIZE);
	fout.close();

	//更新标志
	bp->b_flags = BufFlag::B_DONE;

	//从I/O请求队列取出
	bp->av_forw->av_back = bp->av_back;
	bp->av_back->av_forw = bp->av_forw;

	//加入自由队列
	bp->av_forw = bFreeList.av_forw;
	bp->av_back = &bFreeList;
	bp->av_forw->av_back = bp;
	bp->av_back->av_forw = bp;
}

// 设置延迟写标志
void  BufferManager::Bdwirte(Buf* bp){
	bp->b_flags |= BufFlag::B_DELWRI;
}
