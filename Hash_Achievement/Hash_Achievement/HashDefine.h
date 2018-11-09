#include "stdafx.h"

#define HASH_MAP_SIZE 250

struct Student
{
	char name[20];
	int age;
	Student* next;
};

struct HashMap_Node
{
	int key;
	Student* user_data;
};

struct Hash_Map
{
	HashMap_Node hashMapNode[HASH_MAP_SIZE];
};

static Hash_Map* HM_hashMap = (Hash_Map*)malloc(sizeof(Hash_Map));

void InitHashMap()
{
	cout << "初始化hashMap！！！" << endl;
	for (int i = 0; i < HASH_MAP_SIZE; i++)
	{
		HM_hashMap->hashMapNode[i].key = i;
		HM_hashMap->hashMapNode[i].user_data = NULL;
	}
}



static Hash_Map* GetHashMap()
{
	return HM_hashMap;
}

Student* CreatNode(int key, char* name, int age)
{
	if (name == NULL)
	{
		return NULL;
	}

	Student* stu = (Student*)malloc(sizeof(Student));
	stu->age = age;
	strcpy_s(stu->name, strlen(name) + 1, name);
	stu->next = NULL;

	return stu;
}

int GetHashKey(char* name)
{
	if (name == NULL)
	{
		return -1;
	}

	int key = 0;
	key = (strlen(name) * 33 + 1) % HASH_MAP_SIZE;
	return key;
}

bool AddToHashTable(int key, Student* node)
{
	if (node == NULL)
	{
		return false;
	}

	bool result = false;
	Hash_Map* hashMap = GetHashMap();

	if (key != hashMap->hashMapNode[key].key)
	{
		result = false;
	}
	else
	{
		Student* userData = hashMap->hashMapNode[key].user_data;
		if (userData == NULL)
		{
			hashMap->hashMapNode[key].user_data = node;
			result = true;
			return result;
		}

		do
		{
			if (userData->next == NULL)
			{
				userData->next = static_cast<Student*>(node);
				result = true;
				return result;
			}
			else
			{
				userData = userData->next;
			}
		} while (userData != NULL);
	}

	return result;
}

bool  HashMapAddNode(char* name, int age)
{
	if (name == NULL)
	{
		return false;
	}

	bool result = false;
	int key = GetHashKey(name);
	Student* hashNode = CreatNode(key, name, age);
	if (hashNode == NULL)
	{
		return result;
	}

	result = AddToHashTable(key, hashNode);
	return result;
}

bool HashMapDeleteNode(char* name)
{
	if (name == NULL)
	{
		return false;
	}

	bool result = false;
	int key = GetHashKey(name);
	Hash_Map* hashMap = GetHashMap();

	if (key == hashMap->hashMapNode[key].key)
	{
		Student* headNode = static_cast<Student*>(hashMap->hashMapNode[key].user_data);
		Student* prior_Node = static_cast<Student*>(hashMap->hashMapNode[key].user_data);
		Student* beHind_Node = headNode->next;
		char* tempName = headNode->name;
		if (strcmp(name, tempName) == 0)
		{
			headNode->next = NULL;
			delete headNode;
			headNode = NULL;
			hashMap->hashMapNode[key].user_data = beHind_Node;
			result = true;
		}
		else
		{
			while (beHind_Node != NULL)
			{
				prior_Node = headNode;
				Student* cur_Node = beHind_Node;
				beHind_Node = beHind_Node->next;
				tempName = cur_Node->name;
				if (strcmp(name, tempName) == 0)
				{
					headNode->next = NULL;
					prior_Node->next = beHind_Node;
					free(cur_Node);
					cur_Node = NULL;
					result = true;
					break;
				}

				headNode = headNode->next;
			}
		}
	}

	return result;
}

void HashMapFindNode(char* name)
{
	if (name == NULL)
	{
		return;
	}

	int key = GetHashKey(name);
	Hash_Map* hashMap = GetHashMap();
	Student* headNode = static_cast<Student*>(hashMap->hashMapNode[key].user_data);

	while (headNode)
	{
		if (headNode != NULL)
		{
			char* tempName = headNode->name;
			if (strcmp(name, tempName) == 0)
			{
				cout << "找到信息如下：" << endl;
				cout << "姓名：  " << tempName << endl;
				cout << "年龄：  " << headNode->age << endl;
				break;
			}
		}

		headNode = headNode->next;
	}

}

void HashMapClear()
{
	Hash_Map* hashMap = GetHashMap();
	for (int i = 0; i < HASH_MAP_SIZE; i++)
	{
		if (hashMap->hashMapNode[i].user_data != NULL)
		{
			//如何一次性释放链表内存：预先分配一块内存区域给链表，然后直接释放这块区域！！！！！
			Student* head = hashMap->hashMapNode[i].user_data;
			Student* beHind = head->next;
			if (beHind)
			{
				head->next = NULL;
				free(head);
				head = beHind;

				while (head)
				{
					beHind = beHind->next;
					head->next = NULL;
					free(head);					
					head = beHind;								
				}
				
				hashMap->hashMapNode[i].user_data = NULL;
			}
			else
			{
				free(head);
				hashMap->hashMapNode[i].user_data = NULL;
			}
		}

	}
	delete hashMap;
	hashMap = NULL;
}