// Hash_Achievement.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "HashDefine.h"

int _tmain(int argc, _TCHAR* argv[])
{
	Hash_Map* hashMap = GetHashMap();
	InitHashMap();
	
	cout << "strlen(cjia) : " << strlen("cjia") << endl;
	HashMapAddNode("cjia", 20);
	cout << "strlen(�м�1) : " << strlen("�м�1") << endl;
	HashMapAddNode("�м�1", 30);
	cout << "strlen(ׯ��22) : " << strlen("ׯ��22") << endl;
	HashMapAddNode("ׯ��22", 40);
	cout << "strlen(�ϼ�33) : " << strlen("�ϼ�33") << endl;
	HashMapAddNode("�ϼ�33", 50);
	cout << "strlen(�¼�111) : " << strlen("�¼�111") << endl;
	HashMapAddNode("�¼�111", 50);
	cout << "strlen(ׯ��3) : " << strlen("ׯ��3") << endl;
	HashMapAddNode("ׯ��3", 60);
	cout << "strlen(�м�2) : " << strlen("�м�2") << endl;
	HashMapAddNode("�м�2", 70);
	HashMapAddNode("�м�3", 80);
	HashMapAddNode("�м�4", 90);
	HashMapAddNode("�м�5", 100);
	HashMapAddNode("�м�6", 110);

	HashMapFindNode("ׯ��3");
	HashMapDeleteNode("ׯ��3");
	HashMapFindNode("ׯ��3");

	HashMapFindNode("�м�4");
	HashMapDeleteNode("�м�4");
	HashMapFindNode("�м�4");

	HashMapAddNode("�м�4", 190);
	HashMapFindNode("�м�4");

	HashMapClear();

	system("pause");
	return 0;
}

