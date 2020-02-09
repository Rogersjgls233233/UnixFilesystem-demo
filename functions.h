#pragma once
#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include <string>
#include "classdefine.h"
#include<time.h>
#include<vector>
#include<string>
#include<iomanip>
#include<sstream>
using namespace std;
FILE* virtualDisk;
superblock* super;
bool logout = false;
struct inode* currentinode;
vector<direct>pathname;
struct inode* root;
string userName = "cairongjiu";

void saveSuper()
{
	if (virtualDisk != NULL)
	{
		fseek(virtualDisk, BLOCKSIZE, SEEK_SET);
		fwrite(super, sizeof(superblock), 1, virtualDisk);
	}
}
void saveinodeindisk(inode* inode)
{
	int iposition;
	iposition = 2 * BLOCKSIZE + inode->inodeID * PERINODESIZE;
	fseek(virtualDisk, iposition, SEEK_SET);
	fwrite(&inode->diskinode, sizeof(diskinode), 1, virtualDisk);
}
void getinodefromdisk(inode* inode)
{
	int iposition = 2 * BLOCKSIZE + inode->inodeID * PERINODESIZE;
	struct diskinode* dinode = (struct diskinode*)calloc(1, sizeof(struct diskinode));
	fseek(virtualDisk, iposition, SEEK_SET);
	fread(dinode, sizeof(diskinode), 1, virtualDisk);
	for (int i = 0; i < 6; i++)
	{
		inode->diskinode.addr[i] = dinode->addr[i];
	}
	inode->diskinode.filelink = dinode->filelink;
	inode->diskinode.filesize = dinode->filesize;
	strcpy(inode->diskinode.group,dinode->group);
	strcpy(inode->diskinode.owner, dinode->owner);
	inode->diskinode.mode = dinode->mode; 
	inode->diskinode.modifytime = dinode->modifytime;

}
void freeinodetodisk(inode* inode)
{
	inode->diskinode.filelink--;
	time_t timer;
	time(&timer);
	inode->diskinode.modifytime = timer;
	saveinodeindisk(inode);
	if (inode->diskinode.filelink == 0)
	{
		super->inodestack[inode->inodeID] = 0;
		super->freeInodeNum++;
		saveSuper();

	}
}
inode* iget(int inodeID)
{
	int ipositon = 0;
	if (virtualDisk == NULL)
		return NULL;
	struct inode* tmpinode = (struct inode*)calloc(1, sizeof(struct inode));
	tmpinode->inodeID = inodeID;
	getinodefromdisk(tmpinode);
	time_t timer;
	time(&timer);
	tmpinode->diskinode.modifytime = timer;
	saveinodeindisk(tmpinode);
	return tmpinode;
}

void formatting()
{
	int group[STACKSIZE];
	for (int i = 0; i < STACKSIZE; i++)
	{
		group[i] = i + BLOCKSTART;
	}
	for (int i = 0; i < DATABLOCKNUM/ STACKSIZE-1; i++)
	{		
		for (int j = 0; j < STACKSIZE; j++)
		{
			group[j] += STACKSIZE;
			cout << group[j] << endl;
		}
		fseek(virtualDisk, (BLOCKSTART + (i + 1) * STACKSIZE-1) * BLOCKSIZE, SEEK_SET);
		fwrite(group, sizeof(group), 1, virtualDisk);
		cout << "连接点" << BLOCKSTART + (i + 1) * STACKSIZE - 1 << endl;
		

	}

}


void loadSuper()
{
	virtualDisk = fopen("vm.txt", "rb+");
	if (virtualDisk == NULL) {
		printf("Cannot create the file.");
		exit(-1);
	}
	super = (superblock*)calloc(1, sizeof(superblock));
	fseek(virtualDisk, BLOCKSIZE, SEEK_SET);
	fread(super, sizeof(superblock), 1, virtualDisk);
}

void help()
{
	cout << setiosflags(ios::left) << setw(23) << "命令" << setw(10) << "说明" << endl;
	cout << setiosflags(ios::left) << setw(20) << "ls" << setw(10) << "显示文件目录" << endl;
	cout << setiosflags(ios::left) << setw(20) << "pwd" << setw(10) << "显示当前目录" << endl;
	cout << setiosflags(ios::left) << setw(20) << "cd" << setw(10) << "改变当前目录" << endl;
	cout << setiosflags(ios::left) << setw(20) << "mkdir" << setw(10) << "创建子目录" << endl;
	cout << setiosflags(ios::left) << setw(20) << "touch" << setw(10) << "创建文件" << endl;
	cout << setiosflags(ios::left) << setw(20) << "append" << setw(10) << "编辑文件" << endl;
	cout << setiosflags(ios::left) << setw(20) << "mv" << setw(10) << "改变文件名" << endl;
	cout << setiosflags(ios::left) << setw(20) << "cp" << setw(10) << "文件拷贝" << endl;
	cout << setiosflags(ios::left) << setw(20) << "rm" << setw(10) << "文件删除(文件夹亦可)" << endl;
	cout << setiosflags(ios::left) << setw(20) << "cat" << setw(10) << "连接显示文件内容" << endl;
	cout << setiosflags(ios::left) << setw(20) << "help" << setw(10) << "帮助" << endl;
	cout << setiosflags(ios::left) << setw(20) << "sbinfo" << setw(10) << "查看超级块" << endl;
	cout << setiosflags(ios::left) << setw(20) << "format" << setw(10) << "格式化" << endl;
}
inode* ialloc(){
	int num;
	if (super->freeInodeNum == 0)
	{
		return NULL;
	}
	
	for (int i = 0; i < TOTALINODENUM; i++) {
		if (super->inodestack[i] == 0) {
			super->freeInodeNum--;
			super->inodestack[i] = 1;
			saveSuper();
			num = i;	
			return iget(num);
		}
		
	}

}
void superInfo()
{
	cout << "size" << super->size << endl;
	cout << "freeBlock:" << endl;
	for (int i = 0; i < super->nextFreeBlock; i++)
		cout << super->freeBlock[19-i] << " ";
	cout << endl;
	cout << "freeInode:" << endl;
	for (int i = 0; i < TOTALINODENUM; i++)
	{
		if (super->inodestack[i] == 0)
		{
			cout << i << " ";
		}
	}
	cout << endl;
		
	cout << "nextFreeBlock:" << super->nextFreeBlock << endl;
	cout << "freeBlockNum:" << super->freeBlockNum << endl;
	cout << "freeInodeNum:" << super->freeInodeNum << endl;
	cout << "nextFreeInode:";
	for (int i = 0; i < TOTALINODENUM; i++)
	{
		if (super->inodestack[i] == 0)
		{
			cout << i << endl;
			return;
			
		}
		
	}

}

int balloc() {
	unsigned int bno;
	if (super->nextFreeBlock == 1)
	{
		bno = super->freeBlock[19];

		fseek(virtualDisk, bno * BLOCKSIZE, SEEK_SET);
		fread(super->freeBlock, sizeof(unsigned int), STACKSIZE, virtualDisk);
		super->freeBlockNum--;
		super->nextFreeBlock = 20;
		saveSuper();
		return bno;


	}

	super->freeBlockNum--;
	super->nextFreeBlock--;
	saveSuper();	
	cout << super->freeBlockNum << endl;
	cout << super->nextFreeBlock << endl;
	return super->freeBlock[19-super->nextFreeBlock];
}
void bwrite(void* _Buf, int bno, int offset, int size, int count = 1)
{
	long int pos;
	pos = bno * BLOCKSIZE + offset;
	fseek(virtualDisk, pos, SEEK_SET);
	fwrite(_Buf, size, count, virtualDisk);
	fflush(virtualDisk);
}
int bread(void* _Buf, int bno, int offset, int size, int count = 1)
{
	long int pos;
	int ret;
	pos = bno * BLOCKSIZE + offset;
	fseek(virtualDisk, pos, SEEK_SET);
	ret = fread(_Buf, size, count, virtualDisk);
	if (ret != count)
		return 7;
	return 1;
}
void bfree(int bno)
{
	if (super->nextFreeBlock == 20)
	{
		cout << "把这组写入" << bno << endl;
		fseek(virtualDisk, bno * BLOCKSIZE, SEEK_SET);
		fwrite(super->freeBlock, sizeof(unsigned int), STACKSIZE, virtualDisk);
		super->nextFreeBlock = 0;

		
	}
	super->nextFreeBlock++;
	super->freeBlock[STACKSIZE- super->nextFreeBlock] = bno;
		
	
	super->freeBlockNum++;
	saveSuper();
}

int strPos(char* str, int start, const char needle)
{
	for (int i = start; i < strlen(str); i++)
	{
		if (str[i] == needle)
			return i;
	}
	return -1;
}

int subStr(char* src, char* dst, int start, int end = -1)
{
	int pos = 0;
	end == -1 ? end = strlen(src) : NULL;
	for (int i = start; i < end; i++)
		dst[pos++] = src[i];
	dst[pos] = 0;
	return 1;
}



void writefileblock(inode* inode,string str)
{
	string s;
	int once = 1;
	int twice = 1;
	int start = 0;
	int secondindex;
	if (str == "a0")
	{
		for (int i = 0; i < 200 * BLOCKSIZE; i++)
		{
			s = s + '1';
		}
		s = s + '*';
	}
	else {
		s = str;
	}
	inode->diskinode.filesize = s.length();
	int left= s.length()% BLOCKSIZE;
	int addrnum;
	int k = 0;
	int j = 0;
	if (left == 0)
	{
		addrnum = s.length() / BLOCKSIZE;
	}
	else {
		addrnum = s.length() / BLOCKSIZE + 1;
	}
	if (addrnum+2 > super->freeBlockNum)
	{
		cout << "系统没有足够的空间！" << endl;
		return;
	}
	for (int i = 0; i < addrnum;i++)
	{
		if (i < 4)
		{
			inode->diskinode.addr[i] = balloc();
			fseek(virtualDisk, BLOCKSIZE * inode->diskinode.addr[i], SEEK_SET);
			fwrite(s.substr(start, BLOCKSIZE).c_str(), BLOCKSIZE, 1, virtualDisk);
			cout << "写入直接寻址" << endl;
			start = start + BLOCKSIZE;
		}
		else if (i < 132)
		{
			
			if (once)
			{
				inode->diskinode.addr[4] = balloc();
				cout << "分配给addr4的盘块号为:" << inode->diskinode.addr[4] << endl;
			}
			once = 0;
			int blockid = balloc();
			fseek(virtualDisk, BLOCKSIZE * inode->diskinode.addr[4] + 4 * (i - 4), SEEK_SET);
			fwrite(&blockid, sizeof(int), 1, virtualDisk);
			cout << "分配一级间址盘块号" << blockid << endl;
			fseek(virtualDisk, BLOCKSIZE * blockid, SEEK_SET);
			fwrite(s.substr(start, BLOCKSIZE).c_str(), BLOCKSIZE, 1, virtualDisk);
			cout << "写入一级间址"<< endl;
			start = start + BLOCKSIZE;
		}
		else 
		{
			if (twice)
			{
				inode->diskinode.addr[5] = balloc();
				cout << "分配给addr5的盘块号为:" << inode->diskinode.addr[5] << endl;
			}
			twice = 0;
			if ((i - 4) % 128 == 0)
			{
				int blockid = balloc();
				fseek(virtualDisk, BLOCKSIZE * inode->diskinode.addr[5] + 4 * k, SEEK_SET);
				fwrite(&blockid, sizeof(int), 1, virtualDisk);
				
				secondindex = blockid;
				cout << "二级间址的一级索引盘块号" << secondindex << endl;
				j = i;
				k++;
			}
			int blockid = balloc();
			fseek(virtualDisk, secondindex * BLOCKSIZE + 4 * (i - j), SEEK_SET);
			fwrite(&blockid, sizeof(int), 1, virtualDisk);
			cout << "分配二级间址盘块号" << blockid << endl;
			fseek(virtualDisk, BLOCKSIZE * blockid, SEEK_SET);
			fwrite(s.substr(start, BLOCKSIZE).c_str(), BLOCKSIZE, 1, virtualDisk);
			cout << "写入二级间址" << endl;
			start = start + BLOCKSIZE;

		}
	}

	saveinodeindisk(inode);
}
string readfileblock(inode* inode)
{
	string result;
	int fileSize = inode->diskinode.filesize;
	int addrnum = 0;
	int j;
	int k = 0;
	int left = fileSize % 512;
	cout << "文件大小:"<<fileSize << endl;
	int secondIndex;
	if (left == 0)
	{
		addrnum = fileSize / 512;
	}
	else
	{
		addrnum = fileSize / 512 + 1;
	}
	for (int i = 0; i < addrnum; i++)
	{
		if (i < 4)
		{
			cout << "开始读直接块" << endl;
			char s[512];
			fseek(virtualDisk, inode->diskinode.addr[i] * BLOCKSIZE, SEEK_SET);
			fread(&s, BLOCKSIZE, 1, virtualDisk);
			if (i + 1 == addrnum)
			{
				if (left == 0)
				{
					for (int i = 0; i < 512; i++)
					{
						result = result + s[i];
					}
				}
				else
				{
					for (int i = 0; i < left; i++)
					{
						result = result + s[i];
					}
				}
			}
			else
			{
				for (int i = 0; i < 512; i++)
				{
					result = result + s[i];
				}
			}
		}
		else if (i < 132)
		{
			cout << "开始读一次间址" << endl;
			char s[512];
			int firstIndex;
			fseek(virtualDisk, inode->diskinode.addr[4] * BLOCKSIZE + 4 * (i - 4), SEEK_SET);
			fread(&firstIndex, sizeof(int), 1, virtualDisk);
			fseek(virtualDisk, firstIndex * BLOCKSIZE, SEEK_SET);
			fread(&s, BLOCKSIZE, 1, virtualDisk);
			if (i + 1 == addrnum)
			{
				if (left == 0)
				{
					for (int i = 0; i < 512; i++)
					{
						result = result + s[i];
					}
				}
				else
				{
					for (int i = 0; i < left; i++)
					{
						result = result + s[i];
					}
				}
			}
			else
			{
				for (int i = 0; i < 512; i++)
				{
					result = result + s[i];
				}
			}
		}
		else
		{
			cout<<"开始读二次间址"<<endl;
			char s[512];
			
			if ((i - 4) % 128 == 0)
			{
				fseek(virtualDisk, inode->diskinode.addr[5] * BLOCKSIZE + 4 * k, SEEK_SET);
				fread(&secondIndex, sizeof(int), 1, virtualDisk);

				j = i;
				k++;
			}
			int thirdIndex;
			fseek(virtualDisk, BLOCKSIZE * secondIndex + 4 * (i - j), SEEK_SET);
			fread(&thirdIndex, sizeof(int), 1, virtualDisk);

			fseek(virtualDisk, BLOCKSIZE * thirdIndex, SEEK_SET);

			fread(&s, BLOCKSIZE, 1, virtualDisk);
			if (i + 1 == addrnum)
			{
				if (left == 0)
				{
					for (int i = 0; i < 512; i++)
					{
						result = result + s[i];
					}
				}
				else
				{
					for (int i = 0; i < left; i++)
					{
						result = result + s[i];
					}
				}
			}
			else
			{
				for (int i = 0; i < 512; i++)
				{
					result = result + s[i];
				}
			}
		}
	}
	return result;

}
void freefileblock(inode* inode)
{
	int fileSize = inode->diskinode.filesize;
	int addrnum = 0;
	int intoFirst = 0;
	int intoSecond = 0;
	int j;
	int k = 0;
	int secondIndex;
	vector<int> secondWait;
	if (fileSize % 512 == 0) {
		addrnum = fileSize / 512;
	}
	else {
		addrnum = fileSize / 512 + 1;
	}
	for (int i = 0; i < addrnum; i++) {
		if (i < 4) {
			bfree(inode->diskinode.addr[i]);
			cout << "释放直接盘块号" << inode->diskinode.addr[i] << endl;
		}
		else if (i < 132) {
			intoFirst = 1;
			int firstIndex;
			fseek(virtualDisk, inode->diskinode.addr[4] * BLOCKSIZE + 4 * (i - 4), SEEK_SET);
			fread(&firstIndex, 4, 1, virtualDisk);
			bfree(firstIndex);
			cout << "释放一级间址盘块号" << firstIndex << endl;
		}
		else
		{
			intoSecond = 1;
			
			if ((i - 4) % 128 == 0) {
				fseek(virtualDisk, inode->diskinode.addr[5] * BLOCKSIZE + 4 * k, SEEK_SET);
				fread(&secondIndex, 4, 1, virtualDisk);
				secondWait.push_back(secondIndex);
				j = i;
				k++;
			}
			int thirdIndex;
			fseek(virtualDisk, BLOCKSIZE * secondIndex + 4 * (i - j), SEEK_SET);
			fread(&thirdIndex, 4, 1, virtualDisk);
			bfree(thirdIndex);
			cout << "释放二级间址盘块号" << thirdIndex << endl;
		}
	}
	if (intoFirst) {
		bfree(inode->diskinode.addr[4]);
		cout << "释放addr4   " << inode->diskinode.addr[4] << endl;
	}
	if (intoSecond) {
		cout << "释放addr5   " << inode->diskinode.addr[5] << endl;
		bfree(inode->diskinode.addr[5]);
		
		for (int i = 0; i < secondWait.size(); i++) {
			bfree(secondWait[i]);
			cout << "释放二级盘块的索引块" << secondWait[i] << endl;
			
		}
	}
}
void touch(char* filename,inode *inode)
{
	if (inode->diskinode.mode / 1000 != 1)
	{
		cout << "this is not a dir" << endl;
		return;
	}
	struct dir* dir = (struct dir*)calloc(1, sizeof(struct dir));
	bread(dir, inode->diskinode.addr[0], 0, sizeof(struct dir));
	for (int i = 2; i < DIRECTNUM; i++)
	{
		if (strcmp(dir->direct[i].directname, filename) == 0)
		{
			cout << "this is an existed file!" << endl;
			return;
		}
	}
	int i;
	for (i = 0; i < DIRECTNUM; i++)
	{
		if (dir->direct[i].inodeID == -1)break;
	}
	if (i == DIRECTNUM)
	{
		cout << "Can't make more file!" << endl;
	}
	inode->diskinode.filesize += sizeof(direct);
	saveinodeindisk(inode);

	strcpy(dir->direct[i].directname, filename);

	struct inode* tmpinode = ialloc();
	tmpinode->diskinode.mode = 2777;
	tmpinode->diskinode.filelink=1;
	tmpinode->diskinode.filesize = 0;
	saveinodeindisk(tmpinode);
	dir->direct[i].inodeID = tmpinode->inodeID;
	bwrite(dir, inode->diskinode.addr[0], 0, sizeof(struct dir));
}
void mkdir(char* dirname)
{
	if (currentinode->diskinode.mode / 1000 != 1)
	{
		cout << "this is not a dir" << endl;
		return;
	}

	struct dir* dir = (struct dir*)calloc(1, sizeof(struct dir));
	bread(dir, currentinode->diskinode.addr[0], 0, sizeof(struct dir));	
	for (int i = 2; i < DIRECTNUM; i++)
	{
		if (strcmp(dir->direct[i].directname, dirname) == 0)
		{
			cout << "已存在同名目录!" << endl;
			return;
		}
	}

	int i;
	for (i = 0; i < DIRECTNUM; i++)
	{
		if (dir->direct[i].inodeID == -1)break;
	}
	if (i == DIRECTNUM||super->freeBlockNum==0)
	{
		cout << "不可以再创建更多目录!" << endl;
		return;
	}

	currentinode->diskinode.filesize += sizeof(direct);
	
	saveinodeindisk(currentinode);

	strcpy(dir->direct[i].directname, dirname);

	struct inode* tmpinode = ialloc();

	tmpinode->diskinode.addr[0] = balloc();
	tmpinode->diskinode.mode = 1777;
	tmpinode->diskinode.filelink=1;
	saveinodeindisk(tmpinode);
	

	dir->direct[i].inodeID = tmpinode->inodeID;
	bwrite(dir, currentinode->diskinode.addr[0], 0, sizeof(struct dir));
	struct dir* dir1 = (struct dir*)calloc(1, sizeof(struct dir));	
	for (int i = 0; i < DIRECTNUM; i++)
	{

		dir1->direct[i].inodeID = -1;
		strcpy(dir1->direct[i].directname, "");
	}
	char name[MAXDIRECTNAME] = { 0 };
	strcpy(name, ".");
	strcpy(dir1->direct[1].directname, name);
	dir1->direct[1].inodeID = tmpinode->inodeID;
	char fathername[MAXDIRECTNAME] = { 0 };
	strcpy(fathername, "..");
	strcpy(dir1->direct[0].directname, fathername);
	dir1->direct[0].inodeID = currentinode->inodeID;
	bwrite(dir1, tmpinode->diskinode.addr[0], 0, sizeof(struct dir));
}

inode* cd(char* path, inode* inode)
{

	if (path[0] == '/')
	{
		currentinode = root;
		inode = root;
		int lenth = pathname.size();

		for (int i = 0; i < lenth - 1; i++)
		{
			pathname.pop_back();
		}
		
		if (strlen(path) == 1)
		{
			return inode;
		}
	}
	
	char tmp[16] = { 0 };
	int start = (path[0] == '/');
	int pos = strPos(path, 1, '/');
	subStr(path, tmp, start, pos);
	int type = inode->diskinode.mode / 1000;
	if (type == 1)
	{
		int count = 0;
		struct dir* dir = (struct dir*)calloc(1, sizeof(struct dir));
		bread(dir, inode->diskinode.addr[0], 0, sizeof(struct dir));

		for (int i = 0; i <DIRECTNUM; i++)		
		{
			if (strcmp(dir->direct[i].directname, tmp) == 0&& dir->direct[i].inodeID!=-1)
			{
				if (i == 0)
				{
					if (pathname.size() > 1)
					{
						pathname.pop_back();
					}
				}
				else if (i == 1)
				{

				}
				else
				{
					direct currentdirect;
					currentdirect.inodeID = dir->direct[i].inodeID;
					strcpy(currentdirect.directname, tmp);
					pathname.push_back(currentdirect);
				}
				count = -1;
				struct inode* tmpnode = iget(dir->direct[i].inodeID);
			
				inode=tmpnode;
				break;
			}
		}
		if (count != -1)
		{
			inode = NULL;
			return inode;
		}
	}
	else
	{	
		
		inode = NULL;
		return inode;
	}
	if (pos != -1 && inode != NULL)
	{
		subStr(path, tmp, pos + 1);
		return cd(tmp, inode);
	}
	if (pos == -1)
		return inode;
}

void rm(char* path, inode* inode)
{
	if (strcmp(path, ".") == 0 || strcmp(path, "..") == 0)
	{
		cout << "非法操作" << endl;
		return;
	}
	dir* selfdir = (struct dir*)calloc(1, sizeof(struct dir));
	int flag = 0;
	bread(selfdir, inode->diskinode.addr[0], 0, sizeof(struct dir));
	for (int i = 0; i < DIRECTNUM; i++)
	{
		if (strcmp(selfdir->direct[i].directname, path) == 0&& selfdir->direct[i].inodeID!=-1)
		{
			flag = 1;
		}
	}
	if (flag == 0) {
		cout << "找不到指定文件!" << endl;
		return;
	}
	char rmpath[14];
	struct inode* rminode = cd(path, inode);
	if (rminode != NULL)
	{
		pathname.pop_back();
		if (rminode->diskinode.mode / 1000 == 1)
		{
			struct dir* dir = (struct dir*)calloc(1, sizeof(struct dir));
			bread(dir, rminode->diskinode.addr[0], 0, sizeof(struct dir));
			for (int i = 2; i < DIRECTNUM; i++)
			{
				if (dir->direct[i].inodeID != -1)
				{
					strcpy(rmpath, dir->direct[i].directname);
					rm(rmpath, rminode);
				}
			}
			
			bfree(rminode->diskinode.addr[0]);
			freeinodetodisk(rminode);
			struct dir* dir1 = (struct dir*)calloc(1, sizeof(struct dir));
			bread(dir1, inode->diskinode.addr[0], 0, sizeof(struct dir));
			for (int i = 2; i < DIRECTNUM; i++)
			{
				if (strcmp(dir1->direct[i].directname, path) == 0)
				{
					dir1->direct[i].inodeID = -1;
					strcpy(dir1->direct[i].directname, "");
				}
			}
			bwrite(dir1, inode->diskinode.addr[0], 0, sizeof(struct dir));
		}
		else
		{
			freefileblock(rminode);
			freeinodetodisk(rminode);
			struct dir* dir1 = (struct dir*)calloc(1, sizeof(struct dir));
			bread(dir1, inode->diskinode.addr[0], 0, sizeof(struct dir));
			for (int i = 2; i < DIRECTNUM; i++)
			{
				if (strcmp(dir1->direct[i].directname, path) == 0)
				{
					dir1->direct[i].inodeID = -1;
					strcpy(dir1->direct[i].directname, "");
				}
			}
			bwrite(dir1, inode->diskinode.addr[0], 0, sizeof(struct dir));
			

		}
	}
	else {
		cout << "找不到指定文件!" << endl;
	}

	



}

void append(char* path)
{
	dir* selfdir = (struct dir*)calloc(1, sizeof(struct dir));
	int flag = 0;
	bread(selfdir, currentinode->diskinode.addr[0], 0, sizeof(struct dir));
	for (int i = 0; i < DIRECTNUM; i++)
	{
		if (strcmp(selfdir->direct[i].directname, path) == 0 && selfdir->direct[i].inodeID != -1)
		{
			flag = 1;
		}
	}
	if (flag == 0) {
		cout << "找不到指定文件!" << endl;
		return;
	}
	else
	{
		struct inode* appinode = cd(path, currentinode);
		
		if (appinode != NULL)
		{
			pathname.pop_back();
			if (appinode->diskinode.mode / 1000 == 1)
			{
				cout << "这不是一个有效文件！" << endl;
				return;
			}
			else
			{
				string s = "a0";
				writefileblock(appinode,s);
			}
		}
	}
}
void ls() {
	int type = currentinode->diskinode.mode / 1000;
	if (type != 1)
	{
		cout << "this is not a dir" << endl;
		return;
	}
	int count = currentinode->diskinode.filesize / sizeof(struct direct);
	dir* dir = (struct dir*)calloc(1, sizeof(struct dir));
	bread(dir, currentinode->diskinode.addr[0], 0, sizeof(struct dir));
	for (int i = 0; i < DIRECTNUM; i++)
	{
		if (dir->direct[i].inodeID != -1)
		{
			puts(dir->direct[i].directname);
		}
	}
		
}
void reset()
{
	formatting();
	fseek(virtualDisk, 512, SEEK_SET);
	super->freeBlockNum = DATABLOCKNUM;
	super->freeInodeNum = TOTALINODENUM;
	super->size = (INODEBLOCKNUM + DATABLOCKNUM + 2) * BLOCKSIZE;
	super->nextFreeBlock = STACKSIZE;
	for (int i = 0; i < TOTALINODENUM; i++)
	{
		super->inodestack[i] = 0;
	}
	for (int i = 0; i < STACKSIZE; i++)
	{
		super->freeBlock[i] = BLOCKSTART+i;
	}
	saveSuper();

	root = (struct inode*)calloc(1, sizeof(struct inode));
	root = ialloc();
	root->diskinode.addr[0] = balloc();
	root->diskinode.mode = 1777; 
	saveinodeindisk(root);
	currentinode = root;
	dir* dir = (struct dir*)calloc(1, sizeof(struct dir));
	bread(dir, currentinode->diskinode.addr[0], 0, sizeof(struct dir));
	for (int i = 0; i < DIRECTNUM; i++)
	{
		dir->direct[i].inodeID = -1;
		strcpy(dir->direct[i].directname, "");
	}
	
	char name[MAXDIRECTNAME] = { 0 };
	strcpy(name, ".");
	strcpy(dir->direct[1].directname, name);
	dir->direct[1].inodeID = currentinode->inodeID;
	pathname.clear();
	char fathername[MAXDIRECTNAME] = { 0 };
	strcpy(fathername, "..");
	strcpy(dir->direct[0].directname,fathername);
	dir->direct[0].inodeID = currentinode->inodeID;
	direct currentdirect;
	currentdirect.inodeID = currentinode->inodeID;
	strcpy(currentdirect.directname, "/");
	pathname.push_back(currentdirect);
	bwrite(dir, currentinode->diskinode.addr[0], 0, sizeof(struct dir));
	char makename[14];
	strcpy(makename, "root");
	mkdir(makename);
	currentinode = cd(makename, root);
}
void pwd()
{
	for (int i = 0; i < pathname.size(); i++)
	{
		if ((i == pathname.size() - 1) || i == 0)
		{
			cout << pathname[i].directname;
		}
		else
		{
			cout << pathname[i].directname << '/';
		}
	}
}


inode* returninode;
void changedirect(char* path)
{

	char temp1[] = "/";
	if (strcmp(path, temp1) == 0)
	{
		returninode = iget(0);
		pathname.clear();
		direct tmpdirect;
		tmpdirect.inodeID = 0;
		strcpy(tmpdirect.directname, "/");
		pathname.push_back(tmpdirect);
		return;
	}
	else
	{
		if (path[0] == '/')
		{
			returninode = iget(0);
			pathname.clear();
			direct tmpdirect;
			tmpdirect.inodeID = 0;
			strcpy(tmpdirect.directname, "/");
			pathname.push_back(tmpdirect);
		}
		if (strchr(path, '/') != NULL)
		{
			char* ptr;
			ptr = strtok(path, "/");
			while (ptr != NULL)
			{
				changedirect(ptr);
				ptr = strtok(NULL, "/");
			}
			return;
		}
		else {
			if (returninode != NULL)
			{
				if (returninode->diskinode.mode / 1000 == 1)
				{
					dir* selfdir = (struct dir*)calloc(1, sizeof(struct dir));
					int flag = 0;
					bread(selfdir, returninode->diskinode.addr[0], 0, sizeof(struct dir));
					for (int i = 0; i < DIRECTNUM; i++)
					{
						if (strcmp(selfdir->direct[i].directname, path) == 0 && selfdir->direct[i].inodeID != -1)
						{
							flag = 1;
							struct inode *newinode= (struct inode*)calloc(1, sizeof(struct inode));
							newinode = cd(path, returninode);
							returninode = newinode;
							return;
						}
					}
					if (flag == 0)
					{
						returninode = NULL;
						return;
					}
				}
				else {
					return;
				}
			}
			else
			{
				return;
			}
		}
	}
}
void cp(char *fileName, char *newPath)
{

	dir* selfdir = (struct dir*)calloc(1, sizeof(struct dir));
	int flag = 0;
	int flag2 = 0;
	bread(selfdir, currentinode->diskinode.addr[0], 0, sizeof(struct dir));
	for (int i = 0; i < DIRECTNUM; i++)
	{
		if (strcmp(selfdir->direct[i].directname, fileName) == 0 && selfdir->direct[i].inodeID != -1)
		{
			flag = 1;
		}
	}
	if (flag == 0) {
		cout << "找不到指定文件!" << endl;
		return;
	}
	else
	{
		struct inode* appinode = cd(fileName, currentinode);

		if (appinode != NULL)
		{
			pathname.pop_back();
			if (appinode->diskinode.mode / 1000 == 1)
			{
				cout << "要复制的不是一个有效文件！" << endl;
				return;
			}
			else
			{
				vector<direct>tmppath;
				tmppath = pathname;
				returninode = (struct inode*)calloc(1, sizeof(struct inode));
				returninode->inodeID = currentinode->inodeID;
				returninode = iget(returninode->inodeID);
				changedirect(newPath);
				if (returninode != NULL)
				{
					if (returninode->diskinode.mode / 1000 == 1)
					{
						dir* aimdir = (struct dir*)calloc(1, sizeof(struct dir));
						bread(aimdir, returninode->diskinode.addr[0], 0, sizeof(struct dir));
						for (int i = 0; i < DIRECTNUM; i++)
						{
							if (strcmp(aimdir->direct[i].directname, fileName) == 0 && selfdir->direct[i].inodeID != -1)
							{
								flag2 = 1;
							}

						}
						if (flag2 == 1) {
							cout << "目标目录已存在同名文件，拒绝操作！" << endl;
							pathname = tmppath;
							return;
						}
						else
						{
							touch(fileName, returninode);
							struct inode* aiminode = cd(fileName, returninode);
							string ss = readfileblock(appinode);
							writefileblock(aiminode, ss);
							pathname = tmppath;
						}

					}
					else {
						pathname = tmppath;
						cout << "不是有效路径" << endl;
					}

				}
				else
				{
					pathname = tmppath;
					cout << "系统找不到指定路径" << endl;
				}



			}
		}
	}


}
void cat(char* fileName)
{
	dir* selfdir = (struct dir*)calloc(1, sizeof(struct dir));
	int flag = 0;
	bread(selfdir, currentinode->diskinode.addr[0], 0, sizeof(struct dir));
	for (int i = 0; i < DIRECTNUM; i++)
	{
		if (strcmp(selfdir->direct[i].directname, fileName) == 0 && selfdir->direct[i].inodeID != -1)
		{
			flag = 1;
		}
	}
	if (flag == 0) {
		cout << "找不到指定文件!" << endl;
		return;
	}
	else
	{
		struct inode* appinode = cd(fileName, currentinode);
		if (appinode != NULL)
		{
			pathname.pop_back();
			if (appinode->diskinode.mode / 1000 == 1)
			{
				cout << "不是一个有效文件！" << endl;
				return;
			}
			else
			{
				string ss = readfileblock(appinode);
				cout << ss << endl;
			}
		}
	}
}

void mv(char* filename, char* newname)
{
	dir* selfdir = (struct dir*)calloc(1, sizeof(struct dir));
	int flag = 0;
	int flag2 = 0;
	int j = 0;
	bread(selfdir, currentinode->diskinode.addr[0], 0, sizeof(struct dir));
	for (int i = 0; i < DIRECTNUM; i++)
	{
		if (strcmp(selfdir->direct[i].directname, filename) == 0 && selfdir->direct[i].inodeID != -1)
		{
			flag = 1;
			j = i;
		}
	}
	if (flag == 0) {
		cout << "找不到指定文件!" << endl;
		return;
	}
	else
	{
		for (int i = 0; i < DIRECTNUM; i++)
		{
			if (strcmp(selfdir->direct[i].directname, newname) == 0 && selfdir->direct[i].inodeID != -1)
			{
				flag2 = 1;
			}
		}
		if (flag2 == 1)
		{
			cout << "已存在相同文件名的文件!" << endl;
		}
		else {
			strcpy(selfdir->direct[j].directname, newname);
			bwrite(selfdir, currentinode->diskinode.addr[0], 0, sizeof(struct dir));
		}
	}
}

void doit()
{
	cout << "[" << userName << "@localhost";
	for (int i = 0; i < pathname.size(); i++)
	{
		if ((i == pathname.size() - 1) || i == 0)
		{
			cout << pathname[i].directname;
		}
		else
		{
			cout << pathname[i].directname << '/';
		}
	}
	cout << "]" << '#';
	char shell1[100];
	char command[100];
	char shell2[100];
	int i = 0;
	string line,out;
	getline(cin, line);
	stringstream ss(line);
	while (ss >> out)
	{
		if (i == 0)
		{
			strcpy(command, out.c_str());
		}
		if (i == 1)
		{
			strcpy(shell1, out.c_str());
		}
		if (i == 2)
		{
			strcpy(shell2, out.c_str());
		}
		i++;
	}
	if (strcmp(command, "help") == 0)
	{
		help();
	}
	else if (strcmp(command, "sbinfo") == 0)
	{
		superInfo();
	}
	else if (strcmp(command, "ls") == 0)
	{
		ls();
	}

	else if (strcmp(command, "format") == 0)
	{
		reset();
	}
	else if (strcmp(command, "mkdir") == 0)
	{
		if (i != 2)
		{
			cout << "命令无效,请重新输入" << endl;
		}
		else
		{
			mkdir(shell1);
		}
	}
	else if (strcmp(command, "append") == 0)
	{
		if (i != 2)
		{
			cout << "命令无效,请重新输入" << endl;
		}
		else
		{
			append(shell1);
		}
	}

	else if (strcmp(command, "touch") == 0)
	{
		if (i != 2)
		{
			cout << "命令无效,请重新输入" << endl;
		}
		else
		{
			touch(shell1,currentinode);
		}
	}
	else if (strcmp(command, "rm") == 0)
	{
		if (i != 2)
		{
			cout << "命令无效,请重新输入" << endl;
		}
		else
		{
			rm(shell1, currentinode);
		}
	}

	else if (strcmp(command, "cd") == 0)
	{
		if (i != 2)
		{
			cout << "命令无效,请重新输入" << endl;
		}
		else
		{
			returninode = (struct inode*)calloc(1, sizeof(struct inode));
			returninode->inodeID = currentinode->inodeID;
			returninode = iget(returninode->inodeID);
			vector<direct>tmppath;
			tmppath = pathname;
			changedirect(shell1);
			if (returninode != NULL)
			{
				if (returninode->diskinode.mode / 1000 == 1)
				{
					currentinode = returninode;
				}
				else {
					pathname = tmppath;
					cout << "不是有效路径" << endl;
				}

			}
			else
			{
				pathname = tmppath;
				cout << "系统找不到指定路径" << endl;
			}
		}

	}
	else if (strcmp(command, "pwd") == 0)
	{
		pwd();
	}
	else if (strcmp("cp", command) == 0)
	{
		if (i != 3)
		{
			cout << "命令无效,请重新输入" << endl;
		}
		else
		{
			cp(shell1, shell2);
		}
	}
	else if (strcmp("cat", command) == 0)
	{
		if (i != 2)
		{
			cout << "命令无效,请重新输入" << endl;
		}
		else
		{
			cat(shell1);
		}
	}
	else if (strcmp("mv", command) == 0)
	{
		if (i != 3)
		{
			cout << "命令无效,请重新输入" << endl;
		}
		else
		{
			mv(shell1, shell2);
		}
	}
}