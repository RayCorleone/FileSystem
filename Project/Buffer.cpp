/*
	@name:      Buffer.cpp
	@function:	Buf & BufferManager �ඨ��
	@author:    Ray
	@version:   1.0
	@problem:	None
	@timestamp: 2022/05/10
	@notice:	None
*/

#include "Define.h"

using namespace std;

// BufferManager���캯��
BufferManager::BufferManager() {
	this->disk = NULL;
	memset(Buffer, 0, sizeof(Buffer));

	bFreeList.b_blkno = -1;
	bFreeList.b_addr = NULL;
	bFreeList.b_flags = BufFlag::B_NONE;
	bFreeList.b_forw = &bFreeList;
	bFreeList.b_back = &bFreeList;
	bFreeList.av_forw = &bFreeList;
	bFreeList.av_back = &bFreeList;

	Buf* bp;
	for (int i = 0; i < NBUF; i++){
		bp = &(this->m_Buf[i]);

		bp->b_blkno = -1;
		bp->b_addr = Buffer[i];
		bp->b_flags = BufFlag::B_NONE;

		//��ʼ�����ɶ���
		Brelse(bp);
	}
}

// BufferManager��������
BufferManager::~BufferManager() {
	for (int i = 0; i < NBUF; i++)
		Brelse(&m_Buf[i]);
	memset(Buffer, 0, sizeof(Buffer));
}

// ��������ʼ��: ������ƿ���еĳ�ʼ��,ͬʱ��������豸
void BufferManager::Initialize(DiskDevice* disk) {
	this->disk = disk;
	memset(Buffer, 0, sizeof(Buffer));

	bFreeList.b_blkno = -1;
	bFreeList.b_addr = NULL;
	bFreeList.b_flags = BufFlag::B_NONE;
	bFreeList.b_forw = &bFreeList;
	bFreeList.b_back = &bFreeList;
	bFreeList.av_forw = &bFreeList;
	bFreeList.av_back = &bFreeList;

	Buf* bp;
	for (int i = 0; i < NBUF; i++) {
		bp = &(this->m_Buf[i]);

		bp->b_blkno = -1;
		bp->b_addr = Buffer[i];
		bp->b_flags = BufFlag::B_NONE;

		//��ʼ�����ɶ���
		Brelse(bp);
	}
}

// ���뻺���
Buf* BufferManager::GetBlk(unsigned int blkno) {
	Buf* bp = NULL;

	// 1.�����豸�Ĺ��ض�������֮ǰ��û�� Buf �����ÿ�
	for (bp = disk->bDiskList.b_back; bp != &(disk->bDiskList); bp = bp->b_back) {
		if (bp->b_blkno == blkno)
			return bp;
	}
	// 2.���ɶ��п�
	if (bFreeList.av_back == &bFreeList) {
		// ʹ��æ��, I/O��������������һ�����棬���ɶ��в�����Ϊ��
		throw("-!ERROR: �������ɶ���Ϊ��, BufferManager��ʼ������.\n");
	}
	// 3.�����ɶ���ȡһ�������
	else {
		//(1)�����ɶ���ȡ��
		bp = bFreeList.av_back;
		bp->av_forw->av_back = bp->av_back;
		bp->av_back->av_forw = bp->av_forw;
		bp->b_forw->b_back = bp->b_back;
		bp->b_back->b_forw = bp->b_forw;

		//(2)�����ӳ�д
		if ((bp->b_flags & BufFlag::B_DELWRI) == BufFlag::B_DELWRI){
			Bwrite(bp);		//����д�����
			bp->b_flags = bp->b_flags & (~BufFlag::B_DELWRI);	//����ӳ�д��־
		}

		//(3)������б�־
		bp->b_flags = BufFlag::B_NONE;

		//(4)��дblkno, �����豸���ж�ͷ
		bp->b_blkno = blkno;
		bp->b_forw = &(disk->bDiskList);
		bp->b_back = disk->bDiskList.b_back;
		bp->b_forw->b_back = bp;
		bp->b_back->b_forw = bp;
	}
	return bp;
}

// �ͷŻ����
void BufferManager::Brelse(Buf* bp) {
	//���ӳ�д��־��,  �ͷ�ʱд�����
	if ((bp->b_flags & BufFlag::B_DELWRI) == BufFlag::B_DELWRI) {
		Bwrite(bp);		//����д�����
		bp->b_flags = bp->b_flags & (~BufFlag::B_DELWRI);	//����ӳ�д��־
	}

	//������б�־
	bp->b_flags = BufFlag::B_NONE;

	//�����������ɶ���
	bp->av_forw = bFreeList.av_forw;
	bp->av_back = &bFreeList;
	bp->av_forw->av_back = bp;
	bp->av_back->av_forw = bp;

	//NODEV��������ά��
	bp->b_forw = &bFreeList;
	bp->b_back = &bFreeList;
}

// ��һ���̿�
Buf* BufferManager::Bread(unsigned int blkno){
	//��ȡһ��blkno�̿黺��
	Buf* bp = GetBlk(blkno);

	//������Ѿ�������,����ֱ��ʹ��
	if ((bp->b_flags & BufFlag::B_DONE) == BufFlag::B_DONE)
		return bp;

	//����鲻����ֱ��ʹ��: æ�ȶ�ȡ
	else {
		//I/O������������I/O�������
		bp->av_forw = disk->bDiskList.av_forw;
		bp->av_back = &(disk->bDiskList);
		bp->av_forw->av_back = bp;
		bp->av_back->av_forw = bp;

		//��ʼ������
		fstream fin;
		fin.open(disk->disk_name, ios::in | ios::binary);
		if (!fin.is_open())
			throw("-!ERROR: ����ӳ�������ļ�����ʧ��.\n");
		fin.seekg(streampos(blkno) * streampos(BUFFER_SIZE), ios::beg);
		fin.read(bp->b_addr, BUFFER_SIZE);
		fin.close();

		//���±�־
		bp->b_flags = BufFlag::B_DONE;
		
		//��I/O�������ȡ��
		bp->av_forw->av_back = bp->av_back;
		bp->av_back->av_forw = bp->av_forw;

		//���¼������ɶ���
		bp->av_forw = bFreeList.av_forw;
		bp->av_back = &bFreeList;
		bp->av_forw->av_back = bp;
		bp->av_back->av_forw = bp;
		
		return bp;
	}
}

// дһ���̿�
void BufferManager::Bwrite(Buf* bp) {
	//I/Oд����������I/O�������
	bp->av_forw = disk->bDiskList.av_forw;
	bp->av_back = &(disk->bDiskList);
	bp->av_forw->av_back = bp;
	bp->av_back->av_forw = bp;

	//��ʼд����
	fstream fout;
	fout.open(disk->disk_name, ios::in | ios::out | ios::binary);
	if (!fout.is_open())
		throw("-!ERROR: ����ӳ������ļ�����ʧ��.\n");
	fout.seekp(streampos(bp->b_blkno) * streampos(BUFFER_SIZE), ios::beg);
	fout.write((const char*)bp->b_addr, BUFFER_SIZE);
	fout.close();

	//���±�־
	bp->b_flags = BufFlag::B_DONE;

	//��I/O�������ȡ��
	bp->av_forw->av_back = bp->av_back;
	bp->av_back->av_forw = bp->av_forw;

	//�������ɶ���
	bp->av_forw = bFreeList.av_forw;
	bp->av_back = &bFreeList;
	bp->av_forw->av_back = bp;
	bp->av_back->av_forw = bp;
}

// �����ӳ�д��־
void  BufferManager::Bdwirte(Buf* bp){
	bp->b_flags |= BufFlag::B_DELWRI;
}
