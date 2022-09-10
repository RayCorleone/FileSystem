/*
	@name:      Inode.h
	@function:	Inode & DiskInode 类声明
	@author:    Ray
	@version:   1.0
	@problem:	None
	@timestamp: 2022/05/10
	@notice:	Inode 通过 DiskInode 的恢复/保存使用 memcpy 函数,
				因此前 64 字节的数据成员必须一一对应;
*/

#pragma once
#ifndef INODE_H
#define INODE_H

#include "Define.h"

using namespace std;

// 内存索引节点
class Inode {
public:
	Inode();		//构造函数
	~Inode();		//析构函数

	void Clean();	//清空Inode中数据

	unsigned int	i_mode;			//文件工作方式
	unsigned int	i_nlink;		//文件勾连数
	short			i_uid;			//所属User的id
	short			i_gid;			//所属Group的id
	unsigned int	i_size;			//文件大小(字节)
	unsigned int	i_addr[10];		//文件逻辑块号和物理块号转换索引表
	unsigned int	i_atime;		//最后访问时间
	unsigned int	i_mtime;		//最后修改时间

	int				i_number;		//DiskInode区中的编号
};

// 外存索引节点(64字节)
class DiskInode {
public:
	DiskInode();	//构造函数
	~DiskInode();	//析构函数

	unsigned int	d_mode;			//文件工作方式
	unsigned int	d_nlink;		//文件勾连数
	short			d_uid;			//所属User的id
	short			d_gid;			//所属Group的id
	unsigned int	d_size;			//文件大小(字节)
	unsigned int	d_addr[10];		//文件逻辑块号和物理块号转换索引表
	unsigned int	d_atime;		//最后访问时间
	unsigned int	d_mtime;		//最后修改时间
};

#endif // !INODE_H