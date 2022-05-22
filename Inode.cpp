/*
	@name:      Inode.cpp
	@function:	Inode & DiskInode 类定义
	@author:    Ray
	@version:   1.0
	@problem:	None
	@timestamp: 2022/05/10
	@notice:	None
*/

#include "Define.h"

using namespace std;

// Inode构造函数
Inode::Inode() {
	this->i_mode = 0;
	this->i_nlink = 0;
	this->i_uid = -1;
	this->i_gid = -1;
	this->i_size = 0;
	this->i_number = -1;
	for (int i = 0; i < 10; i++) {
		this->i_addr[i] = 0;
	}
	this->i_atime = 0;
	this->i_mtime = 0;
}

// Inode析构函数
Inode::~Inode() {
	//Nothing to do
}

// 清空Inode中数据
void Inode::Clean() {
	this->i_mode = 0;
	this->i_nlink = 0;
	this->i_uid = -1;
	this->i_gid = -1;
	this->i_size = 0;
	for (int i = 0; i < 10; i++) {
		this->i_addr[i] = 0;
	}
}

// DiskInode构造函数
DiskInode::DiskInode() {
	this->d_mode = 0;
	this->d_nlink = 0;
	this->d_uid = -1;
	this->d_gid = -1;
	this->d_size = 0;
	for (int i = 0; i < 10; i++) {
		this->d_addr[i] = 0;
	}
	this->d_atime = 0;
	this->d_mtime = 0;
}

// DiskInode析构函数
DiskInode::~DiskInode() {
	//Nothing to do
}