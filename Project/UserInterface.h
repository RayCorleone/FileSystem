/*
	@name:      UserInterface.h
	@function:	UserInterface 类声明
	@author:    Ray
	@version:   1.0
	@problem:	None
	@timestamp: 2022/05/20
	@notice:	八进制以0开头, chmod需要输入的数据是八进制;
*/

#pragma once
#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include "Define.h"

using namespace std;

// 用户界面
class UserInterface {
private:
	FileSystem fs;		//文件系统
	string local_host;	//主机名

	int cur_uid;		//用户id
	int cur_gid;		//用户组id
	bool is_login;		//是否登录
	string cur_user;	//用户名
	string cur_group;	//用户组名
	string cur_dir;		//当前路径

	int cur_file_cnt;				//当前文件数
	File* cur_flist[FLIST_SIZE];	//当前打开文件表
	int cur_file_mode[FLIST_SIZE];	//当前打开文件权限表

protected:
	// 测试类指令(除去以下指令，还有exit/cd/cls/show)
	void _ftest();									//集成测试指令
	void _fhelp();									//显示测试指令帮助
	void _fformat();								//格式化文件系统
	void _flist();									//列出文件目录
	int _fsize(const int fd);						//文件大小(返回文件大小)
	void _dcreat(const char* name, const int mode);	//创建新目录
	void _ddelete(const char* name);				//删除目录
	int _fcreat(const char* name, const int mode);	//创建新文件(返回文件号)
	void _fdelete(const char* name);				//删除文件(无返回)
	int _fopen(const char* name, const int mode);	//打开文件(返回文件号)
	void _fclose(const int fd);						//关闭文件(无返回)
	int _fread(const int fd, char* buf, int size);	//读取文件(返回读多少数据)
	int _fwrite(const int fd, const char* buf, int size);//写入文件(返回写多少数据)
	int _flseek(const int fd, const int offset, const int ptrname);	//定位文件读写指针(返回位置)
	int _fcseek(const int fd);						//文件指针当前位置(返回当前位置)

	// 功能类指令
	void help();							//显示帮助
	void cls();								//清空命令行
	void disk();							//显示磁盘使用情况
	void format();							//格式化文件系统
	void ls();								//显示当前文件夹列表
	void path();							//显示当前路径
	void cd(const char* name);				//进入文件夹
	void mkdir(const char* name);			//新建文件夹
	void rmdir(const char* name);			//删除文件夹
	void touch(const char* name);			//新建文件
	void remove(const char* name);			//删除文件
	void chmod(const char* name, int mode);	//修改文件(夹)权限
	bool login(string name, string passwd);	//登录
	void logout();							//登出
	void addu(string uname, string gname, string passwd);	//添加用户
	void delu(string uname);				//删除用户
	void addg(string uname);				//添加用户组
	void delg(string uname);				//删除用户组
	void show(const char* name);			//显示文件
	void cp(const char* ori_fname, const char* tar_fname);		//fs中文件复制
	void win2fs(const char* win_fname, const char* fs_fname);	//从win复制文件到fs
	void fs2win(const char* fs_fname, const char* win_fname);	//从fs复制文件到win

public:
	UserInterface();	//构造函数
	~UserInterface();	//析构函数

	void TestRun();		//测试指令集运行函数
	void BetaRun();		//功能指令集运行函数
	void Run();			//主运行函数
};

#endif // !USER_INTERFACE_H
