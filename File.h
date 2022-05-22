/*
	@name:      File.h
	@function:	File & DirectoryEntry 类声明
	@author:    Ray
	@version:   1.0
	@problem:	None
	@timestamp: 2022/05/10
	@notice:	File 中 f_inode.i_uid(i_gid)是创建者(组)的 id, 可以与
				打开者(组)的 f_uid(f_gid)区别，以区分权限;
*/

#pragma once
#ifndef FILE_H
#define FILE_H

#include "Define.h"

using namespace std;

// 文件
class File {
public:
	File();		//构造函数
	~File();	//析构函数

	short			f_uid;		//打开文件的用户id(模拟OpenFile)
	short			f_gid;		//打开文件的用户组id(模拟OpenFile)
	Inode*			f_inode;	//指向文件对应的Inode指针
	unsigned int	f_offset;	//文件读写指针的位置
};

// 目录入口
class DirectoryEntry {
public:
	DirectoryEntry();	//构造函数
	~DirectoryEntry();	//析构函数

	int		m_ino;				//目录项中Inode编号
	char	m_name[NAME_SIZE];	//目录项中路径名称
};

#endif // !FILE_H
