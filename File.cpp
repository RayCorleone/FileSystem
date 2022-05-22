/*
	@name:      File.cpp
	@function:	File & DirectoryEntry 类定义
	@author:    Ray
	@version:   1.0
	@problem:	None
	@timestamp: 2022/05/10
	@notice:	None
*/

#include "Define.h"

using namespace std;

// File构造函数
File::File() {
	this->f_uid = -1;
	this->f_gid = -1;
	this->f_offset = 0;
	this->f_inode = NULL;
}

// File析构函数
File::~File() {
	//Nothing to do
}

// DirectoryEntry构造函数
DirectoryEntry::DirectoryEntry() {
	this->m_ino = 0;
	this->m_name[0] = '\0';
}

// DirectoryEntry析构函数
DirectoryEntry::~DirectoryEntry() {
	//Nothing to do
}
