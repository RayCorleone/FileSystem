/*
	@name:      Define.h
	@function:	�������ࡢͨ�ú�����Ԥ����
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
//@ͨ��ͷ�ļ�
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
//@����
// 0.���û���Ϣ
#define ADMIN		"admin"			//����Ա�û���
#define ADMIN_PSW	"password"		//����Ա�û���
#define ROOT_GROUP	"root"			//����Ա�û�����

// 1.�ļ���Ϣ
#define SMALL_FILE_BLOCK	6					//С���ļ���ֱ������������Ѱַ���߼����
#define LARGE_FILE_BLOCK	128*2+6				//�����ļ�����һ�μ������������Ѱַ���߼����
#define HUGE_FILE_BLOCK		128*128*2+128*2+6	//�����ļ��������μ����������Ѱַ�ļ��߼����

// 2.��С��Ϣ
#define NBUF		15		//�������
#define DIR_SIZE	32		//Ŀ¼���С
#define NAME_SIZE	28		//�ļ�����С
#define FLIST_SIZE	10		//�����ļ�����
#define INODE_SIZE	64		//���Inode��С
#define BLOCK_SIZE 	512		//�߼����С
#define	BUFFER_SIZE 512		//������С

// 3.������Ϣ
#define DISK_NAME		"FileSystem.img"	//�ļ�����
#define DISK_SECTOR		262144 				//������������
#define ROOT_INO		0		//��Ŀ¼Inode��
#define ROOT_DIR_SYMBOL "root"

// 4.������Ϣ
#define SUPER_BLOCK_START_SECTOR	200		//SuperBlock��ʼ������
#define INODE_ZONE_START_SECTOR 	202 	//Inode����ʼ������
#define DATA_ZONE_START_SECTOR		1024	//����������ʼ������

// 5.��ַ��Ϣ
#define SUPER_BLOCK_START_ADDR	BLOCK_SIZE * SUPER_BLOCK_START_SECTOR	//SuperBlock��ʼ��ַ
#define INODE_ZONE_START_ADDR	BLOCK_SIZE * INODE_ZONE_START_SECTOR	//Inode����ʼ��ַ
#define DATA_ZONE_START_ADDR	BLOCK_SIZE * DATA_ZONE_START_SECTOR		//��������ʼ��ַ

// 6.������Ϣ
#define MAX_DISK_SIZE	BLOCK_SIZE * DISK_SECTOR		//��������С
#define MAX_FILE_SIZE	BLOCK_SIZE * HUGE_FILE_BLOCK	//�ļ�����С

// 7.���������Ϣ
#define DIR_NUMBER_PER_SECTOR	BLOCK_SIZE / DIR_SIZE		//�����̿�Ŀ¼����
#define INODE_NUMBER_PER_SECTOR BLOCK_SIZE / INODE_SIZE		//�����̿����Inode����
#define	INODE_SECTOR_TOTAL		DATA_ZONE_START_SECTOR - INODE_ZONE_START_SECTOR	//Inode�����̿���
#define	DATA_SECTOR_TOTAL		DISK_SECTOR - DATA_ZONE_START_SECTOR				//���������̿���

// 8.�ļ�״̬��־(Inode & DiskInode's mode)
#define	IALLOC	1 << 15		//15	�Ѿ���ʹ��
#define	IFCHR	1 << 13		//14-13 �ַ��豸
#define	IFDIR	2 << 13		//14-13 Ŀ¼�ļ�
#define IFMT	3 << 13		//14-13	���豸
#define ILARG	1 << 12		//12	���ͻ�����ļ�
#define IREAD	4 << 6		//8		���ļ��Ķ�Ȩ��
#define IWRITE	2 << 6		//7		���ļ���дȨ��
#define IEXEC	1 << 6		//6		���ļ���ִ��Ȩ��
#define IRWXU	(IREAD|IWRITE|IEXEC)	//8-6	�ļ�����Ȩ��
#define IRWXG	((IREAD|IWRITE)>>3)		//5-3	ͬ���û���Ȩ��
#define IRWXO	((IREAD)>>6)			//2-0	�����û���Ȩ��
#define PFILE	(IREAD|IWRITE|IRWXG|IRWXO)		//�ļ���Ȩ��
#define PDIR	(IRWXU|IRWXG|IRWXO)				//Ŀ¼��Ȩ��
#define RDIR	(IRWXU|(IRWXU>>3)|(IRWXU>>6))	//��Ŀ¼��Ȩ��

//###########################################
//@ö�ٶ���
enum BufFlag {	// b_flag�еı�־λ
	B_NONE	= 0x0,		//�ޱ�־
	B_WRITE = 0x1,		//(x)д����, ��������Ϣд��Ӳ��
	B_READ	= 0x2,		//(x)������, ���̶�ȡ��Ϣ������
	B_DONE	= 0x4,		//��д���
	B_ERROR = 0x8,		//(x)I/O��������ֹ
	B_BUSY	= 0x10,		//(x)��Ӧ��������ʹ����
	B_WANTED= 0x20,		//(x)�н��̵ȴ�ʹ�ø�buf����Ļ���
	B_ASYNC = 0x40,		//(x)�첽I/O, ����Ҫ�ȴ������
	B_DELWRI= 0x80		//�ӳ�д, ����Ӧ����Ҫ��������ʱ���ٽ�������д����Ӧ���豸��
};

//###########################################
//@������
class Buf;				//�����
class File;				//�ļ�
class Inode;			//�ڴ������ڵ�
class DiskInode;		//��������ڵ�(��Inode��Ӧ)
class SuperBlock;		//�洢��Դ�����
class DiskDevice;		//�����豸
class FileSystem;		//�ļ�ϵͳ
class BufferManager;	//�������
class UserInterface;	//�û�����
class DirectoryEntry;	//Ŀ¼���

//###########################################
//@��дͷ�ļ�
#include "File.h"
#include "Inode.h"
#include "Buffer.h"
#include "DiskDevice.h"
#include "SuperBlock.h"
#include "FileSystem.h"
#include "UserInterface.h"

//###########################################
//@������������
using namespace std;

int get_fsize_from_windows(const char* fname);			//��window��ȡ�ļ���С
char* read_from_windows(const char* fname);				//��window��ȡ�ļ�����

string wstring2string(wstring wstr);					//��ǰ��������ת��
vector<string> splitstr(string str, string pattern);	//�����ֶηָ�

#endif // !DEFINE_H
