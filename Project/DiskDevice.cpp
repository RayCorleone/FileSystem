/*
	@name:      DiskDevice.cpp
	@function:	DiskDevice �ඨ��
	@author:    Ray
	@version:   1.0
	@problem:	None
	@timestamp: 2022/05/10
	@notice:	None
*/

#include "Define.h"

using namespace std;

// DiskDevice���캯��
DiskDevice::DiskDevice() {
	this->disk_name[0] = '\0';
	this->bm = NULL;

	bDiskList.b_blkno = -1;
	bDiskList.b_addr = NULL;
	bDiskList.b_flags = BufFlag::B_NONE;
	bDiskList.b_forw = &bDiskList;
	bDiskList.b_back = &bDiskList;
	bDiskList.av_forw = &bDiskList;
	bDiskList.av_back = &bDiskList;
}

// DiskDevice��������
DiskDevice::~DiskDevice() {
	//Nothing to do
}

// ���ô��̲���
void DiskDevice::SetDisk(const char* name, BufferManager* bm) {
	strcpy_s(this->disk_name, name);
	this->bm = bm;

	bDiskList.b_blkno = -1;
	bDiskList.b_addr = NULL;
	bDiskList.b_flags = BufFlag::B_NONE;
	bDiskList.b_forw = &bDiskList;
	bDiskList.b_back = &bDiskList;
	bDiskList.av_forw = &bDiskList;
	bDiskList.av_back = &bDiskList;
}

// ͨ�������ȡ����
void DiskDevice::Dread(char* buf, unsigned int start_addr, unsigned int length) {
	// buf Ϊ��: û�д���Ķ���
	if (buf == NULL)
		return;

	// λ�ü���������: ��������ȡ��
	unsigned int pos = 0;
	unsigned int start_blkno = start_addr / BUFFER_SIZE;
	unsigned int end_blkno = (start_addr + length - 1) / BUFFER_SIZE;

	// ѭ��д�� buf ����: æ��
	for (unsigned int blkno = start_blkno; blkno <= end_blkno; blkno++) {
		// blkno����뻺��
		Buf* bp = bm->Bread(blkno);

		// һ���ڶ���
		if (blkno == start_blkno && blkno == end_blkno) {
			memcpy_s(buf + pos, length, bp->b_addr + start_addr % BUFFER_SIZE, length);
			pos += length;
		}
		// һ�������: ����һ��
		else if (blkno == start_blkno) {
			memcpy_s(buf + pos, (BUFFER_SIZE - 1) - (start_addr % BUFFER_SIZE) + 1, bp->b_addr + start_addr % BUFFER_SIZE, (BUFFER_SIZE - 1) - (start_addr % BUFFER_SIZE) + 1);
			pos += BUFFER_SIZE - (start_addr % BUFFER_SIZE);
		}
		// һ�������: �����һ��
		else if (blkno == end_blkno) {
			memcpy_s(buf + pos, (start_addr + length - 1) % BUFFER_SIZE - 0 + 1, bp->b_addr, (start_addr + length - 1) % BUFFER_SIZE - 0 + 1);
			pos += (start_addr + length - 1) % BUFFER_SIZE - 0 + 1;
		}
		// һ�������: ���м��
		else {
			memcpy_s(buf + pos, BUFFER_SIZE, bp->b_addr, BUFFER_SIZE);
			pos += BUFFER_SIZE;
		}
	}
}

// ͨ������д�����(�����ӳ�д)
void DiskDevice::Dwrite(const char* buf, unsigned int start_addr, unsigned int length) {
	//cout << "Dwrite" << endl;
	// λ�ü���������: ��������ȡ��
	unsigned int pos = 0;
	unsigned int start_blkno = start_addr / BUFFER_SIZE;
	unsigned int end_blkno = (start_addr + length - 1) / BUFFER_SIZE;

	// ѭ�����뻺��: æ��
	for (unsigned int blkno = start_blkno; blkno <= end_blkno; blkno++) {
		// blkno����뻺��
		Buf* bp = bm->Bread(blkno);

		// һ����д��
		if (blkno == start_blkno && blkno == end_blkno) {
			memcpy_s(bp->b_addr + start_addr % BUFFER_SIZE, length, buf + pos, length);
			pos += length;
		}
		// һ��д����: д��һ��
		else if (blkno == start_blkno) {
			memcpy_s(bp->b_addr + start_addr % BUFFER_SIZE, (BUFFER_SIZE - 1) - (start_addr % BUFFER_SIZE) + 1, buf + pos, (BUFFER_SIZE - 1) - (start_addr % BUFFER_SIZE) + 1);
			pos += BUFFER_SIZE - (start_addr % BUFFER_SIZE);
		}
		// һ��д����: д���һ��
		else if (blkno == end_blkno) {
			memcpy_s(bp->b_addr, (start_addr + length - 1) % BUFFER_SIZE - 0 + 1, buf + pos, (start_addr + length - 1) % BUFFER_SIZE - 0 + 1);
			pos += (start_addr + length - 1) % BUFFER_SIZE - 0 + 1;
		}
		// һ��д����: д�м��
		else {
			memcpy_s(bp->b_addr, BUFFER_SIZE, buf + pos, BUFFER_SIZE);
			pos += BUFFER_SIZE;
		}

		// �����ӳ�д, ��Ҫʱ��bm->Bread/GetBlk��д�����
		bm->Bdwirte(bp);
	}
}