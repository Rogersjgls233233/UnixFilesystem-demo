#include "define.h"
#include<bitset>
using namespace std;
struct superblock
{
	int size;					//整个磁盘的大小
	int freeBlock[STACKSIZE];	//盘块栈
	int nextFreeBlock;		//盘块栈中剩余的块数
	int freeBlockNum;			//整个系统中剩余的盘块数量
	bitset<TOTALINODENUM>inodestack;  //位图法表示inode的使用情况
	int freeInodeNum;			//整个系统中剩余的inode数量
};
struct diskinode
{
	int mode;//文件模式，包括类型和权限
	long filesize;//文件大小
	int filelink;//文件链接数
	long modifytime;//最近更改时间
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