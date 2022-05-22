/*
	@name:      SuperBlock.cpp
	@function:	SuperBlock 类定义
	@author:    Ray
	@version:   1.0
	@problem:	None
	@timestamp: 2022/05/10
	@notice:	None
*/

#include "Define.h"

using namespace std;

// SuperBlock构造函数
SuperBlock::SuperBlock() {
	this->s_inum = (INODE_SECTOR_TOTAL)*INODE_NUMBER_PER_SECTOR;
	this->s_ifnum = (INODE_SECTOR_TOTAL)*INODE_NUMBER_PER_SECTOR;
	this->s_bnum = DATA_SECTOR_TOTAL;
	this->s_bfnum = DATA_SECTOR_TOTAL;
	this->s_ninode = 0;
	this->s_nfree = 0;

	memset(this->s_inode, 0, sizeof(this->s_inode));
	memset(this->s_free, 0, sizeof(this->s_free));
	memset(this->padding, 0, sizeof(this->padding));
}

// SuperBlock析构函数
SuperBlock::~SuperBlock() {
	//Nothing to do
}