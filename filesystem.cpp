#pragma warning(disable : 4996)
#include<stdio.h>
#include<stdlib.h>
#include <string>
#include"functions.h"
#include<iostream>
#include<string>
using namespace std;

int main() {
	string userName = "cairongjiu";
	string a;	
	cout << "请随便输入一个密码:" << endl;
	cin >> a;
	cin.ignore();
	loadSuper();
	//formatting();	
	//reset();
	
	cout << "load"<< endl;
	pathname.clear();
	root= (struct inode*)calloc(1, sizeof(struct inode));
	root->inodeID = 0;
	getinodefromdisk(root);
	currentinode = root;
	direct currentdirect;
	currentdirect.inodeID = currentinode->inodeID;
	strcpy(currentdirect.directname, "/");
	pathname.push_back(currentdirect);
	char makename[14];
	strcpy(makename, "root");
	inode* inode = NULL, * ret = NULL;
	inode = currentinode;
	ret = cd(makename, inode);
	if (ret != NULL)
		currentinode = ret;

	help();
	while (!logout)
	{
		doit();
	}
}
