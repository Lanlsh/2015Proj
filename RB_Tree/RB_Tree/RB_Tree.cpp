// RB_Tree.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include < iostream >
#include < stdlib.h >
#include < stdio.h>
#include < vector>
#include < string>
#include < assert.h>

using namespace std;

/*
	�������Red Black Tree������һ�֡���ƽ������������BST���������ڼ������ѧ���õ���һ�����ݽṹ�����͵���;��ʵ�֡��������顱��
	�������AVL�����ƣ������ڽ��в����ɾ������ʱͨ������������ֶ����������ƽ�⣬�Ӷ���ýϸߵĲ������ܡ�
	�������Ȼ�Ǹ��ӵģ�������������������ʱ��Ҳ�Ƿǳ����õģ�������ʵ�����Ǹ�Ч�ģ���������O(logn)ʱ���������ҡ������ɾ���������n������Ԫ�ص���Ŀ

	�����ʹ�ó�����
		��C++ STL�У��ܶಿ��(����set, multiset, map, multimap)Ӧ���˺�����ı���(SGI STL�еĺ������һЩ�仯����Щ�޸��ṩ�˸��õ����ܣ��Լ���set������֧��)��

	RB_Tree��AVL���ıȽϣ�
		1. �������ܣ�AVLһ�������������RB���ģ���ΪAVL��������������ȵľ���ֵ<=1.
		2. �����ɾ����RB��һ��������AVL���ģ���ΪRB���ڽ��в����ɾ����������Ҫ��������ɫ������Ҳ�������������ת�����ڲ��������Σ�������ʹ��ﵽƽ�⣡��

	����������ʣ� �������������������ʺ�����������5������
		1. �ڵ�ʱ��ɫ���ɫ
		2. ���ڵ��Ǻ�ɫ
		3. ÿ��Ҷ�ӽڵ㣨NIL�ڵ㣬�սڵ㣩�Ǻ�ɫ
		4. ÿ����ɫ�ڵ�������ӽڵ㶼�Ǻ�ɫ������ÿ��Ҷ�ӽڵ㵽��������·���ϲ����������ĺ�ɫ�ڵ㣩
		5. ����һ�ڵ㵽��ÿ��Ҷ�ӽڵ������·����������ͬ��Ŀ�ĺ�ɫ�ڵ㡣
		
	��1~5�ɵôӸ���Ҷ�ӵ���Ŀ���·����������̵Ŀ���·������������

	���ԣ�������ǡ�����ƽ��ġ�����
	�������̵Ŀ���·�����ǡ���ɫ�ڵ㡱����Ŀ���·�����н���ĺ�ɫ�ͺ�ɫ�ڵ㡣

*/

struct TreeNode 
{
	int nValue;
	TreeNode* pLeft;	//����
	TreeNode* pRight;	//�Һ���
	TreeNode* pParent;	//���ڵ�
	bool bIsBlack;	//�Ƿ�Ϊ��ɫ,false��Ϊ��ɫ

	TreeNode(int value)
	{
		//Ĭ�������ڵ�Ϊ��ɫ
		nValue = value;
		pLeft = NULL;
		pRight = NULL;
		pParent = NULL;
		bIsBlack = false;
	}
};

static TreeNode* m_rootNode = NULL;

/*
Desc: ����ת
Param: node //Ҫ��ת�ĸ��ڵ�
*/
void RotationRight(TreeNode* node)
{
	TreeNode* c = node->pLeft;
	TreeNode* b = c->pLeft;
	TreeNode* a = node;

	if ((a == NULL) || (b == NULL) || (c == NULL))
	{
		//assert(a && b && c);
		return ;
	}

	TreeNode* tempRootNode = a->pParent;
	c->pParent = tempRootNode;
	if (tempRootNode != NULL)
	{
		if (tempRootNode->pRight == a)
		{
			tempRootNode->pRight = c;
		}
		else
		{
			tempRootNode->pLeft = c;
		}
	}

	a->pLeft = c->pRight;
	if (c->pRight)
	{
		c->pRight->pParent = a;
	}

	c->pRight = a;
	a->pParent = c;

	c->bIsBlack = true;
	a->bIsBlack = false;
	b->bIsBlack = false;
	if (c->pParent == NULL)
	{
		//���ڵ�
		m_rootNode = c;
	}

}

/*
Desc: ����ת
Param: node //Ҫ��ת�ĸ��ڵ�
*/
void RotationLeft(TreeNode* node)
{
	TreeNode* c = node->pRight;
	TreeNode* b = c->pRight;
	TreeNode* a = node;

	if ((a == NULL) || (b == NULL) || (c == NULL))
	{
		//assert(a && b && c);
		return ;
	}

	//���1�� ��a�ڵ���ӽڵ�ֻ����a���Һ��ӽڵ���ʱ����תһ�ξ�OK
	TreeNode* tempRootNode = a->pParent;
	c->pParent = tempRootNode;
	if (tempRootNode != NULL)
	{
		if (tempRootNode->pRight == a)
		{
			tempRootNode->pRight = c;
		}
		else
		{
			tempRootNode->pLeft = c;
		}

	}

	a->pRight = c->pLeft;
	if (c->pLeft)
	{
		c->pLeft->pParent = a;
	}

	c->pLeft = a;
	a->pParent = c;

	c->bIsBlack = true;
	a->bIsBlack = false;
	b->bIsBlack = false;
	if (c->pParent == NULL)
	{
		//���ڵ�
		m_rootNode = c;
	}
}

/*
Desc: ��������������ת
Param: node //Ҫ��ת�ĸ��ڵ�
*/
void RotationRL(TreeNode* node)
{
	if ((node == NULL) || (node->pRight == NULL) || (node->pRight->pLeft == NULL))
	{
		return;
	}

	TreeNode* a = node;
	TreeNode* b = a->pRight;
	TreeNode* c = b->pLeft;

	//����
	a->pRight = c;
	c->pParent = a;
	b->pLeft = c->pRight;
	b->pParent = c;
	c->pRight = b;

	//����
	RotationLeft(a);
}

/*
Desc: ��������������ת
Param: node //Ҫ��ת�ĸ��ڵ� //��ʱnode�����һ����>2��
*/
void RotationLR(TreeNode* node)
{
	if ((node == NULL) || (node->pLeft == NULL) || (node->pLeft->pRight == NULL))
	{
		return;
	}

	TreeNode* a = node;
	TreeNode* b = a->pLeft;
	TreeNode* c = b->pRight;

	//����
	a->pLeft = c;
	c->pParent = a;
	b->pRight = NULL;
	b->pParent = c;
	c->pLeft = b;

	//����
	RotationRight(a);
}

/*
	Desc: ���游�ڵ�ֻ��һ������ʱ
	Param�� pGrandFather �游�ڵ�
*/
void AdjustWhenOnlyOneKid(TreeNode* pGrandFather)
{
	//��ֻ��һ������ʱ�����ն��Ὣ�µ��Ӹ��ڵ���Ϊ��ɫ���Ӷ��ﵽ����RB�����򣡣���
	//����游�ڵ�����к��Ӷ���ͬһ�ߣ���ֱ����תһ�ξ���,���򣬾���Ҫʹ����������������
	if (pGrandFather->pLeft && pGrandFather->pLeft->pLeft)
	{
		//ֱ��ʹ������
		RotationRight(pGrandFather);
	}
	else if (pGrandFather->pRight && pGrandFather->pRight->pRight)
	{
		//ֱ��ʹ������
		RotationLeft(pGrandFather);
	}
	else if(pGrandFather->pLeft && pGrandFather->pLeft->pRight)
	{
		//ʹ��������
		RotationLR(pGrandFather);
	}
	else
	{
		//ʹ��������
		RotationRL(pGrandFather);
	}
}

/*
	Desc: ������ڵ�ʱ��ɫʱ����Ҫ�����ڵ���Ϊ��ǰ���������ڵ㣬ͬʱ���ø��ڵ�Ϊ��ɫ���Լ����游�ڵ�Ϊ��ɫ����
	Param��pParent ���ڵ�
	Param: pUncle ����ڵ�
*/
void AdjustWhenUncleIsRed(TreeNode* pParent, TreeNode* pUncle)
{
	pParent->bIsBlack = true;
	pUncle->bIsBlack = true;
	pParent->pParent->bIsBlack = false;

	if (pParent->pParent->pParent == NULL)
	{
		pParent->pParent->bIsBlack = true;
	}
}

/*
Desc: ������ڵ�ʱ��ɫʱ���Ƚ����ڵ������ڵ���Ϊ��ɫ�����Ƿ���ϣ������ϣ��������ϵݹ�
Param��pParent ���ڵ�
Param: pUncle ����ڵ�
Param: pChild ��ǰ�ĺ��ӽڵ�
*/
void AdjustWhenUncleIsBlack(TreeNode* pParent, TreeNode* pUncle, TreeNode* pChild)
{
	//1. ���ڵ����游�ڵ�����
	if (pParent->pParent->pLeft == pParent)
	{
		//�����ڵ㶼��һ�⣬һ����ת
		if (pParent->pLeft == pChild)
		{
			RotationRight(pChild);
		}
		else
		{
			//�����ڵ㲻��ͬһ�⣬��������������
			RotationLR(pChild);
		}
	}
	else
	{
		//2. ���ڵ����游�ڵ���Ҳ�
		if (pParent->pRight == pChild)
		{
			//�����ڵ㶼��һ�⣬һ����ת
			RotationLeft(pChild);
		}
		else
		{
			//�����ڵ㲻��ͬһ�⣬��������������
			RotationRL(pChild);
		}
	}

	
}

/*
Desc: ���游�ڵ�������������ʱ
Param��pGrandFather �游�ڵ�
Param: pRedNode ���������ĺ�ڵ�ĵڶ�����ڵ�
*/
void AdjustWhenHaveDoubleKid(TreeNode* pGrandFather, TreeNode* pUncle, TreeNode* pRedNode)
{
	//���������
	//1. pRedNode�������Ǻ�ɫ����ʱ��ͨ���ı�pRedNode�ĸ��ڵ������ڵ�Ϊ��ɫ...
	//2. pRedNode�������Ǻ�ɫ, ��ʱ����ת�������ڵ���Ϊ��ɫ���Լ����游�ڵ���Ϊ��ɫ
	if (pGrandFather->pLeft == pRedNode->pParent)
	{
		if (pGrandFather->pRight->bIsBlack == false)
		{
			//����Ϊ��ɫ
			AdjustWhenUncleIsRed(pRedNode->pParent, pUncle);
		}
		else
		{
			//����Ϊ��ɫ
			AdjustWhenUncleIsBlack(pRedNode->pParent, pUncle, pRedNode);
		}
	}
	else
	{
		if (pGrandFather->pLeft->bIsBlack == false)
		{
			//����Ϊ��ɫ
			AdjustWhenUncleIsRed(pRedNode->pParent, pUncle);
		}
		else
		{
			//����Ϊ��ɫ
			AdjustWhenUncleIsBlack(pRedNode->pParent, pUncle, pRedNode);
		}
	}

	
}

/*
	Desc: ��������ʹ�����RB���Ĺ���
	Param: node �����Ľڵ�
*/
void AdjustTree(TreeNode* node)
{
	//ֻ�е�������node�ĸ��ڵ�Ҳ�Ǻ�ɫʱ������Ҫ����
	if (node->pParent == NULL)
	{
		return;
	}

	if (node->pParent->bIsBlack == true)
	{
		return;
	}

	TreeNode* pTempParent = node->pParent->pParent;  //�游�ڵ�
	if (pTempParent == NULL)
	{
		return;
	}

	TreeNode* pChild = node; //���ӽڵ�
	TreeNode* pParent = node->pParent;	//���ڵ�
	TreeNode* pUncle = pTempParent->pLeft; //����ڵ�
	if(pTempParent->pLeft == pParent)
	{
		pUncle = pTempParent->pRight;
	}

	while (pTempParent)
	{
		//1. ��pTempParentֻ��һ������ʱ
		//2. ��pTempParent����������ʱ
		if ((pTempParent->pLeft != NULL) && (pTempParent->pRight != NULL))
		{
			//2. ��pTempParent����������ʱ
			AdjustWhenHaveDoubleKid(pTempParent, pUncle, pChild);
		}
		else
		{
			//1. ��pTempParentֻ��һ������ʱ
			AdjustWhenOnlyOneKid(pTempParent);
		}

		//�ж��Ƿ������
		if (pTempParent->pParent)
		{
			if (pTempParent->pParent->bIsBlack != pTempParent->bIsBlack)
			{
				//��������
				break;
			}
			else
			{
				pChild = pTempParent; //���ӽڵ�
				pParent = pChild->pParent;	//���ڵ�
				pTempParent = pParent->pParent;
				if (pTempParent == NULL)
				{
					assert(pTempParent == NULL);
					break;
				}

				TreeNode* pUncle = pTempParent->pLeft; //����ڵ�
				if (pTempParent->pLeft == pParent)
				{
					pUncle = pTempParent->pRight;
				}
			}
		}
		else
		{
			if (pTempParent->bIsBlack == true)
			{
				break;
			}
			else
			{
				assert(false);
			}
			
		}
		
	}
}

/*
	Desc: ���ӽڵ�
*/
void AddNode(int value)
{
	//1. �Ƚ��ڵ㰴��BST������ӣ��������ڵ����ɫ����Ϊ��ɫ������
	//2. ��ȥ��������ʹ�����RB_Tree��ԭ��

	TreeNode* node = new TreeNode(value);
	if (m_rootNode == NULL)
	{
		node->bIsBlack = true;
		m_rootNode = node;

		return;
	}

	//���ȣ��Ƚ��ڵ㰴��BST������ӣ��������ڵ����ɫ����Ϊ��ɫ������
	TreeNode* pTmpNode = m_rootNode;
	while (pTmpNode)
	{
		if (pTmpNode->nValue <= node->nValue)
		{
			if (pTmpNode->pRight == NULL)
			{
				pTmpNode->pRight = node;
				node->pParent = pTmpNode;
				break;
			}
			else
			{
				pTmpNode = pTmpNode->pRight;
			}
		}
		else
		{
			if (pTmpNode->pLeft == NULL)
			{
				pTmpNode->pLeft = node;
				node->pParent = pTmpNode;
				break;
			}
			else
			{
				pTmpNode = pTmpNode->pLeft;
			}
		}
	}

	//��ȥ��������ʹ�����RB_Tree��ԭ��
	AdjustTree(node);
}

/*
Desc: ����RB_Tree
*/
void CreatAVLTree(int array[], int size)
{
	if ((array == NULL) || (size <= 1))
	{
		return;
	}

	for (int i = 0; i<size; i++)
	{
		AddNode(array[i]);
	}
}

//������� 
void MiddleOrder(TreeNode* pRoot)
{
	if (pRoot != NULL)
	{
		MiddleOrder(pRoot->pLeft);
		string color = pRoot->bIsBlack ? "black" : "red";
		cout <<"ֵ�� " << pRoot->nValue << "  ��ɫ��  " << color << endl;
		MiddleOrder(pRoot->pRight);
	}
}

int main()
{
	int array[10] = { 0 };
	int size = sizeof(array) / sizeof(array[0]);
	cout << "������У�" << endl;
	for (int i = 0; i < size; i++)
	{
		array[i] = rand() % 100;
		cout << array[i] << " ";
	}

	cout << endl;
	CreatAVLTree(array, size);
	cout << "ʹ���������������£� " << endl;
	MiddleOrder(m_rootNode);

	int kind = 1;
	int value = 0;
	while (kind)
	{
		cout << endl << "��Ҫ��������ҵ�� 1������Ԫ��  2����ѯ����  3��ɾ������  0���˳�" << endl;
		cin >> kind;
		switch (kind)
		{
		case 1:
		{
			cout << "���������ӵ�Ԫ�أ� " << endl;
			cin >> value;
			AddNode(value);
			cout << "ʹ���������������£� " << endl;
			MiddleOrder(m_rootNode);
			break;
		}
		//case 2:
		//{
		//	cout << "��������Ҫ��ѯ��Ԫ�أ� " << endl;
		//	cin >> value;
		//	TreeNode* node = searchValue(value);
		//	if (node)
		//	{
		//		cout << "����������ֵΪ�� " << node->value << endl;
		//	}
		//	else
		//	{
		//		cout << "AVL�����޴����ݣ���" << endl;
		//	}

		//	break;
		//}
		//case 3:
		//{
		//	cout << "��������Ҫɾ����Ԫ�أ� " << endl;
		//	cin >> value;
		//	deleteNode(value);
		//	cout << "ʹ���������������£� " << endl;
		//	middleOrder(m_rootNode);
		//	break;
		//}
		}
	}

	cout << endl;
	//�ͷ��ڴ�
//	deleteMemory();
	system("pause");

    return 0;
}

