// Hash_Achievement.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "HashDefine.h"

int _tmain(int argc, _TCHAR* argv[])
{
	Hash_Map* hashMap = GetHashMap();
	InitHashMap();
	
	cout << "strlen(cjia) : " << strlen("cjia") << endl;
	HashMapAddNode("cjia", 20);
	cout << "strlen(闲家1) : " << strlen("闲家1") << endl;
	HashMapAddNode("闲家1", 30);
	cout << "strlen(庄家22) : " << strlen("庄家22") << endl;
	HashMapAddNode("庄家22", 40);
	cout << "strlen(老家33) : " << strlen("老家33") << endl;
	HashMapAddNode("老家33", 50);
	cout << "strlen(新家111) : " << strlen("新家111") << endl;
	HashMapAddNode("新家111", 50);
	cout << "strlen(庄家3) : " << strlen("庄家3") << endl;
	HashMapAddNode("庄家3", 60);
	cout << "strlen(闲家2) : " << strlen("闲家2") << endl;
	HashMapAddNode("闲家2", 70);
	HashMapAddNode("闲家3", 80);
	HashMapAddNode("闲家4", 90);
	HashMapAddNode("闲家5", 100);
	HashMapAddNode("闲家6", 110);

	HashMapFindNode("庄家3");
	HashMapDeleteNode("庄家3");
	HashMapFindNode("庄家3");

	HashMapFindNode("闲家4");
	HashMapDeleteNode("闲家4");
	HashMapFindNode("闲家4");

	HashMapAddNode("闲家4", 190);
	HashMapFindNode("闲家4");

	HashMapClear();

	system("pause");
	return 0;
}

