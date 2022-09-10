/*
	@name:      FileSystem.h
	@function:	FileSystem 类声明
	@author:    Ray
	@version:   1.0
	@problem:	None
	@timestamp: 2022/05/10
	@notice:	fformat中SuperBlock写入文件后需要重读, 否则Spb不匹配;
				fformat中构建了文件系统的基本结构，不含三个文件的复制;
				根目录的权限设置为0777; 文件夹默认0764; 文件默认0664.
*/

#pragma once
#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "Define.h"

using namespace std;

// 文件系统
class FileSystem{
private:
	SuperBlock spb;			//存储资源管理对象
	DiskDevice disk;		//磁盘设备对象
	BufferManager buffer;	//缓存管理对象

	void Initialize();		//初始化文件系统

	//InodeManager
	Inode ILoad(int i_no);		//载入特定Inode
	void  ISave(Inode inode);	//保存特定Inode
	Inode IAlloc();				//分配一个Inode
	void  IFree(Inode inode);	//释放一个Inode

	//BlockManager
	int  BAlloc();			//分配一个盘块
	void BFree(int no);		//释放一个盘块

	//FileManager
	void  FDelete(Inode& cur_inode);		//根据Inode删除文件
	void  FDeleteall(Inode& cur_inode);		//根据Inode删除子项
	Inode FFind(Inode& dir_inode, const char* name);	//根据文件名定位Inode(在目录项中)
	void  FRead(Inode& cur_inode, char* buf, unsigned int offset, unsigned int len);		//根据Inode读取数据到buf
	void  FWrite(Inode& cur_inode, const char* buf, unsigned int offset, unsigned int len);	//根据Inode写buf数据到磁盘

public:
	FileSystem();	//构造函数
	~FileSystem();	//析构函数

	SuperBlock* GetSpb();	//获得SuperBlock
	void SaveSpb();			//保存SuperBlock
	void LoadSpb();			//载入SuperBlock
	void fformat();			//格式化文件系统

	int dcreate(const char* dir, short uid = 0, short gid = 0);	//创建新目录
	int ddelete(const char* dir, short uid = 0, short gid = 0);	//删除目录
	bool enter(const char* dir, short uid = 0, short gid = 0);	//进入目录
	int fcreate(const char* dir, short uid = 0, short gid = 0);	//创建新文件
	int fdelete(const char* dir, short uid = 0, short gid = 0);	//删除文件
	File* fopen(const char* dir, short uid = 0, short gid = 0);	//打开文件
	int fclose(File* fp);	//关闭文件
	int fcseek(File* fp);	//获得文件读写指针位置
	int fseek(File* fp, int offset, int mode);	//修改文件读写指针
	int fread(char* buf, int size, File* fp);			//读文件到buf
	int fwrite(const char* buf, int size, File* fp);	//写buf到文件
	int freplace(const char* buf, int size, File* fp);	//写buf到文件(替换)

	int chmod(const char* dir, int mode, short uid = 0, short gid = 0);	//修改权限
	vector<string> flist(const char* dir, short uid = 0, short gid = 0);//列出文件信息
};

#endif // !FILESYSTEM_H
