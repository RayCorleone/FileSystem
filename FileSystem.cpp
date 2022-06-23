/*
	@name:      FileSystem.cpp
	@function:	FileSystem 类定义
	@author:    Ray
	@version:   1.0
	@problem:	None
	@timestamp: 2022/05/22
	@notice:	在fformat中可以构建基目录; strlen()在读到\0后就停止，
				因此不可以用它判断write的缓存区大小;写的数据超出缓存
				时，会从头开始读，或者报错;
*/

#include "Define.h"

using namespace std;

/*################################################################*/
// FileSystem构造函数
FileSystem::FileSystem() {
	Initialize();
}

// FileSystem析构函数
FileSystem::~FileSystem() {
	SaveSpb();
}

// 初始化文件系统
void FileSystem::Initialize() {
	//初始化磁盘和缓存
	disk.SetDisk(DISK_NAME, &buffer);
	buffer.Initialize(&disk);

	//寻找磁盘文件
	ifstream fin;
	fin.open(disk.disk_name, ios::in | ios::binary | ios::_Nocreate);

	//找不到disk文件，重新创建
	if (!fin.is_open()) {
		tm stdT;
		time_t time1 = unsigned int(time(NULL));
		localtime_s(&stdT, (const time_t*)&time1);
		printf_s("-NOTICE: 正在加载文件系统, 开始时间: %02d:%02d:%02d(不应超过2分钟)\n", stdT.tm_hour, stdT.tm_min, stdT.tm_sec);

		fformat();

		time_t time2 = unsigned int(time(NULL));
		localtime_s(&stdT, (const time_t*)&time2);
		printf_s("-NOTICE: 文件系统加载成功, 结束时间: %02d:%02d:%02d\n", stdT.tm_hour, stdT.tm_min, stdT.tm_sec);
	}

	//读入SuperBlock
	LoadSpb();
}

/*################################################################*/
// 载入特定Inode
Inode FileSystem::ILoad(int i_no) {
	DiskInode d_inode;
	disk.Dread((char*)&d_inode, INODE_ZONE_START_ADDR + i_no * INODE_SIZE, INODE_SIZE);

	Inode inode;
	memcpy_s(&inode, INODE_SIZE, &d_inode, INODE_SIZE);
	inode.i_number = i_no;

	return inode;
}

// 保存特定Inode
void FileSystem::ISave(Inode inode) {
	DiskInode d_inode;
	memcpy_s(&d_inode, INODE_SIZE, &inode, INODE_SIZE);
	disk.Dwrite((const char*)&d_inode, INODE_ZONE_START_ADDR + inode.i_number * INODE_SIZE, INODE_SIZE);
}

// 分配一个Inode
Inode FileSystem::IAlloc() {
	if (spb.s_ifnum <= 0)	//剩余inode为0
		throw("-!ERROR: Inode分配失败, 没有空闲的Inode.\n");

	if (spb.s_ninode <= 0) {	//直接管理的inode数量为0
		//从Inode区重新搜索100个空闲Inode
		for (int i = 0; i < spb.s_inum && spb.s_ninode < 100; i++) {
			Inode item = ILoad(i);
			if ((item.i_mode & IALLOC) == 0)	//入栈
				spb.s_inode[spb.s_ninode++] = i;
		}
	}

	if (spb.s_ninode <= 0 || spb.s_ninode > 100)	//越界
		throw("-!ERROR: 读取空闲Inode越界.\n");

	spb.s_ifnum--;
	int ret_no = spb.s_inode[--spb.s_ninode];	//退栈

	return ILoad(ret_no);
}

// 释放一个Inode
void FileSystem::IFree(Inode inode) {
	memset(&inode, 0, INODE_SIZE);
	ISave(inode);
	spb.s_ifnum++;
	if (spb.s_ninode < 100)	//重新入栈
		spb.s_inode[spb.s_ninode++] = inode.i_number;
}

/*################################################################*/
// 分配一个盘块
int FileSystem::BAlloc() {
	if (spb.s_bfnum <= 0)
		throw("-!ERROR: block分配失败, 没有空闲的block.\n");

	if (spb.s_nfree <= 0 || spb.s_nfree > 100)
		throw("-!ERROR: 读取空闲block越界.\n");

	spb.s_bfnum--;
	int ret_no = spb.s_free[--spb.s_nfree];
	// 空闲block用完, 导入下一批空闲块信息
	if (spb.s_nfree == 0 && spb.s_bfnum > 0) {
		int address = DATA_ZONE_START_ADDR + spb.s_free[0] * BLOCK_SIZE;
		disk.Dread((char*)&spb.s_nfree, address, sizeof(spb.s_nfree));
		disk.Dread((char*)spb.s_free, address + sizeof(spb.s_nfree), sizeof(spb.s_free));
	}
	return ret_no;
}

// 释放一个盘块
void FileSystem::BFree(int no) {
	spb.s_bfnum++;

	// 空闲block达到100, 保存这一批空闲块信息
	if (spb.s_nfree == 100) {
		int cur_block_address = DATA_ZONE_START_ADDR + no * BLOCK_SIZE;
		disk.Dwrite((const char*)&spb.s_nfree, cur_block_address, sizeof(spb.s_nfree));
		disk.Dwrite((const char*)spb.s_free, cur_block_address + int(sizeof(spb.s_nfree)), sizeof(spb.s_free));

		spb.s_nfree = 0;
		memset(spb.s_free, 0, sizeof(spb.s_free));
	}

	spb.s_free[spb.s_nfree++] = no;
}

/*################################################################*/
// 根据文件名定位Inode(在目录项中)
Inode FileSystem::FFind(Inode& dir_inode, const char* name) {
	if ((dir_inode.i_mode & (IFMT)) != IFDIR)
		throw("-!ERROR: 当前路径非目录文件.\n");

	//读出一个Inode中保存的目录项
	DirectoryEntry* dir_list = new DirectoryEntry[dir_inode.i_size / DIR_SIZE];
	FRead(dir_inode, (char*)dir_list, 0, dir_inode.i_size);

	for (int i = 0; i < dir_inode.i_size / DIR_SIZE; i++) {
		//名字相同, 返回对应Inode标号
		if (strcmp(dir_list[i].m_name, name) == 0) {
			int ret_no = dir_list[i].m_ino;
			delete[] dir_list;
			return ILoad(ret_no);
		}
	}
	delete[] dir_list;

	throw("-!ERROR: 找不到该文件.\n");
}

// 根据Inode读取数据到buf
void FileSystem::FRead(Inode& cur_inode, char* buf, unsigned int offset, unsigned int len) {
	if (int(offset + len) > cur_inode.i_size)
		throw("-!ERROR: 读文件越界.\n");

	if (cur_inode.i_size == 0)
		return;

	char* data = new char[cur_inode.i_size]{ 0 };
	unsigned int length = 0;

	//0-5: 直接索引内容读取
	for (int i = 0; i < 6 && length < cur_inode.i_size; i++) {
		if (length + BLOCK_SIZE < cur_inode.i_size) {
			disk.Dread((char*)data + length, DATA_ZONE_START_ADDR + cur_inode.i_addr[i] * BLOCK_SIZE, BLOCK_SIZE);
			length += BLOCK_SIZE;
		}
		else {
			disk.Dread((char*)data + length, DATA_ZONE_START_ADDR + cur_inode.i_addr[i] * BLOCK_SIZE, cur_inode.i_size - length);
			length = cur_inode.i_size;
		}
	}
	//6-7: 一级间接索引内容读取
	for (int i = 6; i < 8 && length < cur_inode.i_size; i++) {
		//读一级索引块
		int* first_index = new int[BLOCK_SIZE / sizeof(int)]{ 0 };
		disk.Dread((char*)first_index, DATA_ZONE_START_ADDR + cur_inode.i_addr[i] * BLOCK_SIZE, BLOCK_SIZE);
		//读一级索引的各块的数据
		for (int j = 0; j < BLOCK_SIZE / sizeof(int) && length < cur_inode.i_size; j++) {
			if (length + BLOCK_SIZE < cur_inode.i_size) {
				disk.Dread((char*)data + length, DATA_ZONE_START_ADDR + first_index[j] * BLOCK_SIZE, BLOCK_SIZE);
				length += BLOCK_SIZE;
			}
			else {
				disk.Dread((char*)data + length, DATA_ZONE_START_ADDR + first_index[j] * BLOCK_SIZE, cur_inode.i_size - length);
				length = cur_inode.i_size;
			}
		}
		delete[] first_index;
	}
	//8-9: 二级间接索引内容读取
	for (int i = 8; i < 10 && length < cur_inode.i_size; i++) {
		//读一级索引块
		int* first_index = new int[BLOCK_SIZE / sizeof(int)]{ 0 };
		disk.Dread((char*)first_index, DATA_ZONE_START_ADDR + cur_inode.i_addr[i] * BLOCK_SIZE, BLOCK_SIZE);
		//读一级索引的各块的数据(还是索引)
		for (int j = 0; j < BLOCK_SIZE / sizeof(int) && length < cur_inode.i_size; j++) {
			//读二级索引块
			int* second_index = new int[BLOCK_SIZE / sizeof(int)]{ 0 };
			disk.Dread((char*)second_index, DATA_ZONE_START_ADDR + first_index[j] * BLOCK_SIZE, BLOCK_SIZE);
			//读二级索引的各块的数据
			for (int k = 0; k < BLOCK_SIZE / sizeof(int) && length < cur_inode.i_size; k++) {
				if (length + BLOCK_SIZE < cur_inode.i_size) {
					disk.Dread((char*)data + length, DATA_ZONE_START_ADDR + second_index[k] * BLOCK_SIZE, BLOCK_SIZE);
					length += BLOCK_SIZE;
				}
				else {
					disk.Dread((char*)data + length, DATA_ZONE_START_ADDR + second_index[k] * BLOCK_SIZE, cur_inode.i_size - length);
					length = cur_inode.i_size;
				}
			}
			delete[] second_index;
		}
		delete[] first_index;
	}

	//将从首地址偏移offset后的数据读入buf
	memcpy_s(buf, len, data + offset, len);
	delete[] data;

	//修改访问时间
	cur_inode.i_atime = unsigned int(time(NULL));
}

// 根据Inode写buf数据到磁盘
void FileSystem::FWrite(Inode& cur_inode, const char* buf, unsigned int offset, unsigned int len) {
	if (int(offset + len) > MAX_FILE_SIZE)
		throw("-!ERROR: 写文件过大.\n");
	unsigned int new_size = int(offset + len) > cur_inode.i_size ? offset + len : cur_inode.i_size;

	//读出原始的数据
	char* data = new char[new_size + 1]{ 0 };
	FRead(cur_inode, data, 0, cur_inode.i_size);
	//写入位置超过文件本身长度: 将中间的内容置0
	if (offset > cur_inode.i_size)
		memset(data + cur_inode.i_size, 0, offset - cur_inode.i_size);
	//将buf中数据保存到data后
	memcpy_s(data + offset, len, buf, len);

	//扩充填写i_addr[10]对应的Block地址
	int last_used_block_cnt = (cur_inode.i_size + BLOCK_SIZE - 1) / BLOCK_SIZE;	//原先需要多少数据块
	int new_last_block_cnt = (new_size + BLOCK_SIZE - 1) / BLOCK_SIZE;			//现在需要多少数据块
	int block_cnt = 0;
	if (last_used_block_cnt < new_last_block_cnt) { //文件需要更多块
		//block_cnt = 0;
		if (last_used_block_cnt < SMALL_FILE_BLOCK)
			block_cnt = 0;
		else if (last_used_block_cnt < LARGE_FILE_BLOCK)
			block_cnt = SMALL_FILE_BLOCK;
		else if (last_used_block_cnt < HUGE_FILE_BLOCK)
			block_cnt = LARGE_FILE_BLOCK;

		//0-5: 直接索引填写i_addr
		for (int i = 0; block_cnt < SMALL_FILE_BLOCK && i < 6 && block_cnt < new_last_block_cnt; i++) {
			if (block_cnt < last_used_block_cnt)
				block_cnt++;
			else {
				int block_no = BAlloc();
				cur_inode.i_addr[i] = block_no;	//直接填写i_addr[i]对应block_no
				block_cnt++;
			}
		}
		//6-7: 一级索引填写i_addr
		for (int i = 6; block_cnt < LARGE_FILE_BLOCK && i < 8 && block_cnt < new_last_block_cnt; i++) {
			//读入一级索引
			int* first_index = new int[BLOCK_SIZE / sizeof(int)]{ 0 };
			if (block_cnt < last_used_block_cnt)
				disk.Dread((char*)first_index, DATA_ZONE_START_ADDR + cur_inode.i_addr[i] * BLOCK_SIZE, BLOCK_SIZE);
			else {
				int block_no = BAlloc();
				cur_inode.i_addr[i] = block_no;	//填写i_addr[i]对应block_no(一级索引块)
			}
			//填写一级索引各项
			for (int j = 0; j < BLOCK_SIZE / sizeof(int) && block_cnt < new_last_block_cnt; j++) {
				if (block_cnt < last_used_block_cnt)
					block_cnt++;
				else {
					int block_no = BAlloc();
					first_index[j] = block_no;	//填写一级索引 i 项对应block_no
					block_cnt++;
				}
			}
			disk.Dwrite((const char*)first_index, DATA_ZONE_START_ADDR + cur_inode.i_addr[i] * BLOCK_SIZE, BLOCK_SIZE);
			delete[] first_index;
		}
		//8-9: 二级索引填写i_addr
		for (int i = 8; block_cnt < HUGE_FILE_BLOCK && i < 10 && block_cnt < new_last_block_cnt; i++) {
			//读入一级索引
			int* first_index = new int[BLOCK_SIZE / sizeof(int)]{ 0 };
			if (block_cnt < last_used_block_cnt)
				disk.Dread((char*)first_index, DATA_ZONE_START_ADDR + cur_inode.i_addr[i] * BLOCK_SIZE, BLOCK_SIZE);
			else {
				int block_no = BAlloc();
				cur_inode.i_addr[i] = block_no;
			}

			//填写一级索引对应项
			for (int j = 0; j < BLOCK_SIZE / sizeof(int) && block_cnt < new_last_block_cnt; j++) {
				//读入二级索引项
				int* second_index = new int[BLOCK_SIZE / sizeof(int)]{ 0 };
				if (block_cnt < last_used_block_cnt)
					disk.Dread((char*)second_index, DATA_ZONE_START_ADDR + first_index[j] * BLOCK_SIZE, BLOCK_SIZE);
				else {
					int block_no = BAlloc();
					first_index[j] = block_no;
				}
				//填写二级索引各项
				for (int k = 0; k < BLOCK_SIZE / sizeof(int) && block_cnt < new_last_block_cnt; k++) {
					if (block_cnt < last_used_block_cnt)
						block_cnt++;
					else {
						int block_no = BAlloc();
						second_index[k] = block_no;
						block_cnt++;
					}
				}
				disk.Dwrite((const char*)second_index, DATA_ZONE_START_ADDR + first_index[j] * BLOCK_SIZE, BLOCK_SIZE);
				delete[] second_index;
			}
			disk.Dwrite((const char*)first_index, DATA_ZONE_START_ADDR + cur_inode.i_addr[i] * BLOCK_SIZE, BLOCK_SIZE);
			delete[] first_index;
		}
	}
	cur_inode.i_size = new_size;

	//写入数据到分配的盘块
	unsigned int length = 0;
	//0-5: 直接索引盘块写入
	for (int i = 0; i < 6 && length < cur_inode.i_size; i++) {
		if (length + BLOCK_SIZE < cur_inode.i_size) {
			disk.Dwrite((const char*)data + length, DATA_ZONE_START_ADDR + cur_inode.i_addr[i] * BLOCK_SIZE, BLOCK_SIZE);
			length += BLOCK_SIZE;
		}
		else {
			disk.Dwrite((const char*)data + length, DATA_ZONE_START_ADDR + cur_inode.i_addr[i] * BLOCK_SIZE, cur_inode.i_size - length);
			length = cur_inode.i_size;
		}
	}
	//6-7: 一级索引的盘块写入
	for (int i = 6; i < 8 && length < cur_inode.i_size; i++) {
		//读一级索引块
		int* first_index = new int[BLOCK_SIZE / sizeof(int)]{ 0 };
		disk.Dread((char*)first_index, DATA_ZONE_START_ADDR + cur_inode.i_addr[i] * BLOCK_SIZE, BLOCK_SIZE);
		//写一级索引的内容
		for (int j = 0; j < BLOCK_SIZE / sizeof(int) && length < cur_inode.i_size; j++) {
			if (length + BLOCK_SIZE < cur_inode.i_size) {
				disk.Dwrite((const char*)data + length, DATA_ZONE_START_ADDR + first_index[j] * BLOCK_SIZE, BLOCK_SIZE);
				length += BLOCK_SIZE;
			}
			else {
				disk.Dwrite((const char*)data + length, DATA_ZONE_START_ADDR + first_index[j] * BLOCK_SIZE, cur_inode.i_size - length);
				length = cur_inode.i_size;
			}
		}
		delete[] first_index;
	}
	//8-9: 二级索引的盘块写入
	for (int i = 8; i < 10 && length < cur_inode.i_size; i++) {
		//一级索引盘块读入
		int* first_index = new int[BLOCK_SIZE / sizeof(int)]{ 0 };
		disk.Dread((char*)first_index, DATA_ZONE_START_ADDR + cur_inode.i_addr[i] * BLOCK_SIZE, BLOCK_SIZE);
		//读出二级索引
		for (int j = 0; j < BLOCK_SIZE / sizeof(int) && length < cur_inode.i_size; j++) {
			//二级索引盘块读入
			int* second_index = new int[BLOCK_SIZE / sizeof(int)]{ 0 };
			disk.Dread((char*)second_index, DATA_ZONE_START_ADDR + first_index[j] * BLOCK_SIZE, BLOCK_SIZE);
			//写入二级索引的盘块
			for (int k = 0; k < BLOCK_SIZE / sizeof(int) && length < cur_inode.i_size; k++) {
				if (length + BLOCK_SIZE < cur_inode.i_size) {
					disk.Dwrite((const char*)data + length, DATA_ZONE_START_ADDR + second_index[k] * BLOCK_SIZE, BLOCK_SIZE);
					length += BLOCK_SIZE;
				}
				else {
					disk.Dwrite((const char*)data + length, DATA_ZONE_START_ADDR + second_index[k] * BLOCK_SIZE, cur_inode.i_size - length);
					length = cur_inode.i_size;
				}
			}
			delete[] second_index;
		}
		delete[] first_index;
	}
	delete[] data;

	//修改Inode信息
	if (cur_inode.i_size <= 6 * BLOCK_SIZE)
		cur_inode.i_mode &= (~(ILARG));
	else
		cur_inode.i_mode |= ILARG;
	cur_inode.i_atime = unsigned int(time(NULL));
	cur_inode.i_mtime = unsigned int(time(NULL));
}

// 根据Inode删除文件
void FileSystem::FDelete(Inode& cur_inode) {
	unsigned int length = 0;

	//0-5: 直接索引块释放
	for (int i = 0; i < 6 && length < cur_inode.i_size; i++) {
		BFree(cur_inode.i_addr[i]);
		length += BLOCK_SIZE;
	}
	//6-7: 一级索引块释放
	for (int i = 6; i < 8 && length < cur_inode.i_size; i++) {
		int* first_index = new int[BLOCK_SIZE / sizeof(int)]{ 0 };
		disk.Dread((char*)first_index, DATA_ZONE_START_ADDR + cur_inode.i_addr[i] * BLOCK_SIZE, BLOCK_SIZE);
		for (int j = 0; j < BLOCK_SIZE / sizeof(int) && length < cur_inode.i_size; j++) {
			BFree(first_index[j]);
			length += BLOCK_SIZE;
		}
		BFree(cur_inode.i_addr[i]);
		delete[] first_index;
	}
	//8-9: 二级索引块释放
	for (int i = 8; i < 10 && length < cur_inode.i_size; i++) {
		int* second_index = new int[BLOCK_SIZE / sizeof(int)]{ 0 };
		disk.Dread((char*)second_index, DATA_ZONE_START_ADDR + cur_inode.i_addr[i] * BLOCK_SIZE, BLOCK_SIZE);
		for (int j = 0; j < BLOCK_SIZE / sizeof(int) && length < cur_inode.i_size; j++) {
			int* one_time_indirect_index = new int[BLOCK_SIZE / sizeof(int)]{ 0 };
			disk.Dread((char*)one_time_indirect_index, DATA_ZONE_START_ADDR + second_index[j] * BLOCK_SIZE, BLOCK_SIZE);
			for (int k = 0; k < BLOCK_SIZE / sizeof(int) && length < cur_inode.i_size; k++) {
				BFree(one_time_indirect_index[k]);
				length += BLOCK_SIZE;
			}
			BFree(second_index[j]);
			delete[] one_time_indirect_index;
		}
		BFree(cur_inode.i_addr[i]);
		delete[] second_index;
	}

	//修改Inode信息
	cur_inode.i_size = 0;
	cur_inode.i_mode &= (~(ILARG));
	cur_inode.i_atime = unsigned int(time(NULL));
	cur_inode.i_mtime = unsigned int(time(NULL));
}

// 根据Inode删除子项(目录)
void FileSystem::FDeleteall(Inode& cur_inode) {
	//不是目录项: 直接删除
	if ((cur_inode.i_mode & (IFMT)) != IFDIR)
		FDelete(cur_inode);
	//是目录项: 递归删除
	else {
		DirectoryEntry* dir_list = new DirectoryEntry[cur_inode.i_size / DIR_SIZE];
		FRead(cur_inode, (char*)dir_list, 0, cur_inode.i_size);
		//删除每一个目录项对应文件
		for (int i = 0; i < cur_inode.i_size / DIR_SIZE; i++) {
			if (strcmp(dir_list[i].m_name, ".") == 0 || strcmp(dir_list[i].m_name, "..") == 0)
				continue;
			else {
				Inode sub_dir = ILoad(dir_list[i].m_ino);
				FDeleteall(sub_dir);
				IFree(sub_dir);
			}
		}
		FDelete(cur_inode);
		delete[] dir_list;
	}
}

/*################################################################*/
// 获得SuperBlock
SuperBlock* FileSystem::GetSpb() { return &spb; }

// 保存SuperBlock
void FileSystem::SaveSpb() {
	disk.Dwrite((const char*)&spb, SUPER_BLOCK_START_ADDR, 2 * BLOCK_SIZE);
}

// 载入SuperBlock
void FileSystem::LoadSpb() {
	disk.Dread((char*)&spb, SUPER_BLOCK_START_ADDR, 2 * BLOCK_SIZE);
}

// 格式化文件系统
void FileSystem::fformat() {
	// 1.创建新的磁盘卷
	fstream fout;
	fout.open(disk.disk_name, ios::out | ios::binary);
	if (!fout.is_open())
		throw("-!ERROR: 磁盘映像输入文件流打开失败.\n");
	fout.seekp(MAX_DISK_SIZE - 1, ios::beg);
	fout.write("", sizeof(char));
	fout.close();


	// 2.初始化SuperBlock
	spb.s_ninode = 0;
	for (int i = 99; i >= 0; i--)
		spb.s_inode[spb.s_ninode++] = i;
	spb.s_nfree = 1;
	memset(spb.s_free, 0, sizeof(spb.s_free));
	spb.s_free[0] = 0;
	for (int i = spb.s_bnum - 1; i >= 0; i--) {
		int cur_block_address = DATA_ZONE_START_ADDR + i * BLOCK_SIZE;
		if (spb.s_nfree == 100) {
			disk.Dwrite((const char*)&spb.s_nfree, cur_block_address, sizeof(spb.s_nfree));
			disk.Dwrite((const char*)spb.s_free, cur_block_address + int(sizeof(spb.s_nfree)), sizeof(spb.s_free));
			spb.s_nfree = 0;
			memset(spb.s_free, 0, sizeof(spb.s_free));
		}
		spb.s_free[spb.s_nfree++] = i;
	}
	SaveSpb();


	// 3.重新读入SuperBlock
	LoadSpb();


	// 4.建立根目录
	Inode root = IAlloc();
	int block_no = BAlloc();
	DirectoryEntry* dir_list = new DirectoryEntry[DIR_NUMBER_PER_SECTOR];
	strcpy_s(dir_list[0].m_name, ".");
	dir_list[0].m_ino = root.i_number;
	disk.Dwrite((const char*)dir_list, DATA_ZONE_START_ADDR + block_no * BLOCK_SIZE, BLOCK_SIZE);
	delete[] dir_list;
	root.i_mode = IALLOC | IFDIR | RDIR;
	root.i_nlink = 1;
	root.i_uid = 0;
	root.i_gid = 0;
	root.i_size = DIR_SIZE;
	root.i_addr[0] = block_no;
	root.i_atime = unsigned int(time(NULL));
	root.i_mtime = unsigned int(time(NULL));
	ISave(root);


	// 5.按照要求构建目录
	dcreate("/bin");
	dcreate("/etc");
	dcreate("/home");
	dcreate("/dev");
	dcreate("/home/texts");
	dcreate("/home/reports");
	dcreate("/home/photos");
	fcreate("/home/texts/ReadMe.txt", 0, 0);
	fcreate("/home/reports/Report.pdf", 0, 0);
	fcreate("/home/photos/Photo.jpg", 0, 0);


	// 6.复制文件到文件夹
	int text_size = get_fsize_from_windows("./res/ReadMe.txt");
	cout << " - text size:\t" << setw(8) << setfill(' ') << text_size << " B ";
	char* text_data = new char[text_size + 1]{ 0 };
	text_data = read_from_windows("./res/ReadMe.txt");
	File* text_fp = fopen("/home/texts/ReadMe.txt", 0, 0);
	cout << "=> wrote size: " << setw(8) << setfill(' ') << freplace(text_data, text_size, text_fp) << " B" << endl;
	fclose(text_fp);
	delete[] text_data;

	int report_size = get_fsize_from_windows("./res/Report.pdf");
	cout << " - report size:\t" << setw(8) << setfill(' ') << report_size << " B ";
	char* report_data = new char[report_size + 1]{ 0 };
	report_data = read_from_windows("./res/Report.pdf");
	File* report_fp = fopen("/home/reports/Report.pdf", 0, 0);
	cout << "=> wrote size: " << setw(8) << setfill(' ') << freplace(report_data, report_size, report_fp) << " B" << endl;
	fclose(report_fp);
	delete[] report_data;

	int photo_size = get_fsize_from_windows("./res/Photo.jpg");
	cout << " - photo size:\t" << setw(8) << setfill(' ') << photo_size << " B ";
	char* photo_data = new char[photo_size + 1]{ 0 };
	photo_data = read_from_windows("./res/Photo.jpg");
	File* photo_fp = fopen("/home/photos/Photo.jpg", 0, 0);
	cout << "=> wrote size: " << setw(8) << setfill(' ') << freplace(photo_data, photo_size, photo_fp) << " B" << endl;
	fclose(photo_fp);
	delete[] photo_data;


	// 7.添加用户信息
	fcreate("/etc/users");	//此时文件可写可读
	File* user = fopen("/etc/users");
	string users_data = ADMIN;
	users_data = users_data + "-" + ADMIN_PSW + "-0-0\n";	//name-passwd-uid-gid
	fwrite(users_data.data(), users_data.length(), user);
	fclose(user);
	chmod("/etc/users", 0000, 0, 0);	//不可读不可写

	fcreate("/etc/groups");	//此时文件可写可读
	File* group = fopen("/etc/groups");
	string groups_data = ROOT_GROUP;
	groups_data = groups_data + "-0-0\n";//name-gid-uid,uid...
	fwrite(groups_data.data(), groups_data.length(), group);
	fclose(group);
	chmod("/etc/groups", 0000, 0, 0);	//不可读不可写


	// 8.更新SuperBlock
	SaveSpb();
	return;
}

/*################################################################*/
// 创建新目录
int FileSystem::dcreate(const char* dir, short uid, short gid) {
	vector<string> route = splitstr(dir, "/");
	string fname = route[route.size() - 1];

	if (fname.size() > NAME_SIZE)
		throw("-!ERROE: 新增目录名超过最大长度28.\n");

	Inode cur_dir = ILoad(ROOT_INO);
	for (int i = 0; i < route.size() - 1; i++)
		cur_dir = FFind(cur_dir, route[i].data());

	if ((cur_dir.i_mode & (IFMT)) != IFDIR)
		throw("-!ERROE: 当前路径非目录文件.\n");

	int file_mode;
	if (uid == cur_dir.i_uid)
		file_mode = 6;
	else if (gid == cur_dir.i_gid)
		file_mode = 3;
	else
		file_mode = 0;

	if (((cur_dir.i_mode >> file_mode) & 2) == 0 && uid != 0)
		throw("-!ERROR: 权限不足, 缺少写权限.\n");	//

	if (cur_dir.i_size >= MAX_FILE_SIZE)
		throw("-!ERROR: 当前路径目录项已写满.\n");

	DirectoryEntry* dir_list = new DirectoryEntry[cur_dir.i_size / DIR_SIZE];
	FRead(cur_dir, (char*)dir_list, 0, cur_dir.i_size);
	for (int i = 0; i < cur_dir.i_size / DIR_SIZE; i++) {
		if (string(dir_list[i].m_name) == fname)
			throw("-!ERROR: 当前路径下已有该目录名.\n");
	}
	delete[] dir_list;

	Inode new_dir = IAlloc();
	new_dir.i_mode = IALLOC | IFDIR | PDIR;
	new_dir.i_nlink = 1;
	new_dir.i_uid = uid;
	new_dir.i_gid = gid;
	new_dir.i_size = 0;
	new_dir.i_mtime = unsigned int(time(NULL));
	new_dir.i_atime = unsigned int(time(NULL));

	DirectoryEntry* new_dir_list = new DirectoryEntry[2];
	strcpy_s(new_dir_list[0].m_name, ".");	//当前目录
	new_dir_list[0].m_ino = new_dir.i_number;
	strcpy_s(new_dir_list[1].m_name, "..");	//上级目录
	new_dir_list[1].m_ino = cur_dir.i_number;
	FWrite(new_dir, (const char*)new_dir_list, 0, 2 * DIR_SIZE);
	delete[] new_dir_list;
	ISave(new_dir);

	DirectoryEntry new_item;
	strcpy_s(new_item.m_name, fname.data());
	new_item.m_ino = new_dir.i_number;
	cur_dir.i_nlink++;
	FWrite(cur_dir, (const char*)&new_item, cur_dir.i_size, DIR_SIZE);
	ISave(cur_dir);

	SaveSpb();
	return 0;
}

// 删除目录
int FileSystem::ddelete(const char* dir, short uid, short gid) {
	vector<string> route = splitstr(dir, "/");
	string fname = route[route.size() - 1];

	Inode cur_dir = ILoad(ROOT_INO);
	for (int i = 0; i < route.size() - 1; i++)
		cur_dir = FFind(cur_dir, route[i].data());

	if ((cur_dir.i_mode & (IFMT)) != IFDIR)
		throw("-!ERROR: 当前路径非目录文件.\n");

	if (fname == "." || fname == "..")
		throw("-!ERROR: 不能对本目录或上级目录执行删除操作.\n");

	int file_mode;
	if (uid == cur_dir.i_uid)
		file_mode = 6;
	else if (gid == cur_dir.i_gid)
		file_mode = 3;
	else
		file_mode = 0;

	if (((cur_dir.i_mode >> file_mode) & 2) == 0 && uid != 0)
		throw("-!ERROR: 权限不足, 缺少写权限.\n");

	DirectoryEntry* dir_list = new DirectoryEntry[cur_dir.i_size / DIR_SIZE];
	FRead(cur_dir, (char*)dir_list, 0, cur_dir.i_size);

	int fno = -1;
	int pos = -1;
	for (int i = 0; i < int(cur_dir.i_size / DIR_SIZE); i++) {
		if (string(dir_list[i].m_name) == fname) {
			fno = dir_list[i].m_ino;
			pos = i;
			break;
		}
	}
	if (pos == -1)
		throw("-!ERROR: 找不到该目录.\n");

	Inode finode = ILoad(fno);
	if ((finode.i_mode & (IFMT)) != IFDIR)
		throw("-!ERROR: 该文件是非目录文件, 不能用ddelete删除.\n");

	// 删除目录
	FDeleteall(finode);
	IFree(finode);

	// 更新目录
	unsigned int old_size = cur_dir.i_size;
	FDelete(cur_dir);
	cur_dir.i_nlink--;
	for (int i = pos; i < old_size / DIR_SIZE - 1; i++)
		dir_list[i] = dir_list[i + 1];

	FWrite(cur_dir, (const char*)dir_list, 0, old_size - DIR_SIZE);
	delete[] dir_list;
	ISave(cur_dir);

	SaveSpb();
	return 0;
}

// 进入目录
bool FileSystem::enter(const char* dir, short uid, short gid) {
	Inode cur_dir = ILoad(ROOT_INO);
	vector<string> route = splitstr(dir, "/");

	for (int i = 0; i < route.size(); i++) {
		int file_mode;
		if (uid == cur_dir.i_uid)
			file_mode = 6;
		else if (gid == cur_dir.i_gid)
			file_mode = 3;
		else
			file_mode = 0;

		if (((cur_dir.i_mode >> file_mode) & 1) == 0 && uid != 0)
			throw("-!ERROR: 权限不足, 缺少执行权限.\n");

		cur_dir = FFind(cur_dir, route[i].data());
	}

	int file_mode;
	if (uid == cur_dir.i_uid)
		file_mode = 6;
	else if (gid == cur_dir.i_gid)
		file_mode = 3;
	else
		file_mode = 0;

	if (((cur_dir.i_mode >> file_mode) & 1) == 0 && uid != 0)
		throw("-!ERROR: 权限不足, 缺少执行权限.\n");

	if ((cur_dir.i_mode & (IFMT)) != IFDIR)
		throw("-!ERROR: 目标路径非目录文件.\n");

	return 1;
}

// 创建新文件
int FileSystem::fcreate(const char* dir, short uid, short gid) {
	vector<string> route = splitstr(dir, "/");	//分解路径
	string fname = route[route.size() - 1];		//获得文件名

	if (fname.size() > NAME_SIZE)
		throw("-!ERROR: 新增文件名超过最大长度28.\n");

	Inode cur_dir = ILoad(ROOT_INO);
	for (int i = 0; i < route.size() - 1; i++)
		cur_dir = FFind(cur_dir, route[i].data());

	if ((cur_dir.i_mode & (3 << 13)) != IFDIR)
		throw("-!ERROR: 当前路径非目录文件.\n");

	int file_mode;
	if (uid == cur_dir.i_uid)
		file_mode = 6;
	else if (gid == cur_dir.i_gid)
		file_mode = 3;
	else
		file_mode = 0;

	if (((cur_dir.i_mode >> file_mode) & 2) == 0 && uid != 0)
		throw("-!ERROR: 权限不足, 缺少写权限.\n");

	if (cur_dir.i_size >= MAX_FILE_SIZE)
		throw("-!ERROR: 当前路径目录项已写满.\n");

	DirectoryEntry* dir_list = new DirectoryEntry[cur_dir.i_size / DIR_SIZE];
	FRead(cur_dir, (char*)dir_list, 0, cur_dir.i_size);
	for (int i = 0; i < cur_dir.i_size / DIR_SIZE; i++) {
		if (string(dir_list[i].m_name) == fname)
			throw("-!ERROR: 当前路径下已有该文件名.\n");
	}
	delete[] dir_list;

	Inode new_inode = IAlloc();
	new_inode.i_mode = IALLOC | PFILE;
	new_inode.i_nlink = 1;
	new_inode.i_uid = uid;
	new_inode.i_gid = gid;
	new_inode.i_size = 0;
	new_inode.i_mtime = unsigned int(time(NULL));
	new_inode.i_atime = unsigned int(time(NULL));
	ISave(new_inode);

	DirectoryEntry new_dir;
	strcpy_s(new_dir.m_name, fname.data());
	new_dir.m_ino = new_inode.i_number;
	FWrite(cur_dir, (const char*)&new_dir, cur_dir.i_size, DIR_SIZE);
	ISave(cur_dir);

	SaveSpb();
	return 0;
}

// 删除文件
int FileSystem::fdelete(const char* dir, short uid, short gid) {
	vector<string> route = splitstr(dir, "/");	//获得路径
	string fname = route[route.size() - 1];

	Inode cur_dir = ILoad(ROOT_INO);
	for (int i = 0; i < route.size() - 1; i++)
		cur_dir = FFind(cur_dir, route[i].data());

	if ((cur_dir.i_mode & (3 << 13)) != IFDIR)
		throw("-!ERROR: 当前路径非目录文件.\n");

	int dir_mode;
	if (uid == cur_dir.i_uid)
		dir_mode = 6;
	else if (gid == cur_dir.i_gid)
		dir_mode = 3;
	else
		dir_mode = 0;
	if (((cur_dir.i_mode >> dir_mode) & 2) == 0 && uid != 0)
		throw("-!ERROR: 权限不足, 缺少写权限.\n");

	DirectoryEntry* dir_list = new DirectoryEntry[cur_dir.i_size / DIR_SIZE];
	FRead(cur_dir, (char*)dir_list, 0, cur_dir.i_size);
	int fno = -1;
	int pos = -1;
	for (int i = 0; i < cur_dir.i_size / DIR_SIZE; i++)	{
		if (string(dir_list[i].m_name) == fname)		{
			fno = dir_list[i].m_ino;
			pos = i;
			break;
		}
	}
	if (pos == -1)
		throw("-!ERROR: 找不到该文件.\n");

	Inode finode = ILoad(fno);
	if ((finode.i_mode & (IFMT)) == IFDIR){
		delete[] dir_list;
		throw("-!ERROR: 该文件是目录文件, 不能用fdelete删除.\n");
	}

	int file_mode;
	if (uid == finode.i_uid)
		file_mode = 6;
	else if (gid == finode.i_gid)
		file_mode = 3;
	else
		file_mode = 0;
	if (((finode.i_mode >> file_mode) & 2) == 0 && uid != 0)
		throw("-!ERROR: 权限不足, 缺少写权限.\n");

	FDelete(finode);
	IFree(finode);

	unsigned int old_size = cur_dir.i_size;
	FDelete(cur_dir);
	for (int i = pos; i < old_size / DIR_SIZE - 1; i++)
		dir_list[i] = dir_list[i + 1];

	FWrite(cur_dir, (const char*)dir_list, 0, old_size - DIR_SIZE);
	delete[] dir_list;
	ISave(cur_dir);

	SaveSpb();
	return 0;
}

// 打开文件
File* FileSystem::fopen(const char* dir, short uid, short gid) {
	Inode cur_inode = ILoad(ROOT_INO);
	vector<string> route = splitstr(dir, "/");

	for (int i = 0; i < route.size(); i++)
		cur_inode = FFind(cur_inode, route[i].data());

	if ((cur_inode.i_mode & (IFMT)) == IFDIR)
		throw("-!ERROR: 不能打开目录文件.\n");

	File* fp = new File;
	Inode* ip = new Inode;
	memcpy_s(ip, sizeof(Inode), &cur_inode, sizeof(Inode));
	fp->f_uid = uid;
	fp->f_gid = gid;
	fp->f_inode = ip;
	fp->f_offset = 0;

	SaveSpb();
	return fp;
}

// 关闭文件
int FileSystem::fclose(File* fp) {
	ISave(*fp->f_inode);
	delete fp->f_inode;
	delete fp;
	SaveSpb();
	return 0;
}

// 获得文件读写指针位置
int FileSystem::fcseek(File* fp) { return fp->f_offset; }

// 修改文件读写指针
int FileSystem::fseek(File* fp, int offset, int mode) {
	if (mode == SEEK_SET)	//设置
		fp->f_offset = offset;
	else if (mode == SEEK_CUR)	//位移
		fp->f_offset = fp->f_offset + offset;
	else	//追加
		fp->f_offset = fp->f_inode->i_size - 1 + offset;

	return fp->f_offset;
}

// 读文件到buf
int FileSystem::fread(char* buf, int size, File* fp) {
	int file_mode;
	if (fp->f_uid == fp->f_inode->i_uid)
		file_mode = 6;
	else if (fp->f_gid == fp->f_inode->i_gid)
		file_mode = 3;
	else
		file_mode = 0;
	if (((fp->f_inode->i_mode >> file_mode) & 4) == 0)// && fp->f_uid != 0)
		throw("-!ERROR: 权限不足, 缺少读权限.\n");

	int length;
	if (int(fp->f_offset + size) > fp->f_inode->i_size)
		length = fp->f_inode->i_size - fp->f_offset;
	else
		length = size;

	FRead(*fp->f_inode, buf, fp->f_offset, length);
	fp->f_offset += length;

	SaveSpb();
	return length;
}

// 写buf到文件
int FileSystem::fwrite(const char* buf, int size, File* fp) {
	int file_mode;
	if (fp->f_uid == fp->f_inode->i_uid)
		file_mode = 6;
	else if (fp->f_gid == fp->f_inode->i_gid)
		file_mode = 3;
	else
		file_mode = 0;
	if (((fp->f_inode->i_mode >> file_mode) & 2) == 0)// && fp->f_uid != 0)
		throw("-!ERROR: 权限不足, 缺少写权限.\n");

	int len = size;

	FWrite(*fp->f_inode, buf, fp->f_offset, len);
	fp->f_offset += len;

	SaveSpb();
	return len;
}

// 写buf到文件(替换)(删除原文件所有内容,把buf按照size大小写到fp->offset后)
int FileSystem::freplace(const char* buf, int size, File* fp) {
	int file_mode;
	if (fp->f_uid == fp->f_inode->i_uid)
		file_mode = 6;
	else if (fp->f_gid == fp->f_inode->i_gid)
		file_mode = 3;
	else
		file_mode = 0;
	if (((fp->f_inode->i_mode >> file_mode) & 2) == 0)// && fp->f_uid != 0)
		throw("-!ERROR: 权限不足, 缺少写权限.\n");

	char* data = new char[fp->f_inode->i_size + 1]{ 0 };
	FRead(*fp->f_inode, data, 0, fp->f_inode->i_size);
	FDelete(*fp->f_inode);

	int len = size;

	char* new_data = new char[fp->f_offset + len + 1]{ 0 };
	memcpy_s(new_data, fp->f_offset, data, fp->f_offset);	//复制offset前的内容
	memcpy_s(new_data + fp->f_offset, len, buf, len);		//重写offset之后内容
	FWrite(*fp->f_inode, new_data, 0, fp->f_offset + len);
	delete[] data;
	delete[] new_data;
	fp->f_offset += len;

	SaveSpb();
	return len;
}

// 修改权限
int FileSystem::chmod(const char* dir, int mode, short uid, short gid) {
	Inode cur_dir = ILoad(ROOT_INO);
	vector<string> route = splitstr(dir, "/");
	for (int i = 0; i < route.size(); i++)
		cur_dir = FFind(cur_dir, route[i].data());

	if (uid != cur_dir.i_uid && uid != 0)
		throw("-!ERROR: 权限不足, 只有管理员或文件主可修改.\n");

	cur_dir.i_mode = (cur_dir.i_mode >> 9 << 9) | mode;	//修改权限
	ISave(cur_dir);

	SaveSpb();
	return 0;
}

// 列出文件信息
vector<string> FileSystem::flist(const char* dir, short uid, short gid) {
	chmod("/etc/users", 0400);	//临时开放权限
	chmod("/etc/groups", 0400);	//临时开放权限

	vector<string>ret_list;

	Inode cur_dir = ILoad(ROOT_INO);
	vector<string> route = splitstr(dir, "/");
	for (int i = 0; i < route.size(); i++)
		cur_dir = FFind(cur_dir, route[i].data());
	if ((cur_dir.i_mode & (3 << 13)) != IFDIR)
		throw("-!ERROR: 当前路径非目录文件.\n");

	int file_mode;
	if (uid == cur_dir.i_uid)
		file_mode = 6;
	else if (gid == cur_dir.i_gid)
		file_mode = 3;
	else
		file_mode = 0;
	if (((cur_dir.i_mode >> file_mode) & 4) == 0 && uid != 0)
		throw("-!ERROR: 权限不足, 缺少读权限.\n");

	DirectoryEntry* dir_list = new DirectoryEntry[cur_dir.i_size / DIR_SIZE];
	FRead(cur_dir, (char*)dir_list, 0, cur_dir.i_size);

	char buf_head[128];
	sprintf_s(buf_head, "%10s    %-8s  %-28s  %-7s  %-7s  %s  \t%s\n", "类型&读写权限", "链接数", "文件(目录)名", "文件主", "用户组", "大小", "修改时间");
	ret_list.push_back(string(buf_head));

	for (int i = 0; i < cur_dir.i_size / DIR_SIZE; i++) {
		Inode sub_inode = ILoad(dir_list[i].m_ino);

		string mode;
		//文件类型
		if (((sub_inode.i_mode >> 13) & 3) == 0)		//普通文件
			mode += "-";
		else if (((sub_inode.i_mode >> 13) & 3) == 1)	//字符设备
			mode += "c";
		else if (((sub_inode.i_mode >> 13) & 3) == 2)	//目录文件
			mode += "d";
		else if (((sub_inode.i_mode >> 13) & 3) == 3)	//块设备
			mode += "b";
		mode += "|";
		//用户权限
		for (int t = 8; t >= 0; t--) {
			if (((sub_inode.i_mode >> t) & 1) == 1) {
				if (t % 3 == 2)			//读
					mode += "r";
				else if (t % 3 == 1)	//写
					mode += "w";
				else if (t % 3 == 0)	//执行
					mode += "x";
			}
			else
				mode += "-";			//无

			if (t % 3 == 0 && t != 0)
				mode += "|";
		}

		//上次修改时间
		tm stdT;
		time_t time = sub_inode.i_mtime;
		localtime_s(&stdT, (const time_t*)&time);

		//用户名
		string uname;
		File* user;
		user = fopen("/etc/users");
		char* data = new char[user->f_inode->i_size + 1]{ 0 };
		fread(data, user->f_inode->i_size, user);
		string fsrd = data;
		delete[] data;
		fclose(user);
		vector<string> users;
		users = splitstr(fsrd, "\n");
		for (int i = 0; i < users.size(); i++) {
			vector<string>umsg = splitstr(users[i], "-");
			if (stoi(umsg[2]) == sub_inode.i_uid) {
				uname = umsg[0];
				break;
			}
		}

		//用户组名
		string gname;
		File* group;
		group = fopen("/etc/groups");
		char* gdata = new char[group->f_inode->i_size + 1]{ 0 };
		fread(gdata, group->f_inode->i_size, group);
		string gstrdata = gdata;
		delete[] gdata;
		fclose(group);
		vector<string> groups;
		groups = splitstr(gstrdata, "\n");
		for (int j = 0; j < groups.size(); j++) {
			vector<string>gmsg = splitstr(groups[j], "-");
			if (stoi(gmsg[1]) == sub_inode.i_gid) {
				gname = gmsg[0];
				break;
			}
		}

		//输出信息
		char buf[128];
		sprintf_s(buf, "%10s    %-8d  %-28s  %-7s  %-7s  %d B  \t%d.%d.%d %02d:%02d:%02d\n", mode.data(), sub_inode.i_nlink, dir_list[i].m_name, uname.data(), gname.data(), sub_inode.i_size, 1900 + stdT.tm_year, stdT.tm_mon + 1, stdT.tm_mday, stdT.tm_hour, stdT.tm_min, stdT.tm_sec);
		ret_list.push_back(string(buf));
	}

	delete[] dir_list;
	chmod("/etc/users", 0000);	//关闭权限
	chmod("/etc/groups", 0000);	//关闭权限

	return ret_list;
}
