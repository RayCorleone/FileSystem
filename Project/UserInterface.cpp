/*
	@name:      UserInterface.cpp
	@function:	UserInterface 类定义
	@author:    Ray
	@version:   1.0
	@problem:	None
	@timestamp: 2022/05/22
	@notice:	None
*/

#include "Define.h"

using namespace std;

/*################################################################*/
// UserInterface构造函数
UserInterface::UserInterface() {
	is_login = false;
	cur_uid = -1;
	cur_user = "";
	cur_gid = -1;
	cur_group = "";
	cur_dir = "";

	cur_file_cnt = 0;
	for (int i = 0; i < FLIST_SIZE; i++) {
		cur_flist[i] = NULL;
		cur_file_mode[i] = 0;
	}

	// 获取主机名
	DWORD size = 0;
	wstring wstr;
	GetComputerName(NULL, &size); //得到电脑名称长度
	wchar_t* name = new wchar_t[size];
	if (GetComputerName(name, &size))
		wstr = name;
	delete[] name;
	local_host = wstring2string(wstr);
}

// UserInterface析构函数
UserInterface::~UserInterface() {
	local_host = "";

	is_login = false;
	cur_uid = -1;
	cur_user = "";
	cur_gid = -1;
	cur_group = "";
	cur_dir = "";

	cur_file_cnt = 0;
	for (int i = 0; i < FLIST_SIZE; i++) {
		cur_flist[i] = NULL;
		cur_file_mode[i] = 0;
	}
}

/*################################################################*/
// 集成测试指令
void UserInterface::_ftest() {
	char* abc = new char[800]{ 0 };
	char init[1000];
	for (int i = 0; i < 1000; i++) {
		if (i < 500)
			init[i] = 'a' + i % 26;
		else
			init[i] = 'A' + i % 26;
	}

	// 格式化系统
	_fformat();

	cout << "(1)新建文件/test/Jerry:" << endl;
	_dcreat("test", 0777);
	int fd = _fcreat("test/Jerry", 0600);
	cout << "-创建的文件的句柄:" << fd << endl;
	cd("test");
	_flist();

	cout << "\n(2)向test/Jerry文件写入800字节:" << endl;
	cout << "-向句柄" << fd << "写入" << _fwrite(fd, init, 800) << "字节" << endl;
	cout << "-句柄" << fd << "的大小:" << _fsize(fd) << endl;

	cout << "\n(3)将文件指针定位到500字节:" << endl;
	cout << "-句柄" << fd << "修改之前的指针:" << _fcseek(fd) << endl;
	cout << "-句柄" << fd << "修改之后的指针:" << _flseek(fd, 500, 0) << endl;

	cout << "\n(4)读出500字节到abc:" << endl;
	cout << "-从句柄" << fd << "读出字节数:" << _fread(fd, abc, 500) << endl;
	cout << "-字符串abc的值:\n" << abc << endl;

	cout << "\n(5)将abc写回文件:" << endl;
	cout << "-向句柄" << fd << "写入" << _fwrite(fd, abc, 500) << "字节" << endl;
	cout << "-句柄" << fd << "的大小:" << _fsize(fd) << endl;
	_fclose(fd);

	cout << "\n-写回文件后Jerry的大小:\n";
	_flist();
	cout << "\n-写回文件后Jerry的内容:\n";
	show("Jerry");

	cout << "\n";
	return;
}

// 显示测试指令帮助
void UserInterface::_fhelp() {
	printf("- cls                                     - 清空命令行\n");
	printf("- ftest                                   - 一步实现测试\n");
	printf("- fhelp                                   - 显示命令清单\n");
	printf("- fformat                                 - 格式化文件系统\n");
	printf("- flist                                   - 显示当前文件夹列表\n");
	printf("- cd      <dir name>                      - 进入文件夹\n");
	printf("- dcreat  <dir name> <mode(OTC)>          - 新建文件夹\n");
	printf("- ddelete <dir name>                      - 删除文件夹\n");
	printf("- fcreat  <file name> <mode(OTC)>         - 新建文件(返回文件句柄)\n");
	printf("- fdelete <file name>                     - 删除文件\n");
	printf("- fshow   <file name>                     - 显示文件\n");
	printf("- fopen   <file name> <mode(OTC)>         - 打开文件(返回文件句柄)\n");
	printf("- fclose  <句柄 fd>                       - 关闭文件\n");
	printf("- fread   <句柄 fd> <buf abc> <size>      - 读取文件内容到abc\n");
	printf("- fwrite  <句柄 fd> <init> <size>         - 初始化文件内容\n");
	printf("- fwrite  <句柄 fd> <buf abc> <size>      - 将abc中内容写入文件\n");
	printf("- flseek  <句柄 fd> <offset> <ptrname>    - 修改文件指针\n");
	printf("- fcseek  <句柄 fd>                       - 返回文件指针\n");
	printf("- fsize   <句柄 fd>                       - 返回fd文件大小\n");
	printf("- exit                                    - 退出系统\n");
}

// 格式化文件系统
void UserInterface::_fformat() {
	tm stdT;
	time_t time1 = unsigned int(time(NULL));
	localtime_s(&stdT, (const time_t*)&time1);
	printf_s("-NOTICE: 正在格式化文件系统, 开始时间: %02d:%02d:%02d(不应超过2分钟).\n", stdT.tm_hour, stdT.tm_min, stdT.tm_sec);

	logout();
	login(ADMIN, ADMIN_PSW);
	cur_dir = "";
	cur_file_cnt = 0;
	for (int i = 0; i < FLIST_SIZE; i++) {
		cur_flist[i] = NULL;
		cur_file_mode[i] = NULL;
	}
	fs.fformat();

	time_t time2 = unsigned int(time(NULL));
	localtime_s(&stdT, (const time_t*)&time2);
	printf_s("-NOTICE: 文件系统格式化成功, 结束时间: %02d:%02d:%02d.\n\n", stdT.tm_hour, stdT.tm_min, stdT.tm_sec);
}

// 列出文件目录
void UserInterface::_flist() {
	vector<string>list = fs.flist(cur_dir.data(), cur_uid, cur_gid);
	for (unsigned int i = 0; i < list.size(); i++)
		cout << list[i];
}

// 文件大小(返回文件大小)
int UserInterface::_fsize(const int fd) {
	if (cur_flist[fd] == NULL)
		return -1;
	else {
		return cur_flist[fd]->f_inode->i_size;
	}
}

// 创建新目录
void UserInterface::_dcreat(const char* name, const int mode) {
	string new_dir = string(name);
	if (new_dir.find("/") != string::npos)
		throw("-!ERROR: 参数中不能包含多级目录.\n");

	string dir = cur_dir + "/" + new_dir;
	fs.dcreate(dir.data(), cur_uid, cur_gid);
	fs.chmod(dir.data(), mode, cur_uid, cur_gid);
}

// 删除目录
void UserInterface::_ddelete(const char* name) {
	string new_dir = string(name);
	if (new_dir.find("/") != string::npos)
		throw("-!ERROR: 参数中不能包含多级目录.\n");

	string dir = cur_dir + "/" + new_dir;
	fs.ddelete(dir.data(), cur_uid, cur_gid);
}

// 创建新文件(返回文件号)
int UserInterface::_fcreat(const char* name, const int mode) {
	string sub_dir = "";
	string fs_dir = cur_dir;
	vector<string> route = splitstr(name, "/");

	for (int i = 0; i < route.size() - 1; i++) {
		sub_dir = route[i];
		string dir = fs_dir + "/" + sub_dir;

		if (fs.enter(dir.data(), cur_uid, cur_gid)) {
			if (sub_dir == ".") {}
			else if (sub_dir == "..") {
				size_t pos = fs_dir.find_last_of('/');
				fs_dir = fs_dir.substr(0, pos);
			}
			else
				fs_dir = dir;
		}
		else
			throw("-!ERROR: 无法进入指定目录.\n");
	}
	fs_dir = fs_dir + "/" + route[route.size() - 1];

	string dir = fs_dir;
	fs.fcreate(dir.data(), cur_uid, cur_gid);
	fs.chmod(dir.data(), mode, cur_uid, cur_gid);

	if (cur_file_cnt < FLIST_SIZE) {	//还有句柄
		File* fp = fs.fopen(dir.data(), cur_uid, cur_gid);
		for (int i = 0; i < FLIST_SIZE; i++) {
			if (cur_flist[i] == NULL) {
				cur_flist[i] = fp;
				cur_file_mode[i] = (mode >> 6);
				cur_file_cnt = cur_file_cnt + 1;
				return i;
			}
		}
	}

	cout << "-NOTICE: 文件创建成功, 但没有空余的文件句柄." << endl;
	return -1;
}

// 删除文件(无返回)
void UserInterface::_fdelete(const char* name) {
	string sub_dir = "";
	string fs_dir = cur_dir;
	vector<string> route = splitstr(name, "/");

	for (int i = 0; i < route.size() - 1; i++) {
		sub_dir = route[i];
		string dir = fs_dir + "/" + sub_dir;

		if (fs.enter(dir.data(), cur_uid, cur_gid)) {
			if (sub_dir == ".") {}
			else if (sub_dir == "..") {
				size_t pos = fs_dir.find_last_of('/');
				fs_dir = fs_dir.substr(0, pos);
			}
			else
				fs_dir = dir;
		}
		else
			throw("-!ERROR: 无法进入指定目录.\n");
	}
	fs_dir = fs_dir + "/" + route[route.size() - 1];

	string dir = fs_dir;

	fs.fdelete(dir.data(), cur_uid, cur_gid);
	return;
}

// 打开文件(返回文件号)
int UserInterface::_fopen(const char* name, const int mode) {
	string sub_dir = "";
	string fs_dir = cur_dir;
	vector<string> route = splitstr(name, "/");

	for (int i = 0; i < route.size() - 1; i++) {
		sub_dir = route[i];
		string dir = fs_dir + "/" + sub_dir;

		if (fs.enter(dir.data(), cur_uid, cur_gid)) {
			if (sub_dir == ".") {}
			else if (sub_dir == "..") {
				size_t pos = fs_dir.find_last_of('/');
				fs_dir = fs_dir.substr(0, pos);
			}
			else
				fs_dir = dir;
		}
		else
			throw("-!ERROR: 无法进入指定目录.\n");
	}
	fs_dir = fs_dir + "/" + route[route.size() - 1];

	string dir = fs_dir;
	if (cur_file_cnt < FLIST_SIZE) {	//还有句柄
		File* fp = fs.fopen(dir.data(), cur_uid, cur_gid);
		for (int i = 0; i < FLIST_SIZE; i++) {
			if (cur_flist[i] == NULL) {
				cur_flist[i] = fp;

				int file_mode = 0;
				if (fp->f_uid == fp->f_inode->i_uid)
					file_mode = 6;
				else if (fp->f_gid == fp->f_inode->i_gid)
					file_mode = 3;
				else
					file_mode = 0;

				cur_file_mode[i] = (fp->f_inode->i_mode << 7 >> (7 + file_mode)) & (mode);
				cur_file_cnt = cur_file_cnt + 1;
				return i;
			}
		}
	}

	cout << "-NOTICE: 没有空余的文件句柄, 文件打开失败." << endl;
	return -1;
}

// 关闭文件(无返回)
void UserInterface::_fclose(const int fd) {
	if (cur_flist[fd] == NULL)
		cout << "-NOTICE: 文件句柄不存在." << endl;
	else {
		try {
			fs.fclose(cur_flist[fd]);
		}
		catch(const char* errMsg){
			//pass
		}
		cur_flist[fd] = NULL;
		cur_file_mode[fd] = 0;
		cur_file_cnt = cur_file_cnt - 1;
	}
	return;
}

// 读取文件(返回读多少数据)
int UserInterface::_fread(const int fd, char* buf, int size) {
	if (cur_flist[fd] == NULL)
		cout << "-NOTICE: 文件句柄不存在." << endl;
	else if (cur_file_mode[fd] & 4) {
		return fs.fread(buf, size, cur_flist[fd]);
	}
	else
		cout << "-NOTICE: 没有读权限." << endl;
	return -1;
}

// 写入文件(返回写多少数据)
int UserInterface::_fwrite(const int fd, const char* buf, int size) {
	if (cur_flist[fd] == NULL)
		cout << "-NOTICE: 文件句柄不存在." << endl;
	else if (cur_file_mode[fd] & 2) {
		return fs.fwrite(buf, size, cur_flist[fd]);
	}
	else
		cout << "-NOTICE: 没有写权限." << endl;

	return -1;
}

// 定位文件读写指针(返回位置)
int UserInterface::_flseek(const int fd, const int offset, const int ptrname) {
	if (cur_flist[fd] == NULL)
		cout << "-NOTICE: 文件句柄不存在." << endl;
	else {
		return fs.fseek(cur_flist[fd], offset, ptrname);
	}
	return -1;
}

// 文件指针当前位置(返回当前位置)
int UserInterface::_fcseek(const int fd) {
	if (cur_flist[fd] == NULL)
		return -1;
	else {
		return fs.fcseek(cur_flist[fd]);
	}
}

/*################################################################*/
// 显示帮助
void UserInterface::help() {
	if (!is_login) {
		printf("- help                     - 显示命令清单   (未登录可用)\n");
		printf("- cls                      - 清空命令行     (未登录可用)\n");
		printf("- format                   - 格式化文件系统 (未登录可用)\n");
		printf("- login   <user name>      - 用户登录       (未登录可用)\n");
		printf("- exit                     - 退出系统       (未登录可用)\n");
		printf("              ......登录查看更多指令......              \n");
		//printf("\033[40;36mp.s.管理员名:%s | 密码:%s | 初始用户组:%s\n\n\033[0m", ADMIN, ADMIN_PSW, ROOT_GROUP);//(浅蓝)cur_user@LOCAL_HOST:
		printf("p.s.管理员名:%s | 密码:%s | 初始用户组:%s\n\n", ADMIN, ADMIN_PSW, ROOT_GROUP);//(浅蓝)cur_user@LOCAL_HOST:
	}
	else {
		printf("- help                                    - 显示命令清单\n");
		printf("- cls                                     - 清空命令行\n");
		printf("- disk                                    - 显示磁盘使用情况\n");
		printf("- format                                  - 格式化文件系统\n");
		printf("- ls                                      - 显示当前文件夹列表\n");
		printf("- path                                    - 显示当前路径\n");
		printf("- cd      <dir name>                      - 进入文件夹\n");
		printf("- mkdir   <dir name>                      - 新建文件夹\n");
		printf("- rmdir   <dir name>                      - 删除文件夹\n");
		printf("- touch   <file name>                     - 新建文件\n");
		printf("- remove  <file name>                     - 删除文件\n");
		printf("- chmod   <file/dir name> <mode(OTC)>     - 修改文件(夹)权限\n");
		printf("- login   <user name>                     - 用户登录\n");
		printf("- logout                                  - 用户退出\n");
		printf("- addu    <user name> <group name>        - 添加用户\n");
		printf("- delu    <user name>                     - 删除用户\n");
		printf("- addg    <group name>                    - 添加用户组\n");
		printf("- delg    <group name>                    - 删除用户组\n");
		printf("- show    <file name>                     - 显示文件\n");
		printf("- cp      <source name> <target name>     - 文件系统内文件复制\n");
		printf("- win2fs  <win file name> <fs file name>  - 将Windows文件内容复制到FS文件系统文件\n");
		printf("- fs2win  <fs file name> <win file name>  - 将FS文件系统文件内容复制到Windows文件\n");
		printf("- exit                                    - 退出系统\n");
	}
}

// 清空命令行
void UserInterface::cls() {
	system("cls");
}

// 显示磁盘使用情况
void UserInterface::disk() {
	SuperBlock* superblock = fs.GetSpb();

	int size = superblock->s_bnum * BLOCK_SIZE;
	int used = (superblock->s_bnum - superblock->s_bfnum) * BLOCK_SIZE;
	int unused = superblock->s_bfnum * BLOCK_SIZE;
	double use = 100 * double((double)superblock->s_bnum - (double)superblock->s_bfnum) / double(superblock->s_bnum);
	int inode = superblock->s_inum;
	int iunused = superblock->s_ifnum;

	printf("%-10s\t|%-10s\t|%-10s\t|%-10s\t|%-10s\t|%-10s\t|%-10s\n", "文件系统", "总空间", "已使用", "未使用", "空间比", "Inode数", "空闲Inode");
	printf("%-10s\t|%-10d\t|%-10d\t|%-10d\t|%-10.2f\t|%-10d\t|%-10d\n", DISK_NAME, size, used, unused, use, inode, iunused);
}

// 格式化文件系统
void UserInterface::format() {
	tm stdT;
	time_t time1 = unsigned int(time(NULL));
	localtime_s(&stdT, (const time_t*)&time1);
	printf_s("-NOTICE: 正在格式化文件系统, 开始时间: %02d:%02d:%02d(不应超过2分钟).\n", stdT.tm_hour, stdT.tm_min, stdT.tm_sec);

	logout();
	fs.fformat();

	time_t time2 = unsigned int(time(NULL));
	localtime_s(&stdT, (const time_t*)&time2);
	printf_s("-NOTICE: 文件系统格式化成功, 结束时间: %02d:%02d:%02d.\n\n", stdT.tm_hour, stdT.tm_min, stdT.tm_sec);
}

// 显示当前文件夹列表
void UserInterface::ls() {
	vector<string>list = fs.flist(cur_dir.data(), cur_uid, cur_gid);
	for (unsigned int i = 0; i < list.size(); i++)
		cout << list[i];
}

// 显示当前路径
void UserInterface::path() {
	printf_s("[root]%s\n",cur_dir.data());
}

// 进入文件夹
void UserInterface::cd(const char* name) {
	string sub_dir = "";
	vector<string> route = splitstr(name, "/");

	for (int i = 0; i < route.size(); i++) {
		sub_dir = route[i];
		string dir = cur_dir + "/" + sub_dir;

		if (fs.enter(dir.data(), cur_uid, cur_gid)) {
			if (sub_dir == ".") {}
			else if (sub_dir == "..") {
				size_t pos = cur_dir.find_last_of('/');
				cur_dir = cur_dir.substr(0, pos);
			}
			else
				cur_dir = dir;
		}
		else
			throw("-!ERROR: 无法进入指定目录.\n");
	}
}

// 新建文件夹
void UserInterface::mkdir(const char* name) {
	string new_dir = string(name);
	if (new_dir.find("/") != string::npos)
		throw("-!ERROR: 参数中不能包含多级目录.\n");

	string dir = cur_dir + "/" + new_dir;
	fs.dcreate(dir.data(), cur_uid, cur_gid);
}

// 删除文件夹
void UserInterface::rmdir(const char* name) {
	string new_dir = string(name);
	if (new_dir.find("/") != string::npos)
		throw("-!ERROR: 参数中不能包含多级目录.\n");

	string dir = cur_dir + "/" + new_dir;
	fs.ddelete(dir.data(), cur_uid, cur_gid);
}

// 新建文件
void UserInterface::touch(const char* name) {
	string new_dir = string(name);
	if (new_dir.find("/") != string::npos)
		throw("-!ERROR: 参数中不能包含多级目录.\n");

	string dir = cur_dir + "/" + new_dir;
	fs.fcreate(dir.data(), cur_uid, cur_gid);
}

// 删除文件
void UserInterface::remove(const char* name) {
	string new_dir = string(name);
	if (new_dir.find("/") != string::npos)
		throw("-!ERROR: 参数中不能包含多级目录.\n");

	string dir = cur_dir + "/" + new_dir;
	fs.fdelete(dir.data(), cur_uid, cur_gid);
}

// 修改文件(夹)权限
void UserInterface::chmod(const char* name, int mode) {
	string new_dir = string(name);
	if (new_dir.find("/") != string::npos)
		throw("-!ERROR: 参数中不能包含多级目录.\n");

	string dir = cur_dir + "/" + new_dir;
	fs.chmod(dir.data(), mode, cur_uid, cur_gid);
}

// 登录
bool UserInterface::login(string name, string passwd) {
	//从文件读取用户数据
	fs.chmod("/etc/users", 0400, 0, 0);	//文件主可读
	File* user = fs.fopen("/etc/users");
	char* udata = new char[user->f_inode->i_size + 1]{ 0 };
	fs.fread(udata, user->f_inode->i_size, user);
	string ustrdata = udata;
	delete[] udata;
	fs.fclose(user);
	fs.chmod("/etc/users", 0000, 0, 0);	//不可读不可写

	//分割读取的user数据
	vector<string> users;
	users = splitstr(ustrdata, "\n");
	for (unsigned int i = 0; i < users.size(); i++) {
		vector<string>umsg = splitstr(users[i], "-");

		//找到用户信息
		if (umsg[0] == name && umsg[1] == passwd) {
			is_login = true;
			cur_uid = stoi(umsg[2]);
			cur_user = name;
			cur_gid = stoi(umsg[3]);

			//读取用户组信息
			fs.chmod("/etc/groups", 0400, 0, 0);	//文件主可读
			File* group = fs.fopen("/etc/groups");
			char* gdata = new char[group->f_inode->i_size + 1]{ 0 };
			fs.fread(gdata, group->f_inode->i_size, group);
			string gstrdata = gdata;
			delete[] gdata;
			fs.fclose(group);
			fs.chmod("/etc/groups", 0000, 0, 0);	//不可读不可写

			vector<string> groups;
			groups = splitstr(gstrdata, "\n");
			for (unsigned int j = 0; j < groups.size(); j++) {
				vector<string>gmsg = splitstr(groups[j], "-");
				if (gmsg[1] == umsg[3]) {
					cur_group = gmsg[0].data();
					break;
				}
			}

			cur_dir = "";
			return 1;
		}
	}

	return 0;
}

// 登出
void UserInterface::logout() {
	is_login = false;
	cur_uid = -1;
	cur_user = "";
	cur_gid = -1;
	cur_group = "";
	cur_dir = "";
}

// 添加用户
void UserInterface::addu(string uname, string gname, string passwd) {
	//只有admin可以添加用户
	if (cur_uid != 0)
		throw("-!ERROR: 权限不足, 只有管理员可以添加用户.\n");

	//临时开放user文件
	fs.chmod("/etc/users", 0600);	//临时开放权限
	File* user = fs.fopen("/etc/users");
	char* data = new char[user->f_inode->i_size + 1]{ 0 };
	fs.fread(data, user->f_inode->i_size, user);

	//判断用户是否存在，并向user文件添加
	string ustrdata = data;
	delete[] data;
	vector<string> users;
	users = splitstr(ustrdata, "\n");
	int next_uid = 0;
	for (unsigned int i = 0; i < users.size(); i++) {
		vector<string>umsg = splitstr(users[i], "-");
		if (umsg[0] == uname) {
			fs.fclose(user);
			fs.chmod("/etc/users", 0000);	//收回权限
			throw("-!ERROR: 该用户已存在.\n");
		}
		if (stoi(umsg[2]) >= next_uid)
			next_uid = stoi(umsg[2]) + 1;
	}

	//判断用户组是否存在，存在：向groups文件添加uid
	int gid = -1;
	fs.chmod("/etc/groups", 0600);	//临时开放权限
	File* group = fs.fopen("/etc/groups");
	char* gdata = new char[group->f_inode->i_size + 1]{ 0 };
	fs.fread(gdata, group->f_inode->i_size, group);
	string gstrdata = gdata;
	delete[] gdata;
	vector<string> groups;
	groups = splitstr(gstrdata, "\n");
	for (unsigned int i = 0; i < groups.size(); i++) {
		vector<string>gmsg = splitstr(groups[i], "-");
		if (gmsg[0] == gname) {
			gid = stoi(gmsg[1]);
			groups[i] += (gmsg.size() == 2 ? to_string(next_uid) : "," + to_string(next_uid));
			break;
		}
	}

	//判断用户组是否存在，存在：向users文件添加group
	if (gid != -1){
		string new_gdata;
		for (unsigned int i = 0; i < groups.size(); i++)
			new_gdata += groups[i] + "\n";

		fs.fseek(group, 0, SEEK_SET);
		fs.fwrite(new_gdata.data(), new_gdata.length(), group);
		ustrdata += uname + "-" + passwd + "-" + to_string(next_uid) + "-" + to_string(gid) + "\n";
		fs.fseek(user, 0, SEEK_SET);
		fs.fwrite(ustrdata.data(), ustrdata.length(), user);
	}
	else {
		fs.fclose(user);
		fs.fclose(group);
		fs.chmod("/etc/users", 0000);	//收回权限
		fs.chmod("/etc/groups", 0000);	//收回权限
		throw("-!ERROR: 用户组不存在.\n");
	}

	fs.fclose(user);
	fs.fclose(group);
	fs.chmod("/etc/users", 0000);	//收回权限
	fs.chmod("/etc/groups", 0000);	//收回权限
}

// 删除用户
void UserInterface::delu(string uname) {
	if (cur_uid != 0)
		throw("-!ERROR: 权限不足, 只有管理员可以删除用户.\n");

	if (uname == ADMIN)
		throw("-!ERROR: 不能删除管理员用户.\n");

	fs.chmod("/etc/users", 0600);	//临时开放权限
	File* user = fs.fopen("/etc/users");
	char* data = new char[user->f_inode->i_size + 1]{ 0 };
	fs.fread(data, user->f_inode->i_size, user);
	string ustrdata = data;
	delete[] data;

	vector<string> users;
	users = splitstr(ustrdata, "\n");
	string new_udata;
	int uid = -1;
	int gid = -1;
	for (unsigned int i = 0; i < users.size(); i++) {
		vector<string>umsg = splitstr(users[i], "-");
		if (umsg[0] == uname) {
			uid = stoi(umsg[2]);
			gid = stoi(umsg[3]);
		}
		else
			new_udata += users[i] + "\n";
	}

	if (uid == -1) {
		fs.fclose(user);
		fs.chmod("/etc/users", 0000);	//收回权限
		throw("-!ERROR: 该用户不存在.\n");
	}
	fs.fseek(user, 0, SEEK_SET);
	fs.freplace(new_udata.data(), new_udata.length(), user);
	fs.fclose(user);
	fs.chmod("/etc/users", 0000);	//收回权限


	fs.chmod("/etc/groups", 0600);	//临时开放权限
	File* group = fs.fopen("/etc/groups");
	char* gdata = new char[group->f_inode->i_size + 1]{ 0 };
	fs.fread(gdata, group->f_inode->i_size, group);
	string gstrdata = gdata;
	delete[] gdata;

	vector<string> groups;
	groups = splitstr(gstrdata, "\n");
	string new_gdata;
	for (unsigned int i = 0; i < groups.size(); i++) {
		vector<string>gmsg = splitstr(groups[i], "-");
		if (stoi(gmsg[1]) == gid) {
			vector<string>uids = splitstr(gmsg[2], ",");
			string newuids;
			for (unsigned int j = 0; j < uids.size(); j++) {
				if (stoi(uids[j]) != uid)
					newuids += (newuids.length() == 0 ? uids[j] : "," + uids[j]);
			}
			new_gdata += gmsg[0] + "-" + gmsg[1] + "-" + newuids + "\n";
		}
		else
			new_gdata += groups[i] + "\n";
	}

	fs.fseek(group, 0, SEEK_SET);
	fs.freplace(new_gdata.data(), new_gdata.length(), group);
	fs.fclose(group);
	fs.chmod("/etc/groups", 0000);	//收回权限
}

// 添加用户组
void UserInterface::addg(string gname) {
	if (cur_uid != 0)
		throw("-!ERROR: 权限不足, 只有管理员可以添加用户组.\n");

	fs.chmod("/etc/groups", 0600);	//临时开放权限
	File* group = fs.fopen("/etc/groups");
	char* gdata = new char[group->f_inode->i_size + 1]{ 0 };
	fs.fread(gdata, group->f_inode->i_size, group);
	string gstrdata = gdata;
	delete[] gdata;

	vector<string> groups;
	groups = splitstr(gstrdata, "\n");
	int next_gid = 0;
	for (unsigned int i = 0; i < groups.size(); i++){
		vector<string>gmsg = splitstr(groups[i], "-");
		if (gmsg[0] == gname){
			fs.fclose(group);
			fs.chmod("/etc/groups", 0000);	//收回权限
			throw("-!ERROR: 该用户组已存在.\n");
		}
		if (stoi(gmsg[1]) >= next_gid)
			next_gid = stoi(gmsg[1]) + 1;
	}

	gstrdata += gname + "-" + to_string(next_gid) + "-\n";
	fs.fseek(group, 0, SEEK_SET);
	fs.fwrite(gstrdata.data(), gstrdata.length(), group);
	fs.fclose(group);
	fs.chmod("/etc/groups", 0000);	//收回权限
}

// 删除用户组
void UserInterface::delg(string gname) {
	if (cur_uid != 0)
		throw("-!ERROR: 权限不足, 只有管理员可以删除用户组.\n");

	if (gname == ROOT_GROUP)
		throw("-!ERROR: 不能删除root用户组.\n");

	fs.chmod("/etc/groups", 0600);	//临时开放权限
	File* group = fs.fopen("/etc/groups");
	char* gdata = new char[group->f_inode->i_size + 1]{ 0 };
	fs.fread(gdata, group->f_inode->i_size, group);
	string gstrdata = gdata;
	delete[] gdata;

	vector<string> groups;
	groups = splitstr(gstrdata, "\n");
	string new_gdata;
	vector<string>uids;
	bool flag = false;
	for (unsigned int i = 0; i < groups.size(); i++) {
		vector<string>gmsg = splitstr(groups[i], "-");
		if (gmsg[0] == gname) {
			flag = true;
			uids = splitstr(gmsg[2], ",");
		}
		else
			new_gdata += groups[i] + "\n";
	}

	if (!flag) {
		fs.fclose(group);
		fs.chmod("/etc/groups", 0000);	//收回权限
		throw("-!ERROR: 该用户组不存在.\n");
	}

	fs.fseek(group, 0, SEEK_SET);
	fs.freplace(new_gdata.data(), new_gdata.length(), group);
	fs.fclose(group);
	fs.chmod("/etc/groups", 0000);	//收回权限


	fs.chmod("/etc/users", 0600);	//临时开放权限
	File* user = fs.fopen("/etc/users");
	char* data = new char[user->f_inode->i_size + 1]{ 0 };
	fs.fread(data, user->f_inode->i_size, user);
	string ustrdata = data;
	delete[] data;

	vector<string> users;
	users = splitstr(ustrdata, "\n");
	string new_udata;
	for (unsigned int i = 0; i < users.size(); i++) {
		vector<string>umsg = splitstr(users[i], "-");
		if (find(uids.begin(), uids.end(), umsg[2]) == uids.end())
			new_udata += users[i] + "\n";
	}
	fs.fseek(user, 0, SEEK_SET);
	fs.freplace(new_udata.data(), new_udata.length(), user);
	fs.fclose(user);
	fs.chmod("/etc/users", 0000);	//收回权限
}

// 显示文件
void UserInterface::show(const char* name) {
	string fname = string(name);
	if (fname.find("/") != string::npos)
		throw("-!ERROR: 参数中不能包含多级目录.\n");

	string dir = cur_dir + "/" + fname;
	File* fp = fs.fopen(dir.data(), cur_uid, cur_gid);
	char* fdata = new char[fp->f_inode->i_size + 1]{ 0 };
	fs.fread(fdata, fp->f_inode->i_size, fp);
	cout << fdata << endl;

	fs.fclose(fp);
	delete[] fdata;
}

// fs中文件复制
void UserInterface::cp(const char* ori_fname, const char* tar_fname) {
	//读取源文件，存入data
	string sub_dir = "";
	string fs_dir = cur_dir;
	vector<string> route = splitstr(ori_fname, "/");

	for (int i = 0; i < route.size() - 1; i++) {
		sub_dir = route[i];
		string dir = fs_dir + "/" + sub_dir;

		if (fs.enter(dir.data(), cur_uid, cur_gid)) {
			if (sub_dir == ".") {}
			else if (sub_dir == "..") {
				size_t pos = fs_dir.find_last_of('/');
				fs_dir = fs_dir.substr(0, pos);
			}
			else
				fs_dir = dir;
		}
		else
			throw("-!ERROR: 无法进入指定目录.\n");
	}
	fs_dir = fs_dir + "/" + route[route.size() - 1];

	File* fsfp = fs.fopen(fs_dir.data(), cur_uid, cur_gid);
	unsigned file_size = fsfp->f_inode->i_size;
	char* data = new char[file_size + 1]{ 0 };
	fs.fread(data, file_size, fsfp);
	fs.fclose(fsfp);

	//写入目的文件
	string sub_dir2 = "";
	string fs_dir2 = cur_dir;
	vector<string> route2 = splitstr(tar_fname, "/");

	for (int i = 0; i < route2.size() - 1; i++) {
		sub_dir2 = route2[i];
		string dir2 = fs_dir2 + "/" + sub_dir2;

		if (fs.enter(dir2.data(), cur_uid, cur_gid)) {
			if (sub_dir2 == ".") {}
			else if (sub_dir2 == "..") {
				size_t pos = fs_dir2.find_last_of('/');
				fs_dir2 = fs_dir2.substr(0, pos);
			}
			else
				fs_dir2 = dir2;
		}
		else
			throw("-!ERROR: 无法进入指定目录.\n");
	}

	fs_dir2 = fs_dir2 + "/" + route2[route2.size() - 1];

	File* fsfp2 = fs.fopen(fs_dir2.data(), cur_uid, cur_gid);
	fs.freplace(data, file_size, fsfp2);
	fs.fclose(fsfp2);

	delete[] data;
}

// 从win复制文件到fs
void UserInterface::win2fs(const char* win_fname, const char* fs_fname) {
	string sub_dir = "";
	string fs_dir = cur_dir;
	vector<string> route = splitstr(fs_fname, "/");

	for (int i = 0; i < route.size() - 1; i++) {
		sub_dir = route[i];
		string dir = fs_dir + "/" + sub_dir;

		if (fs.enter(dir.data(), cur_uid, cur_gid)) {
			if (sub_dir == ".") {}
			else if (sub_dir == "..") {
				size_t pos = fs_dir.find_last_of('/');
				fs_dir = fs_dir.substr(0, pos);
			}
			else
				fs_dir = dir;
		}
		else
			throw("-!ERROR: 无法进入指定目录.\n");
	}

	fs_dir = fs_dir + "/" + route[route.size() - 1];

	FILE* winfp;
	fopen_s(&winfp, win_fname, "rb");
	if (winfp == NULL)
		throw("-!ERROR: 打开windows文件失败.\n");

	fseek(winfp, 0, SEEK_END);
	unsigned int file_size = ftell(winfp);

	fseek(winfp, 0, SEEK_SET);
	char* data = new char[file_size + 1]{ 0 };
	fread(data, file_size, 1, winfp);
	fclose(winfp);

	File* fsfp = fs.fopen(fs_dir.data(), cur_uid, cur_gid);
	fs.freplace(data, file_size, fsfp);
	fs.fclose(fsfp);
	
	delete[] data;
}

// 从fs复制文件到win
void UserInterface::fs2win(const char* fs_fname, const char* win_fname) {
	string sub_dir = "";
	string fs_dir = cur_dir;
	vector<string> route = splitstr(fs_fname, "/");

	for (int i = 0; i < route.size() - 1; i++) {
		sub_dir = route[i];
		string dir = fs_dir + "/" + sub_dir;

		if (fs.enter(dir.data(), cur_uid, cur_gid)) {
			if (sub_dir == ".") {}
			else if (sub_dir == "..") {
				size_t pos = fs_dir.find_last_of('/');
				fs_dir = fs_dir.substr(0, pos);
			}
			else
				fs_dir = dir;
		}
		else
			throw("-!ERROR: 无法进入指定目录.\n");
	}
	fs_dir = fs_dir + "/" + route[route.size() - 1];

	File* fsfp = fs.fopen(fs_dir.data(), cur_uid, cur_gid);
	unsigned file_size = fsfp->f_inode->i_size;
	char* data = new char[file_size + 1]{ 0 };
	fs.fread(data, file_size, fsfp);
	fs.fclose(fsfp);

	FILE* winfp;
	fopen_s(&winfp, win_fname, "wb");
	if (winfp == NULL)
		throw("-!ERROR: 打开windows文件失败.\n");

	fwrite(data, file_size, 1, winfp);
	fclose(winfp);

	delete[] data;
}

/*################################################################*/
// 测试指令集运行函数
void UserInterface::TestRun() {
	cout << "\n-NOTICE: 当前文件系统使用test指令集, 输入 fhelp 获取帮助." << endl;

	login(ADMIN, ADMIN_PSW);

	char* abc = new char[800]{ 0 };
	char init[1000];
	for (int i = 0; i < 1000; i++) {
		if (i < 500)
			init[i] = 'a' + i % 26;
		else
			init[i] = 'A' + i % 26;
	}

	while (true) {
		//printf("\033[40;32m%s@%s:\033[0m", cur_user.data(), local_host.data());	//(绿色)cur_user@LOCAL_HOST:
		//printf("\033[40;33m%s%s\033[0m", ROOT_DIR_SYMBOL, cur_dir.data());			//(蓝色)root/cur_dir
		printf("%s@%s:", cur_user.data(), local_host.data());	//(绿色)cur_user@LOCAL_HOST:
		printf("%s%s", ROOT_DIR_SYMBOL, cur_dir.data());		//(蓝色)root/cur_dir
		cout << (cur_user == ADMIN ? "#" : "$") << " ";

		string buf;				//字符缓存
		vector<string> args;	//参数
		getline(cin, buf);
		args = splitstr(buf, " ");

		try {
			//没有输入
			if (args.size() <= 0)
				continue;
			
			//所有测试指令
			else {
				if (args[0] == "exit") {
					return;
				}
				else if (args[0] == "ftest") {
					_ftest();
				}
				else if (args[0] == "fhelp") {
					_fhelp();
				}
				else if (args[0] == "fformat") {
					cout << "-确定对文件系统格式化? [y]: ";
					string ck;
					getline(cin, ck);
					if (ck == "y") {
						memset(&abc, 0, sizeof(800));
						_fformat();
					}
				}
				else if (args[0] == "flist") {
					_flist();
				}
				else if (args[0] == "cd" && args.size() == 2) {
					cd(args[1].data());
				}
				else if (args[0] == "fsize" && args.size() == 2) {
					int fd = stoi(args[1]);
					int result = _fsize(fd);
					cout << "-NOTICE: 返回值为" << result << endl;
				}
				else if (args[0] == "flseek" && args.size() == 4) {
					int fd = stoi(args[1]);
					int offset = stoi(args[2]);
					int ptrname = stoi(args[3]);
					if (ptrname == 0 && offset >= 0) {
						int result = _flseek(fd, offset, ptrname);
						cout << "-NOTICE: 返回值为" << result << endl;
					}
					else if (ptrname > 0 && ptrname <= 2) {
						int result = _flseek(fd, offset, ptrname);
						cout << "-NOTICE: 返回值为" << result << endl;
					}
					else {
						cout << "-!ERROR: 输入无效.\n";
					}
				}
				else if (args[0] == "fclose" && args.size() == 2) {
					int fd = stoi(args[1]);
					_fclose(fd);
				}
				else if (args[0] == "fcseek" && args.size() == 2) {
					int fd = stoi(args[1]);
					int result = _fcseek(fd);
					cout << "-NOTICE: 返回值为" << result << endl;
				}
				else if (args[0] == "fcreat" && args.size() == 3 && args[2].size() == 3) {
					int master = stoi(args[2].substr(0, 1));
					int group = stoi(args[2].substr(1, 1));
					int others = stoi(args[2].substr(2, 1));
					if (master >= 0 && master <= 7 && group >= 0 && group <= 7 && others >= 0 && others <= 7) {
						int mode = (master << 6) + (group << 3) + others;
						int result = _fcreat(args[1].data(), mode);
						cout << "-NOTICE: 返回值为" << result << endl;
					}
					else {
						cout << "-!ERROR: 输入权限无效.\n";
					}
				}
				else if (args[0] == "fopen" && args.size() == 3 && args[2].size() == 1) {
					int master = stoi(args[2].substr(0, 1));
					if (master >= 0 && master <= 7) {
						int mode = (master << 6);
						int result = _fopen(args[1].data(), mode);
						cout << "-NOTICE: 返回值为" << result << endl;
					}
					else {
						cout << "-!ERROR: 输入权限无效.\n";
					}
				}
				else if (args[0] == "fdelete" && args.size() == 2) {
					_fdelete(args[1].data());
				}
				else if (args[0] == "dcreat" && args.size() == 3 && args[2].size() == 3) {
					int master = stoi(args[2].substr(0, 1));
					int group = stoi(args[2].substr(1, 1));
					int others = stoi(args[2].substr(2, 1));
					if (master >= 0 && master <= 7 && group >= 0 && group <= 7 && others >= 0 && others <= 7) {
						int mode = (master << 6) + (group << 3) + others;
						_dcreat(args[1].data(), mode);
					}
					else {
						cout << "-!ERROR: 输入权限无效.\n";
					}
				}
				else if (args[0] == "fdelete" && args.size() == 2) {
					_ddelete(args[1].data());
				}
				else if (args[0] == "fread" && args.size() == 4) {
					int fd = stoi(args[1]);
					int size = stoi(args[3]);
					int result = _fread(fd, abc, size);
					cout << "-NOTICE: 返回值为" << result << endl;
				}
				else if (args[0] == "fwrite" && args.size() == 4) {
					int fd = stoi(args[1]);
					int size = stoi(args[3]);
					if (args[2] == "init") {
						int result = _fwrite(fd, init, size);
						cout << "-NOTICE: 返回值为" << result << endl;
					}
					else {
						int result = _fwrite(fd, abc, size);
						cout << "-NOTICE: 返回值为" << result << endl;
					}
				}
				else if (args[0] == "fshow" && args.size() == 2) {
					show(args[1].data());
				}
				else{
					cout << "-!ERROR: 输入的指令暂不支持.\n";
				}
			}
		}
		catch (const char* errMsg) {
			cout << errMsg;
		}
	}
}

// 功能指令集运行函数
void UserInterface::BetaRun() {
	cout << "\n-NOTICE: 当前文件系统使用beta指令集, 输入 help 获取帮助." << endl;

	while (true) {
		//printf("\033[40;32m%s@%s:\033[0m", cur_user.data(), local_host.data());	//(绿色)cur_user@LOCAL_HOST:
		//printf("\033[40;33m%s%s\033[0m", ROOT_DIR_SYMBOL, cur_dir.data());			//(蓝色)root/cur_dir
		printf("%s@%s:", cur_user.data(), local_host.data());	//(绿色)cur_user@LOCAL_HOST:
		printf("%s%s", ROOT_DIR_SYMBOL, cur_dir.data());		//(蓝色)root/cur_dir
		cout << (cur_user == ADMIN ? "#" : "$") << " ";

		string buf;				//字符缓存
		vector<string> args;	//参数
		getline(cin, buf);
		args = splitstr(buf, " ");

		try {
			//没有输入
			if (args.size() <= 0)
				continue;

			//需要登录权限
			else if (is_login) {
				if (args[0] == "logout") {
					logout();
				}
				else if (args[0] == "exit") {
					return;
				}
				else if (args[0] == "format") {
					cout << "-确定对文件系统格式化? [y]: ";
					string ck;
					getline(cin, ck);
					if (ck == "y") {
						format();
					}
				}
				else if (args[0] == "cls") {
					cls();
				}
				else if (args[0] == "help") {
					help();
				}
				else if (args[0] == "path") {
					path();
				}
				else if (args[0] == "mkdir" && args.size() == 2) {
					mkdir(args[1].data());
				}
				else if (args[0] == "cd" && args.size() == 2) {
					cd(args[1].data());
				}
				else if (args[0] == "ls") {
					ls();
				}
				else if (args[0] == "rmdir" && args.size() == 2) {
					rmdir(args[1].data());
				}
				else if (args[0] == "touch" && args.size() == 2) {
					touch(args[1].data());
				}
				else if (args[0] == "chmod" && args.size() == 3 && args[2].size() == 3) {
					int master = stoi(args[2].substr(0, 1));
					int group = stoi(args[2].substr(1, 1));
					int others = stoi(args[2].substr(2, 1));
					if (master >= 0 && master <= 7 && group >= 0 && group <= 7 && others >= 0 && others <= 7) {
						int mode = (master << 6) + (group << 3) + others;
						chmod(args[1].data(), mode);
					}
					else {
						cout << "-!ERROR: 输入权限无效.\n";
					}
				}
				else if (args[0] == "remove" && args.size() == 2) {
					remove(args[1].data());
				}
				else if (args[0] == "addu" && args.size() == 3) {
					cout << "password: ";
					char password[256] = { 0 };
					char ch = '\0';
					int pos = 0;
					while ((ch = _getch()) != '\r' && pos < 255) {
						if (ch != 8) {	//不是回撤就录入
							password[pos] = ch;
							putchar('*');	//并且输出*号
							pos++;
						}
						else {
							putchar('\b');	//这里是删除一个，我们通过输出回撤符 /b，回撤一格，
							putchar(' ');	//再显示空格符把刚才的*给盖住，
							putchar('\b');	//然后再回撤一格等待录入。
							pos--;
						}
					}
					password[pos] = '\0';
					putchar('\n');
					addu(args[1], args[2], string(password));
				}
				else if (args[0] == "delu" && args.size() == 2) {
					delu(args[1]);
				}
				else if (args[0] == "addg" && args.size() == 2) {
					addg(args[1]);
				}
				else if (args[0] == "delg" && args.size() == 2) {
					delg(args[1]);
				}
				else if (args[0] == "disk") {
					disk();
				}
				else if (args[0] == "show" && args.size() == 2) {
					show(args[1].data());
				}
				else if (args[0] == "cp" && args.size() == 3) {
					cp(args[1].data(), args[2].data());
				}
				else if (args[0] == "win2fs" && args.size() == 3) {
					win2fs(args[1].data(), args[2].data());
				}
				else if (args[0] == "fs2win" && args.size() == 3) {
					fs2win(args[1].data(), args[2].data());
				}
				else {
					cout << "-!ERROR: 输入的指令暂不支持.\n";
				}
			}

			//不需要登录权限(login,cls,exit,format,help)
			else {
				if (args[0] == "login" && args.size() == 2) {
					cout << "password: ";
					char password[256] = { 0 };
					char ch = '\0';
					int pos = 0;
					while ((ch = _getch()) != '\r' && pos < 255) {
						if (ch != 8) {//不是回撤就录入
							password[pos] = ch;
							putchar('*');//并且输出*号
							pos++;
						}
						else {
							putchar('\b');//这里是删除一个，我们通过输出回撤符 /b，回撤一格，
							putchar(' ');//再显示空格符把刚才的*给盖住，
							putchar('\b');//然后再?回撤一格等待录入。
							pos--;
						}
					}
					password[pos] = '\0';
					putchar('\n');
					if (!login(args[1], string(password))) {
						cout << "-!ERROR: 登录失败.\n";
					}
				}
				else if (args[0] == "exit") {
					return;
				}
				else if (args[0] == "format") {
					cout << "-确定对文件系统格式化? [y]: ";
					string ck;
					getline(cin, ck);
					if (ck == "y") {
						format();
					}
				}
				else if (args[0] == "cls") {
					cls();
				}
				else if (args[0] == "help") {
					help();
				}
				else {
					cout << "-!ERROR: 输入的指令暂不支持, 请登录后再尝试.\n";
				}
			}
		}
		catch (const char* errMsg) {
			cout << errMsg;
		}
	}
}

// 主运行函数
void UserInterface::Run() {
	cout << "\n";
	cout << "--------------------------------------------------------------------------------/"	<< endl;
	cout << "| @Project :  FileSystem"															<< endl;
	cout << "| @Author  :  Ray"																	<< endl;
	cout << "| @GitHub  :  https://github.com/RayCorleone/FileSystem"							<< endl;
	cout << "| @Notice  :"																		<< endl;
	cout << "|     1.退出系统务必使用 exit 指令, 否则有一定概率出错;"							<< endl;
	cout << "|     2.系统出错后, 执行 format/fformat 指令可以格式化系统;"						<< endl;
	cout << "|     3.管理员名: admin, 管理员密码: password, 初始用户组: root;"					<< endl;
	cout << "|     4.系统有 test 和 beta 两种指令集, 一旦选择后, 在本次运行过程中不可更改;"		<< endl;
	cout << "|         (1)test指令集: 使用文件句柄操作, 管理员自动登录;"						<< endl;
	cout << "|         (2)beta指令集: 需要自行登录管理员账户, 使用更高级的指令结构;"			<< endl;
	cout << "--------------------------------------------------------------------------------/"	<< endl;
		
	// 选择test/beta指令集
	while (true) {
		cout << "\n-请选择指令集(结束请输入exit)[test/beta]: ";
		string mode;
		getline(cin, mode);

		if (mode == "test") {
			TestRun();
			return;
		}
		else if (mode == "beta") {
			BetaRun();
			return;
		}
		else if (mode == "exit") {
			return;
		}
		else {
			cout << "-!ERROR: 指令集必须是 [test/beta] 中的一个." << endl;
		}
	}

	return;
}
