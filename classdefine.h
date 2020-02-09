#include "define.h"
#include<bitset>
using namespace std;
struct superblock
{
	int size;					//�������̵Ĵ�С
	int freeBlock[STACKSIZE];	//�̿�ջ
	int nextFreeBlock;		//�̿�ջ��ʣ��Ŀ���
	int freeBlockNum;			//����ϵͳ��ʣ����̿�����
	bitset<TOTALINODENUM>inodestack;  //λͼ����ʾinode��ʹ�����
	int freeInodeNum;			//����ϵͳ��ʣ���inode����
};
struct diskinode
{
	int mode;//�ļ�ģʽ���������ͺ�Ȩ��
	long filesize;//�ļ���С
	int filelink;//�ļ�������
	long modifytime;//�������ʱ��
	int addr[6];
	char owner[MAXOWNERLENGTH];
	char group[MAXGROUPLENGTH];
};
struct direct
{
	char directname[MAXDIRECTNAME];
	short inodeID;
};
struct dir {
	direct direct[DIRECTNUM];
};

struct inode
{
	struct	diskinode diskinode;
	short inodeID;				//the node id
};