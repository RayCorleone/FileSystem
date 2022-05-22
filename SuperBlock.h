/*
	@name:      SuperBlock.h
	@function:	SuperBlock 类声明
	@author:    Ray
	@version:   1.0
	@problem:	None
	@timestamp: 2022/05/10
	@notice:	padding[50] 这里可有可无, 之后对 SuperBlock 分空间时使用
				的是绝对大小 2 * BLOCK_SIZE, 不是 sizeof(SuperBlock());
*/

#pragma once
#ifndef SUPERBLOCK_H
#define SUPERBLOCK_H

#include "Define.h"

using namespace std;

// 存储资源管理块
class SuperBlock {
public:
	SuperBlock();	//构造函数
	~SuperBlock();	//析构函数

	unsigned int s_inum;		//Inode总个数
	unsigned int s_ifnum;		//空闲Inode总个数
	unsigned int s_ninode;		//直接管理的空闲外存Inode数
	unsigned int s_inode[100];	//直接管理的空闲外存Inode索引表

	unsigned int s_bnum;		//磁盘盘块总数
	unsigned int s_bfnum;		//空闲盘块总数
	unsigned int s_nfree;		//直接管理的空闲盘块数
	unsigned int s_free[100];	//直接管理的空闲盘块索引表

	unsigned int padding[50];	//填充SuperBlock至1024字节
};

#endif // !SUPERBLOCK_H