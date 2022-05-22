/*
	@name:      Define.h
	@function:	常量、类、通用函数的预声明
	@author:    Ray
	@version:   1.0
	@problem:	None
	@timestamp: 2022/05/22
	@notice:	None
*/

#pragma once
#ifndef DEFINE_H
#define DEFINE_H

//###########################################
//@通用头文件
#include <ctime>
#include <vector>
#include <string>
#include <conio.h>
#include <stdio.h>
#include <iomanip>
#include <fstream>
#include <iostream>
#include <windows.h>

//###########################################
//@常量
// 0.根用户信息
#define ADMIN		"admin"			//管理员用户名
#define ADMIN_PSW	"password"		//管理员用户名
#define ROOT_GROUP	"root"			//管理员用户组名

// 1.文件信息
#define SMALL_FILE_BLOCK	6					//小型文件：直接索引表最多可寻址的逻辑块号
#define LARGE_FILE_BLOCK	128*2+6				//大型文件：经一次间接索引表最多可寻址的逻辑块号
#define HUGE_FILE_BLOCK		128*128*2+128*2+6	//巨型文件：经二次间接索引最大可寻址文件逻辑块号

// 2.大小信息
#define NBUF		15		//缓存个数
#define DIR_SIZE	32		//目录项大小
#define NAME_SIZE	28		//文件名大小
#define FLIST_SIZE	10		//最大打开文件个数
#define INODE_SIZE	64		//外存Inode大小
#define BLOCK_SIZE 	512		//逻辑块大小
#define	BUFFER_SIZE 512		//缓存块大小

// 3.磁盘信息
#define DISK_NAME		"FileSystem.img"	//文件卷名
#define DISK_SECTOR		262144 				//磁盘总扇区数
#define ROOT_INO		0		//根目录Inode号
#define ROOT_DIR_SYMBOL "root"

// 4.分区信息
#define SUPER_BLOCK_START_SECTOR	200		//SuperBlock起始扇区号
#define INODE_ZONE_START_SECTOR 	202 	//Inode区起始扇区号
#define DATA_ZONE_START_SECTOR		1024	//数据区的起始扇区号

// 5.地址信息
#define SUPER_BLOCK_START_ADDR	BLOCK_SIZE * SUPER_BLOCK_START_SECTOR	//SuperBlock起始地址
#define INODE_ZONE_START_ADDR	BLOCK_SIZE * INODE_ZONE_START_SECTOR	//Inode区起始地址
#define DATA_ZONE_START_ADDR	BLOCK_SIZE * DATA_ZONE_START_SECTOR		//数据区起始地址

// 6.限制信息
#define MAX_DISK_SIZE	BLOCK_SIZE * DISK_SECTOR		//磁盘最大大小
#define MAX_FILE_SIZE	BLOCK_SIZE * HUGE_FILE_BLOCK	//文件最大大小

// 7.额外个数信息
#define DIR_NUMBER_PER_SECTOR	BLOCK_SIZE / DIR_SIZE		//单个盘块目录个数
#define INODE_NUMBER_PER_SECTOR BLOCK_SIZE / INODE_SIZE		//单个盘块外存Inode个数
#define	INODE_SECTOR_TOTAL		DATA_ZONE_START_SECTOR - INODE_ZONE_START_SECTOR	//Inode区总盘块数
#define	DATA_SECTOR_TOTAL		DISK_SECTOR - DATA_ZONE_START_SECTOR				//数据区总盘块数

// 8.文件状态标志(Inode & DiskInode's mode)
#define	IALLOC	1 << 15		//15	已经被使用
#define	IFCHR	1 << 13		//14-13 字符设备
#define	IFDIR	2 << 13		//14-13 目录文件
#define IFMT	3 << 13		//14-13	块设备
#define ILARG	1 << 12		//12	大型或巨型文件
#define IREAD	4 << 6		//8		对文件的读权限
#define IWRITE	2 << 6		//7		对文件的写权限
#define IEXEC	1 << 6		//6		对文件的执行权限
#define IRWXU	(IREAD|IWRITE|IEXEC)	//8-6	文件主的权限
#define IRWXG	((IREAD|IWRITE)>>3)		//5-3	同组用户的权限
#define IRWXO	((IREAD)>>6)			//2-0	其他用户的权限
#define PFILE	(IREAD|IWRITE|IRWXG|IRWXO)		//文件的权限
#define PDIR	(IRWXU|IRWXG|IRWXO)				//目录的权限
#define RDIR	(IRWXU|(IRWXU>>3)|(IRWXU>>6))	//根目录的权限

//###########################################
//@枚举定义
enum BufFlag {	// b_flag中的标志位
	B_NONE	= 0x0,		//无标志
	B_WRITE = 0x1,		//(x)写操作, 将缓存信息写到硬盘
	B_READ	= 0x2,		//(x)读操作, 从盘读取信息到缓存
	B_DONE	= 0x4,		//读写完成
	B_ERROR = 0x8,		//(x)I/O因出错而终止
	B_BUSY	= 0x10,		//(x)相应缓存正在使用中
	B_WANTED= 0x20,		//(x)有进程等待使用该buf管理的缓存
	B_ASYNC = 0x40,		//(x)异步I/O, 不需要等待其结束
	B_DELWRI= 0x80		//延迟写, 在相应缓存要移做他用时，再将其内容写到相应块设备上
};

//###########################################
//@类声明
class Buf;				//缓存块
class File;				//文件
class Inode;			//内存索引节点
class DiskInode;		//外存索引节点(和Inode对应)
class SuperBlock;		//存储资源管理块
class DiskDevice;		//磁盘设备
class FileSystem;		//文件系统
class BufferManager;	//缓存管理
class UserInterface;	//用户界面
class DirectoryEntry;	//目录入口

//###########################################
//@自写头文件
#include "File.h"
#include "Inode.h"
#include "Buffer.h"
#include "DiskDevice.h"
#include "SuperBlock.h"
#include "FileSystem.h"
#include "UserInterface.h"

//###########################################
//@辅助函数声明
using namespace std;

int get_fsize_from_windows(const char* fname);			//从window读取文件大小
char* read_from_windows(const char* fname);				//从window读取文件数据

string wstring2string(wstring wstr);					//当前主机名称转化
vector<string> splitstr(string str, string pattern);	//输入字段分割

#endif // !DEFINE_H
