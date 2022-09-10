/*
	@name:      File.cpp
	@function:	File & DirectoryEntry �ඨ��
	@author:    Ray
	@version:   1.0
	@problem:	None
	@timestamp: 2022/05/10
	@notice:	None
*/

#include "Define.h"

using namespace std;

// File���캯��
File::File() {
	this->f_uid = -1;
	this->f_gid = -1;
	this->f_offset = 0;
	this->f_inode = NULL;
}

// File��������
File::~File() {
	//Nothing to do
}

// DirectoryEntry���캯��
DirectoryEntry::DirectoryEntry() {
	this->m_ino = 0;
	this->m_name[0] = '\0';
}

// DirectoryEntry��������
DirectoryEntry::~DirectoryEntry() {
	//Nothing to do
}
