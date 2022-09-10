/*
	@name:      UserInterface.cpp
	@function:	UserInterface �ඨ��
	@author:    Ray
	@version:   1.0
	@problem:	None
	@timestamp: 2022/05/22
	@notice:	None
*/

#include "Define.h"

using namespace std;

/*################################################################*/
// UserInterface���캯��
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

	// ��ȡ������
	DWORD size = 0;
	wstring wstr;
	GetComputerName(NULL, &size); //�õ��������Ƴ���
	wchar_t* name = new wchar_t[size];
	if (GetComputerName(name, &size))
		wstr = name;
	delete[] name;
	local_host = wstring2string(wstr);
}

// UserInterface��������
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
// ���ɲ���ָ��
void UserInterface::_ftest() {
	char* abc = new char[800]{ 0 };
	char init[1000];
	for (int i = 0; i < 1000; i++) {
		if (i < 500)
			init[i] = 'a' + i % 26;
		else
			init[i] = 'A' + i % 26;
	}

	// ��ʽ��ϵͳ
	_fformat();

	cout << "(1)�½��ļ�/test/Jerry:" << endl;
	_dcreat("test", 0777);
	int fd = _fcreat("test/Jerry", 0600);
	cout << "-�������ļ��ľ��:" << fd << endl;
	cd("test");
	_flist();

	cout << "\n(2)��test/Jerry�ļ�д��800�ֽ�:" << endl;
	cout << "-����" << fd << "д��" << _fwrite(fd, init, 800) << "�ֽ�" << endl;
	cout << "-���" << fd << "�Ĵ�С:" << _fsize(fd) << endl;

	cout << "\n(3)���ļ�ָ�붨λ��500�ֽ�:" << endl;
	cout << "-���" << fd << "�޸�֮ǰ��ָ��:" << _fcseek(fd) << endl;
	cout << "-���" << fd << "�޸�֮���ָ��:" << _flseek(fd, 500, 0) << endl;

	cout << "\n(4)����500�ֽڵ�abc:" << endl;
	cout << "-�Ӿ��" << fd << "�����ֽ���:" << _fread(fd, abc, 500) << endl;
	cout << "-�ַ���abc��ֵ:\n" << abc << endl;

	cout << "\n(5)��abcд���ļ�:" << endl;
	cout << "-����" << fd << "д��" << _fwrite(fd, abc, 500) << "�ֽ�" << endl;
	cout << "-���" << fd << "�Ĵ�С:" << _fsize(fd) << endl;
	_fclose(fd);

	cout << "\n-д���ļ���Jerry�Ĵ�С:\n";
	_flist();
	cout << "\n-д���ļ���Jerry������:\n";
	show("Jerry");

	cout << "\n";
	return;
}

// ��ʾ����ָ�����
void UserInterface::_fhelp() {
	printf("- cls                                     - ���������\n");
	printf("- ftest                                   - һ��ʵ�ֲ���\n");
	printf("- fhelp                                   - ��ʾ�����嵥\n");
	printf("- fformat                                 - ��ʽ���ļ�ϵͳ\n");
	printf("- flist                                   - ��ʾ��ǰ�ļ����б�\n");
	printf("- cd      <dir name>                      - �����ļ���\n");
	printf("- dcreat  <dir name> <mode(OTC)>          - �½��ļ���\n");
	printf("- ddelete <dir name>                      - ɾ���ļ���\n");
	printf("- fcreat  <file name> <mode(OTC)>         - �½��ļ�(�����ļ����)\n");
	printf("- fdelete <file name>                     - ɾ���ļ�\n");
	printf("- fshow   <file name>                     - ��ʾ�ļ�\n");
	printf("- fopen   <file name> <mode(OTC)>         - ���ļ�(�����ļ����)\n");
	printf("- fclose  <��� fd>                       - �ر��ļ�\n");
	printf("- fread   <��� fd> <buf abc> <size>      - ��ȡ�ļ����ݵ�abc\n");
	printf("- fwrite  <��� fd> <init> <size>         - ��ʼ���ļ�����\n");
	printf("- fwrite  <��� fd> <buf abc> <size>      - ��abc������д���ļ�\n");
	printf("- flseek  <��� fd> <offset> <ptrname>    - �޸��ļ�ָ��\n");
	printf("- fcseek  <��� fd>                       - �����ļ�ָ��\n");
	printf("- fsize   <��� fd>                       - ����fd�ļ���С\n");
	printf("- exit                                    - �˳�ϵͳ\n");
}

// ��ʽ���ļ�ϵͳ
void UserInterface::_fformat() {
	tm stdT;
	time_t time1 = unsigned int(time(NULL));
	localtime_s(&stdT, (const time_t*)&time1);
	printf_s("-NOTICE: ���ڸ�ʽ���ļ�ϵͳ, ��ʼʱ��: %02d:%02d:%02d(��Ӧ����2����).\n", stdT.tm_hour, stdT.tm_min, stdT.tm_sec);

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
	printf_s("-NOTICE: �ļ�ϵͳ��ʽ���ɹ�, ����ʱ��: %02d:%02d:%02d.\n\n", stdT.tm_hour, stdT.tm_min, stdT.tm_sec);
}

// �г��ļ�Ŀ¼
void UserInterface::_flist() {
	vector<string>list = fs.flist(cur_dir.data(), cur_uid, cur_gid);
	for (unsigned int i = 0; i < list.size(); i++)
		cout << list[i];
}

// �ļ���С(�����ļ���С)
int UserInterface::_fsize(const int fd) {
	if (cur_flist[fd] == NULL)
		return -1;
	else {
		return cur_flist[fd]->f_inode->i_size;
	}
}

// ������Ŀ¼
void UserInterface::_dcreat(const char* name, const int mode) {
	string new_dir = string(name);
	if (new_dir.find("/") != string::npos)
		throw("-!ERROR: �����в��ܰ����༶Ŀ¼.\n");

	string dir = cur_dir + "/" + new_dir;
	fs.dcreate(dir.data(), cur_uid, cur_gid);
	fs.chmod(dir.data(), mode, cur_uid, cur_gid);
}

// ɾ��Ŀ¼
void UserInterface::_ddelete(const char* name) {
	string new_dir = string(name);
	if (new_dir.find("/") != string::npos)
		throw("-!ERROR: �����в��ܰ����༶Ŀ¼.\n");

	string dir = cur_dir + "/" + new_dir;
	fs.ddelete(dir.data(), cur_uid, cur_gid);
}

// �������ļ�(�����ļ���)
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
			throw("-!ERROR: �޷�����ָ��Ŀ¼.\n");
	}
	fs_dir = fs_dir + "/" + route[route.size() - 1];

	string dir = fs_dir;
	fs.fcreate(dir.data(), cur_uid, cur_gid);
	fs.chmod(dir.data(), mode, cur_uid, cur_gid);

	if (cur_file_cnt < FLIST_SIZE) {	//���о��
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

	cout << "-NOTICE: �ļ������ɹ�, ��û�п�����ļ����." << endl;
	return -1;
}

// ɾ���ļ�(�޷���)
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
			throw("-!ERROR: �޷�����ָ��Ŀ¼.\n");
	}
	fs_dir = fs_dir + "/" + route[route.size() - 1];

	string dir = fs_dir;

	fs.fdelete(dir.data(), cur_uid, cur_gid);
	return;
}

// ���ļ�(�����ļ���)
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
			throw("-!ERROR: �޷�����ָ��Ŀ¼.\n");
	}
	fs_dir = fs_dir + "/" + route[route.size() - 1];

	string dir = fs_dir;
	if (cur_file_cnt < FLIST_SIZE) {	//���о��
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

	cout << "-NOTICE: û�п�����ļ����, �ļ���ʧ��." << endl;
	return -1;
}

// �ر��ļ�(�޷���)
void UserInterface::_fclose(const int fd) {
	if (cur_flist[fd] == NULL)
		cout << "-NOTICE: �ļ����������." << endl;
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

// ��ȡ�ļ�(���ض���������)
int UserInterface::_fread(const int fd, char* buf, int size) {
	if (cur_flist[fd] == NULL)
		cout << "-NOTICE: �ļ����������." << endl;
	else if (cur_file_mode[fd] & 4) {
		return fs.fread(buf, size, cur_flist[fd]);
	}
	else
		cout << "-NOTICE: û�ж�Ȩ��." << endl;
	return -1;
}

// д���ļ�(����д��������)
int UserInterface::_fwrite(const int fd, const char* buf, int size) {
	if (cur_flist[fd] == NULL)
		cout << "-NOTICE: �ļ����������." << endl;
	else if (cur_file_mode[fd] & 2) {
		return fs.fwrite(buf, size, cur_flist[fd]);
	}
	else
		cout << "-NOTICE: û��дȨ��." << endl;

	return -1;
}

// ��λ�ļ���дָ��(����λ��)
int UserInterface::_flseek(const int fd, const int offset, const int ptrname) {
	if (cur_flist[fd] == NULL)
		cout << "-NOTICE: �ļ����������." << endl;
	else {
		return fs.fseek(cur_flist[fd], offset, ptrname);
	}
	return -1;
}

// �ļ�ָ�뵱ǰλ��(���ص�ǰλ��)
int UserInterface::_fcseek(const int fd) {
	if (cur_flist[fd] == NULL)
		return -1;
	else {
		return fs.fcseek(cur_flist[fd]);
	}
}

/*################################################################*/
// ��ʾ����
void UserInterface::help() {
	if (!is_login) {
		printf("- help                     - ��ʾ�����嵥   (δ��¼����)\n");
		printf("- cls                      - ���������     (δ��¼����)\n");
		printf("- format                   - ��ʽ���ļ�ϵͳ (δ��¼����)\n");
		printf("- login   <user name>      - �û���¼       (δ��¼����)\n");
		printf("- exit                     - �˳�ϵͳ       (δ��¼����)\n");
		printf("              ......��¼�鿴����ָ��......              \n");
		//printf("\033[40;36mp.s.����Ա��:%s | ����:%s | ��ʼ�û���:%s\n\n\033[0m", ADMIN, ADMIN_PSW, ROOT_GROUP);//(ǳ��)cur_user@LOCAL_HOST:
		printf("p.s.����Ա��:%s | ����:%s | ��ʼ�û���:%s\n\n", ADMIN, ADMIN_PSW, ROOT_GROUP);//(ǳ��)cur_user@LOCAL_HOST:
	}
	else {
		printf("- help                                    - ��ʾ�����嵥\n");
		printf("- cls                                     - ���������\n");
		printf("- disk                                    - ��ʾ����ʹ�����\n");
		printf("- format                                  - ��ʽ���ļ�ϵͳ\n");
		printf("- ls                                      - ��ʾ��ǰ�ļ����б�\n");
		printf("- path                                    - ��ʾ��ǰ·��\n");
		printf("- cd      <dir name>                      - �����ļ���\n");
		printf("- mkdir   <dir name>                      - �½��ļ���\n");
		printf("- rmdir   <dir name>                      - ɾ���ļ���\n");
		printf("- touch   <file name>                     - �½��ļ�\n");
		printf("- remove  <file name>                     - ɾ���ļ�\n");
		printf("- chmod   <file/dir name> <mode(OTC)>     - �޸��ļ�(��)Ȩ��\n");
		printf("- login   <user name>                     - �û���¼\n");
		printf("- logout                                  - �û��˳�\n");
		printf("- addu    <user name> <group name>        - ����û�\n");
		printf("- delu    <user name>                     - ɾ���û�\n");
		printf("- addg    <group name>                    - ����û���\n");
		printf("- delg    <group name>                    - ɾ���û���\n");
		printf("- show    <file name>                     - ��ʾ�ļ�\n");
		printf("- cp      <source name> <target name>     - �ļ�ϵͳ���ļ�����\n");
		printf("- win2fs  <win file name> <fs file name>  - ��Windows�ļ����ݸ��Ƶ�FS�ļ�ϵͳ�ļ�\n");
		printf("- fs2win  <fs file name> <win file name>  - ��FS�ļ�ϵͳ�ļ����ݸ��Ƶ�Windows�ļ�\n");
		printf("- exit                                    - �˳�ϵͳ\n");
	}
}

// ���������
void UserInterface::cls() {
	system("cls");
}

// ��ʾ����ʹ�����
void UserInterface::disk() {
	SuperBlock* superblock = fs.GetSpb();

	int size = superblock->s_bnum * BLOCK_SIZE;
	int used = (superblock->s_bnum - superblock->s_bfnum) * BLOCK_SIZE;
	int unused = superblock->s_bfnum * BLOCK_SIZE;
	double use = 100 * double((double)superblock->s_bnum - (double)superblock->s_bfnum) / double(superblock->s_bnum);
	int inode = superblock->s_inum;
	int iunused = superblock->s_ifnum;

	printf("%-10s\t|%-10s\t|%-10s\t|%-10s\t|%-10s\t|%-10s\t|%-10s\n", "�ļ�ϵͳ", "�ܿռ�", "��ʹ��", "δʹ��", "�ռ��", "Inode��", "����Inode");
	printf("%-10s\t|%-10d\t|%-10d\t|%-10d\t|%-10.2f\t|%-10d\t|%-10d\n", DISK_NAME, size, used, unused, use, inode, iunused);
}

// ��ʽ���ļ�ϵͳ
void UserInterface::format() {
	tm stdT;
	time_t time1 = unsigned int(time(NULL));
	localtime_s(&stdT, (const time_t*)&time1);
	printf_s("-NOTICE: ���ڸ�ʽ���ļ�ϵͳ, ��ʼʱ��: %02d:%02d:%02d(��Ӧ����2����).\n", stdT.tm_hour, stdT.tm_min, stdT.tm_sec);

	logout();
	fs.fformat();

	time_t time2 = unsigned int(time(NULL));
	localtime_s(&stdT, (const time_t*)&time2);
	printf_s("-NOTICE: �ļ�ϵͳ��ʽ���ɹ�, ����ʱ��: %02d:%02d:%02d.\n\n", stdT.tm_hour, stdT.tm_min, stdT.tm_sec);
}

// ��ʾ��ǰ�ļ����б�
void UserInterface::ls() {
	vector<string>list = fs.flist(cur_dir.data(), cur_uid, cur_gid);
	for (unsigned int i = 0; i < list.size(); i++)
		cout << list[i];
}

// ��ʾ��ǰ·��
void UserInterface::path() {
	printf_s("[root]%s\n",cur_dir.data());
}

// �����ļ���
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
			throw("-!ERROR: �޷�����ָ��Ŀ¼.\n");
	}
}

// �½��ļ���
void UserInterface::mkdir(const char* name) {
	string new_dir = string(name);
	if (new_dir.find("/") != string::npos)
		throw("-!ERROR: �����в��ܰ����༶Ŀ¼.\n");

	string dir = cur_dir + "/" + new_dir;
	fs.dcreate(dir.data(), cur_uid, cur_gid);
}

// ɾ���ļ���
void UserInterface::rmdir(const char* name) {
	string new_dir = string(name);
	if (new_dir.find("/") != string::npos)
		throw("-!ERROR: �����в��ܰ����༶Ŀ¼.\n");

	string dir = cur_dir + "/" + new_dir;
	fs.ddelete(dir.data(), cur_uid, cur_gid);
}

// �½��ļ�
void UserInterface::touch(const char* name) {
	string new_dir = string(name);
	if (new_dir.find("/") != string::npos)
		throw("-!ERROR: �����в��ܰ����༶Ŀ¼.\n");

	string dir = cur_dir + "/" + new_dir;
	fs.fcreate(dir.data(), cur_uid, cur_gid);
}

// ɾ���ļ�
void UserInterface::remove(const char* name) {
	string new_dir = string(name);
	if (new_dir.find("/") != string::npos)
		throw("-!ERROR: �����в��ܰ����༶Ŀ¼.\n");

	string dir = cur_dir + "/" + new_dir;
	fs.fdelete(dir.data(), cur_uid, cur_gid);
}

// �޸��ļ�(��)Ȩ��
void UserInterface::chmod(const char* name, int mode) {
	string new_dir = string(name);
	if (new_dir.find("/") != string::npos)
		throw("-!ERROR: �����в��ܰ����༶Ŀ¼.\n");

	string dir = cur_dir + "/" + new_dir;
	fs.chmod(dir.data(), mode, cur_uid, cur_gid);
}

// ��¼
bool UserInterface::login(string name, string passwd) {
	//���ļ���ȡ�û�����
	fs.chmod("/etc/users", 0400, 0, 0);	//�ļ����ɶ�
	File* user = fs.fopen("/etc/users");
	char* udata = new char[user->f_inode->i_size + 1]{ 0 };
	fs.fread(udata, user->f_inode->i_size, user);
	string ustrdata = udata;
	delete[] udata;
	fs.fclose(user);
	fs.chmod("/etc/users", 0000, 0, 0);	//���ɶ�����д

	//�ָ��ȡ��user����
	vector<string> users;
	users = splitstr(ustrdata, "\n");
	for (unsigned int i = 0; i < users.size(); i++) {
		vector<string>umsg = splitstr(users[i], "-");

		//�ҵ��û���Ϣ
		if (umsg[0] == name && umsg[1] == passwd) {
			is_login = true;
			cur_uid = stoi(umsg[2]);
			cur_user = name;
			cur_gid = stoi(umsg[3]);

			//��ȡ�û�����Ϣ
			fs.chmod("/etc/groups", 0400, 0, 0);	//�ļ����ɶ�
			File* group = fs.fopen("/etc/groups");
			char* gdata = new char[group->f_inode->i_size + 1]{ 0 };
			fs.fread(gdata, group->f_inode->i_size, group);
			string gstrdata = gdata;
			delete[] gdata;
			fs.fclose(group);
			fs.chmod("/etc/groups", 0000, 0, 0);	//���ɶ�����д

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

// �ǳ�
void UserInterface::logout() {
	is_login = false;
	cur_uid = -1;
	cur_user = "";
	cur_gid = -1;
	cur_group = "";
	cur_dir = "";
}

// ����û�
void UserInterface::addu(string uname, string gname, string passwd) {
	//ֻ��admin��������û�
	if (cur_uid != 0)
		throw("-!ERROR: Ȩ�޲���, ֻ�й���Ա��������û�.\n");

	//��ʱ����user�ļ�
	fs.chmod("/etc/users", 0600);	//��ʱ����Ȩ��
	File* user = fs.fopen("/etc/users");
	char* data = new char[user->f_inode->i_size + 1]{ 0 };
	fs.fread(data, user->f_inode->i_size, user);

	//�ж��û��Ƿ���ڣ�����user�ļ����
	string ustrdata = data;
	delete[] data;
	vector<string> users;
	users = splitstr(ustrdata, "\n");
	int next_uid = 0;
	for (unsigned int i = 0; i < users.size(); i++) {
		vector<string>umsg = splitstr(users[i], "-");
		if (umsg[0] == uname) {
			fs.fclose(user);
			fs.chmod("/etc/users", 0000);	//�ջ�Ȩ��
			throw("-!ERROR: ���û��Ѵ���.\n");
		}
		if (stoi(umsg[2]) >= next_uid)
			next_uid = stoi(umsg[2]) + 1;
	}

	//�ж��û����Ƿ���ڣ����ڣ���groups�ļ����uid
	int gid = -1;
	fs.chmod("/etc/groups", 0600);	//��ʱ����Ȩ��
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

	//�ж��û����Ƿ���ڣ����ڣ���users�ļ����group
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
		fs.chmod("/etc/users", 0000);	//�ջ�Ȩ��
		fs.chmod("/etc/groups", 0000);	//�ջ�Ȩ��
		throw("-!ERROR: �û��鲻����.\n");
	}

	fs.fclose(user);
	fs.fclose(group);
	fs.chmod("/etc/users", 0000);	//�ջ�Ȩ��
	fs.chmod("/etc/groups", 0000);	//�ջ�Ȩ��
}

// ɾ���û�
void UserInterface::delu(string uname) {
	if (cur_uid != 0)
		throw("-!ERROR: Ȩ�޲���, ֻ�й���Ա����ɾ���û�.\n");

	if (uname == ADMIN)
		throw("-!ERROR: ����ɾ������Ա�û�.\n");

	fs.chmod("/etc/users", 0600);	//��ʱ����Ȩ��
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
		fs.chmod("/etc/users", 0000);	//�ջ�Ȩ��
		throw("-!ERROR: ���û�������.\n");
	}
	fs.fseek(user, 0, SEEK_SET);
	fs.freplace(new_udata.data(), new_udata.length(), user);
	fs.fclose(user);
	fs.chmod("/etc/users", 0000);	//�ջ�Ȩ��


	fs.chmod("/etc/groups", 0600);	//��ʱ����Ȩ��
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
	fs.chmod("/etc/groups", 0000);	//�ջ�Ȩ��
}

// ����û���
void UserInterface::addg(string gname) {
	if (cur_uid != 0)
		throw("-!ERROR: Ȩ�޲���, ֻ�й���Ա��������û���.\n");

	fs.chmod("/etc/groups", 0600);	//��ʱ����Ȩ��
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
			fs.chmod("/etc/groups", 0000);	//�ջ�Ȩ��
			throw("-!ERROR: ���û����Ѵ���.\n");
		}
		if (stoi(gmsg[1]) >= next_gid)
			next_gid = stoi(gmsg[1]) + 1;
	}

	gstrdata += gname + "-" + to_string(next_gid) + "-\n";
	fs.fseek(group, 0, SEEK_SET);
	fs.fwrite(gstrdata.data(), gstrdata.length(), group);
	fs.fclose(group);
	fs.chmod("/etc/groups", 0000);	//�ջ�Ȩ��
}

// ɾ���û���
void UserInterface::delg(string gname) {
	if (cur_uid != 0)
		throw("-!ERROR: Ȩ�޲���, ֻ�й���Ա����ɾ���û���.\n");

	if (gname == ROOT_GROUP)
		throw("-!ERROR: ����ɾ��root�û���.\n");

	fs.chmod("/etc/groups", 0600);	//��ʱ����Ȩ��
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
		fs.chmod("/etc/groups", 0000);	//�ջ�Ȩ��
		throw("-!ERROR: ���û��鲻����.\n");
	}

	fs.fseek(group, 0, SEEK_SET);
	fs.freplace(new_gdata.data(), new_gdata.length(), group);
	fs.fclose(group);
	fs.chmod("/etc/groups", 0000);	//�ջ�Ȩ��


	fs.chmod("/etc/users", 0600);	//��ʱ����Ȩ��
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
	fs.chmod("/etc/users", 0000);	//�ջ�Ȩ��
}

// ��ʾ�ļ�
void UserInterface::show(const char* name) {
	string fname = string(name);
	if (fname.find("/") != string::npos)
		throw("-!ERROR: �����в��ܰ����༶Ŀ¼.\n");

	string dir = cur_dir + "/" + fname;
	File* fp = fs.fopen(dir.data(), cur_uid, cur_gid);
	char* fdata = new char[fp->f_inode->i_size + 1]{ 0 };
	fs.fread(fdata, fp->f_inode->i_size, fp);
	cout << fdata << endl;

	fs.fclose(fp);
	delete[] fdata;
}

// fs���ļ�����
void UserInterface::cp(const char* ori_fname, const char* tar_fname) {
	//��ȡԴ�ļ�������data
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
			throw("-!ERROR: �޷�����ָ��Ŀ¼.\n");
	}
	fs_dir = fs_dir + "/" + route[route.size() - 1];

	File* fsfp = fs.fopen(fs_dir.data(), cur_uid, cur_gid);
	unsigned file_size = fsfp->f_inode->i_size;
	char* data = new char[file_size + 1]{ 0 };
	fs.fread(data, file_size, fsfp);
	fs.fclose(fsfp);

	//д��Ŀ���ļ�
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
			throw("-!ERROR: �޷�����ָ��Ŀ¼.\n");
	}

	fs_dir2 = fs_dir2 + "/" + route2[route2.size() - 1];

	File* fsfp2 = fs.fopen(fs_dir2.data(), cur_uid, cur_gid);
	fs.freplace(data, file_size, fsfp2);
	fs.fclose(fsfp2);

	delete[] data;
}

// ��win�����ļ���fs
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
			throw("-!ERROR: �޷�����ָ��Ŀ¼.\n");
	}

	fs_dir = fs_dir + "/" + route[route.size() - 1];

	FILE* winfp;
	fopen_s(&winfp, win_fname, "rb");
	if (winfp == NULL)
		throw("-!ERROR: ��windows�ļ�ʧ��.\n");

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

// ��fs�����ļ���win
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
			throw("-!ERROR: �޷�����ָ��Ŀ¼.\n");
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
		throw("-!ERROR: ��windows�ļ�ʧ��.\n");

	fwrite(data, file_size, 1, winfp);
	fclose(winfp);

	delete[] data;
}

/*################################################################*/
// ����ָ����к���
void UserInterface::TestRun() {
	cout << "\n-NOTICE: ��ǰ�ļ�ϵͳʹ��testָ�, ���� fhelp ��ȡ����." << endl;

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
		//printf("\033[40;32m%s@%s:\033[0m", cur_user.data(), local_host.data());	//(��ɫ)cur_user@LOCAL_HOST:
		//printf("\033[40;33m%s%s\033[0m", ROOT_DIR_SYMBOL, cur_dir.data());			//(��ɫ)root/cur_dir
		printf("%s@%s:", cur_user.data(), local_host.data());	//(��ɫ)cur_user@LOCAL_HOST:
		printf("%s%s", ROOT_DIR_SYMBOL, cur_dir.data());		//(��ɫ)root/cur_dir
		cout << (cur_user == ADMIN ? "#" : "$") << " ";

		string buf;				//�ַ�����
		vector<string> args;	//����
		getline(cin, buf);
		args = splitstr(buf, " ");

		try {
			//û������
			if (args.size() <= 0)
				continue;
			
			//���в���ָ��
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
					cout << "-ȷ�����ļ�ϵͳ��ʽ��? [y]: ";
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
					cout << "-NOTICE: ����ֵΪ" << result << endl;
				}
				else if (args[0] == "flseek" && args.size() == 4) {
					int fd = stoi(args[1]);
					int offset = stoi(args[2]);
					int ptrname = stoi(args[3]);
					if (ptrname == 0 && offset >= 0) {
						int result = _flseek(fd, offset, ptrname);
						cout << "-NOTICE: ����ֵΪ" << result << endl;
					}
					else if (ptrname > 0 && ptrname <= 2) {
						int result = _flseek(fd, offset, ptrname);
						cout << "-NOTICE: ����ֵΪ" << result << endl;
					}
					else {
						cout << "-!ERROR: ������Ч.\n";
					}
				}
				else if (args[0] == "fclose" && args.size() == 2) {
					int fd = stoi(args[1]);
					_fclose(fd);
				}
				else if (args[0] == "fcseek" && args.size() == 2) {
					int fd = stoi(args[1]);
					int result = _fcseek(fd);
					cout << "-NOTICE: ����ֵΪ" << result << endl;
				}
				else if (args[0] == "fcreat" && args.size() == 3 && args[2].size() == 3) {
					int master = stoi(args[2].substr(0, 1));
					int group = stoi(args[2].substr(1, 1));
					int others = stoi(args[2].substr(2, 1));
					if (master >= 0 && master <= 7 && group >= 0 && group <= 7 && others >= 0 && others <= 7) {
						int mode = (master << 6) + (group << 3) + others;
						int result = _fcreat(args[1].data(), mode);
						cout << "-NOTICE: ����ֵΪ" << result << endl;
					}
					else {
						cout << "-!ERROR: ����Ȩ����Ч.\n";
					}
				}
				else if (args[0] == "fopen" && args.size() == 3 && args[2].size() == 1) {
					int master = stoi(args[2].substr(0, 1));
					if (master >= 0 && master <= 7) {
						int mode = (master << 6);
						int result = _fopen(args[1].data(), mode);
						cout << "-NOTICE: ����ֵΪ" << result << endl;
					}
					else {
						cout << "-!ERROR: ����Ȩ����Ч.\n";
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
						cout << "-!ERROR: ����Ȩ����Ч.\n";
					}
				}
				else if (args[0] == "fdelete" && args.size() == 2) {
					_ddelete(args[1].data());
				}
				else if (args[0] == "fread" && args.size() == 4) {
					int fd = stoi(args[1]);
					int size = stoi(args[3]);
					int result = _fread(fd, abc, size);
					cout << "-NOTICE: ����ֵΪ" << result << endl;
				}
				else if (args[0] == "fwrite" && args.size() == 4) {
					int fd = stoi(args[1]);
					int size = stoi(args[3]);
					if (args[2] == "init") {
						int result = _fwrite(fd, init, size);
						cout << "-NOTICE: ����ֵΪ" << result << endl;
					}
					else {
						int result = _fwrite(fd, abc, size);
						cout << "-NOTICE: ����ֵΪ" << result << endl;
					}
				}
				else if (args[0] == "fshow" && args.size() == 2) {
					show(args[1].data());
				}
				else{
					cout << "-!ERROR: �����ָ���ݲ�֧��.\n";
				}
			}
		}
		catch (const char* errMsg) {
			cout << errMsg;
		}
	}
}

// ����ָ����к���
void UserInterface::BetaRun() {
	cout << "\n-NOTICE: ��ǰ�ļ�ϵͳʹ��betaָ�, ���� help ��ȡ����." << endl;

	while (true) {
		//printf("\033[40;32m%s@%s:\033[0m", cur_user.data(), local_host.data());	//(��ɫ)cur_user@LOCAL_HOST:
		//printf("\033[40;33m%s%s\033[0m", ROOT_DIR_SYMBOL, cur_dir.data());			//(��ɫ)root/cur_dir
		printf("%s@%s:", cur_user.data(), local_host.data());	//(��ɫ)cur_user@LOCAL_HOST:
		printf("%s%s", ROOT_DIR_SYMBOL, cur_dir.data());		//(��ɫ)root/cur_dir
		cout << (cur_user == ADMIN ? "#" : "$") << " ";

		string buf;				//�ַ�����
		vector<string> args;	//����
		getline(cin, buf);
		args = splitstr(buf, " ");

		try {
			//û������
			if (args.size() <= 0)
				continue;

			//��Ҫ��¼Ȩ��
			else if (is_login) {
				if (args[0] == "logout") {
					logout();
				}
				else if (args[0] == "exit") {
					return;
				}
				else if (args[0] == "format") {
					cout << "-ȷ�����ļ�ϵͳ��ʽ��? [y]: ";
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
						cout << "-!ERROR: ����Ȩ����Ч.\n";
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
						if (ch != 8) {	//���ǻس���¼��
							password[pos] = ch;
							putchar('*');	//�������*��
							pos++;
						}
						else {
							putchar('\b');	//������ɾ��һ��������ͨ������س��� /b���س�һ��
							putchar(' ');	//����ʾ�ո���Ѹղŵ�*����ס��
							putchar('\b');	//Ȼ���ٻس�һ��ȴ�¼�롣
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
					cout << "-!ERROR: �����ָ���ݲ�֧��.\n";
				}
			}

			//����Ҫ��¼Ȩ��(login,cls,exit,format,help)
			else {
				if (args[0] == "login" && args.size() == 2) {
					cout << "password: ";
					char password[256] = { 0 };
					char ch = '\0';
					int pos = 0;
					while ((ch = _getch()) != '\r' && pos < 255) {
						if (ch != 8) {//���ǻس���¼��
							password[pos] = ch;
							putchar('*');//�������*��
							pos++;
						}
						else {
							putchar('\b');//������ɾ��һ��������ͨ������س��� /b���س�һ��
							putchar(' ');//����ʾ�ո���Ѹղŵ�*����ס��
							putchar('\b');//Ȼ����?�س�һ��ȴ�¼�롣
							pos--;
						}
					}
					password[pos] = '\0';
					putchar('\n');
					if (!login(args[1], string(password))) {
						cout << "-!ERROR: ��¼ʧ��.\n";
					}
				}
				else if (args[0] == "exit") {
					return;
				}
				else if (args[0] == "format") {
					cout << "-ȷ�����ļ�ϵͳ��ʽ��? [y]: ";
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
					cout << "-!ERROR: �����ָ���ݲ�֧��, ���¼���ٳ���.\n";
				}
			}
		}
		catch (const char* errMsg) {
			cout << errMsg;
		}
	}
}

// �����к���
void UserInterface::Run() {
	cout << "\n";
	cout << "--------------------------------------------------------------------------------/"	<< endl;
	cout << "| @Project :  FileSystem"															<< endl;
	cout << "| @Author  :  Ray"																	<< endl;
	cout << "| @GitHub  :  https://github.com/RayCorleone/FileSystem"							<< endl;
	cout << "| @Notice  :"																		<< endl;
	cout << "|     1.�˳�ϵͳ���ʹ�� exit ָ��, ������һ�����ʳ���;"							<< endl;
	cout << "|     2.ϵͳ�����, ִ�� format/fformat ָ����Ը�ʽ��ϵͳ;"						<< endl;
	cout << "|     3.����Ա��: admin, ����Ա����: password, ��ʼ�û���: root;"					<< endl;
	cout << "|     4.ϵͳ�� test �� beta ����ָ�, һ��ѡ���, �ڱ������й����в��ɸ���;"		<< endl;
	cout << "|         (1)testָ�: ʹ���ļ��������, ����Ա�Զ���¼;"						<< endl;
	cout << "|         (2)betaָ�: ��Ҫ���е�¼����Ա�˻�, ʹ�ø��߼���ָ��ṹ;"			<< endl;
	cout << "--------------------------------------------------------------------------------/"	<< endl;
		
	// ѡ��test/betaָ�
	while (true) {
		cout << "\n-��ѡ��ָ�(����������exit)[test/beta]: ";
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
			cout << "-!ERROR: ָ������� [test/beta] �е�һ��." << endl;
		}
	}

	return;
}
