// AVL_Tree.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <assert.h>

using namespace std;

/*
	ƽ���������Self-Balacing binary tree��: �ֳ�Ϊ��AVL��������AVL��ͬ��AVL�㷨���������¼��Ϊ��AVL������
	��AVL�����ǡ�BST����Binary Search Tree�������ݱ�汾��Ҳ���ǡ�AVL����Ҳ��BST��������

	"AVL��"����������ڵ��������������Ȳ�ľ���ֵС����1����Ȩֵ����<���ڵ�<�Һ��ӵȵ����ԣ�ʹ�����ѯ������ɾ�ڵ��ʱ�临�Ӷȶ�ΪO(log2n)������ʹ�����
������£���������������ɾ�ڵ��Ч��Ҳ�ܴﵽlog2n!!!!

	AVL���Ĺ淶Ҫ��
		1. ����<���ڵ�<�Һ��� ��Ȩֵ��
		2. �����е�����ڵ��������������Ȳ�ľ���ֵ<=1

	AVL���Ĵ�����
		������ڵ�ΪT����һ���ڵ�aʱ��
		1. ���ж�T�Ƿ���ڣ���������ڣ���T = a
		2. ��a�Ȱ�BST�����ӽڵ�ķ�ʽ����
		3. �����ӽڵ�ĸ��ڵ㿪ʼ���жϴ����ĵ�������������Ȳ�ľ���ֵ�Ƿ�<=1����>1,��ִ��4����
		4. ͨ������ת��ʹ�������´ﵽƽ�⣬��ת�Ĵ���Ϊ1�λ��Σ������ʼ���ֲ�ƽ��������ĸ��ڵ��root����ת�������У�
			a. ����(R)������������ȸ���������ʱ�����������Լ��������Ľڵ�ȫ��������������ߣ���60��50��40����ʱ������תһ�μ�ƽ��
			b. ����(L)������������ȸ���������ʱ�����������Լ��������Ľڵ�ȫ�������������ұߣ���60��70��80����ʱ������תһ�μ�ƽ��
			c. ��������LR��������������ȸ���������ʱ���������ڵ�a��root�����ӵ��Һ����ϣ���60��40��50����ʱ��������תһ�ε�60��50��40����������תһ�μ�ƽ��
			d. ��������RL��������������ȸ���������ʱ���������ڵ�a��root���Һ��ӵ������ϣ���60��80��70����ʱ������תһ�ε�60��70��80����������תһ�μ�ƽ��

	AVL��ɾ���ڵ㣺
		���ҵ���Ҫɾ���Ľڵ㣬�ٸ�����Ҫɾ���ڵ�node���޺��ӽڵ����ɾ��node������Һ��ӵ�����Լ�node�����Һ��ӵ�������жϣ��ο�����void deleteNode(int value)��
		1. ɾ����ΪҶ�ӽڵ�
		2. ɾ���Ľڵ�������
		3. ɾ���Ľڵ����Һ���
		4. ɾ���Ľڵ����Һ��Ӷ���
*/

struct Tree 
{
	int value;	//Ȩֵ
	Tree* pLeft;	//��¼�������ڵ�
	Tree* pRight;	//��¼�������ڵ�
	Tree* pParent;	//��¼���ڵ�
	Tree* pEqualValue;	//��¼��ֵͬ�Ľڵ�

	int leftDeep;	//��¼���������
	int rightDeep;	//��¼���������

	Tree(int n)
	{
		value = n;
		pLeft = NULL;
		pRight = NULL;
		pParent = NULL;
		pEqualValue = NULL;
		leftDeep = 0;
		rightDeep = 0;
	}

	void AddEV(Tree* node)
	{
		//Ĭ�Ͻ�ֵ��ͬ�Ľڵ�ȫ���������ӣ���
		if (node == NULL)
		{
			return;
		}

		if (pEqualValue == NULL)
		{
			pEqualValue = node;
		}
		else
		{
			Tree* pTempNode = pEqualValue;
			while(pTempNode->pLeft)
			{
				pTempNode = pTempNode->pLeft;
			}

			pTempNode->pLeft = node;
			node->pParent = pTempNode;
		}
	}

	~Tree()
	{
		cout << "�ͷ��ڴ棺 " << value << endl;
		pLeft = NULL;
		pRight = NULL;
		pParent = NULL;
		pEqualValue = NULL;
	}
};

static vector<Tree*> m_rootTree;
static Tree* m_rootNode = NULL;

/*
Desc: ���ӽڵ�����
*/
void AddDeep(Tree* node)
{
	Tree* pParent = node->pParent;
	Tree* pChilde = node;
	while (pParent != NULL)
	{
		if (pParent->pLeft == pChilde)
		{
			pParent->leftDeep = pChilde->leftDeep >= pChilde->rightDeep ? (pChilde->leftDeep + 1) : (pChilde->rightDeep + 1);
		}
		else if (pParent->pRight == pChilde)
		{
			pParent->rightDeep = pChilde->leftDeep >= pChilde->rightDeep ? (pChilde->leftDeep + 1) : (pChilde->rightDeep + 1);
		}

		pChilde = pParent;
		pParent = pParent->pParent;
	}

}

/*
	Desc: �������
*/
void subDeep(Tree* node)
{
	Tree* pParent = node->pParent;
	Tree* pChilde = node;
	while (pParent != NULL)
	{
		if (pParent->pLeft == pChilde)
		{
			pParent->leftDeep = pChilde->leftDeep >= pChilde->rightDeep ? (pChilde->leftDeep + 1) : (pChilde->rightDeep + 1);
		}
		else if (pParent->pRight == pChilde)
		{
			pParent->rightDeep = pChilde->leftDeep >= pChilde->rightDeep ? (pChilde->leftDeep + 1) : (pChilde->rightDeep + 1);
		}

		pChilde = pParent;
		pParent = pParent->pParent;
	}

	m_rootNode = pChilde;
}

/*
	Desc: ����ת
	Param: node //Ҫ��ת�ĸ��ڵ�
*/
bool rotationRight(Tree* node)
{
	Tree* c = node->pLeft;
	Tree* b = c->pLeft;
	Tree* a = node;

	if ((a==NULL) || (b==NULL) || (c==NULL))
	{
		//assert(a && b && c);
		return false;
	}

	Tree* tempRootNode = a->pParent;
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

	if (a->pLeft)
	{
		a->leftDeep = a->pLeft->leftDeep >= a->pLeft->rightDeep ? (a->pLeft->leftDeep + 1) : (a->pLeft->rightDeep + 1);
	}
	else
	{
		a->leftDeep = 0;
	}
	
	c->rightDeep = c->pLeft->leftDeep >= c->pRight->rightDeep ? (c->pLeft->leftDeep + 1) : (c->pRight->rightDeep + 1);

	//�жϵ�����ת���Ƿ���ϡ�AVL����
	if (abs(c->leftDeep - c->rightDeep) <= 1)
	{
		return true;
	}

	return false;
}

/*
	Desc: ����ת
	Param: node //Ҫ��ת�ĸ��ڵ�
*/
bool rotationLeft(Tree* node)
{
	Tree* c = node->pRight;
	Tree* b = c->pRight;
	Tree* a = node;

	if ((a == NULL) || (b == NULL) || (c == NULL))
	{
		//assert(a && b && c);
		return false;
	}

	//���1�� ��a�ڵ���ӽڵ�ֻ����a���Һ��ӽڵ���ʱ����תһ�ξ�OK
	Tree* tempRootNode = a->pParent;
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

	if (a->pRight)
	{
		a->rightDeep = a->pRight->leftDeep >= a->pRight->rightDeep ? (a->pRight->leftDeep + 1) : (a->pRight->rightDeep + 1);
	}
	else
	{
		a->rightDeep = 0;
	}

	c->leftDeep = c->pLeft->leftDeep >= c->pLeft->rightDeep ? (c->pLeft->leftDeep + 1) : (c->pLeft->rightDeep + 1);

	//�жϵ�����ת���Ƿ���ϡ�AVL����
	if (abs(c->leftDeep - c->rightDeep) <= 1)
	{
		return true;
	}

	return false;
}

/*
	Desc: ��������������ת
	Param: node //Ҫ��ת�ĸ��ڵ�  //��ʱnode�����һ����>2��
*/
bool rotationRL(Tree* node)
{
	if ((node == NULL) || (node->pRight == NULL) || (node->pRight->pLeft == NULL))
	{
		assert((node != NULL) && (node->pRight != NULL) && (node->pRight->pLeft != NULL));
		return false;
	}

	Tree* a = node;
	Tree* b = a->pRight;
	Tree* c = b->pLeft;

	//����
	a->pRight = c;
	c->pParent = a;
	b->pLeft = c->pRight;
	b->pParent = c;
	c->pRight = b;

	if (b->pLeft)
	{
		b->leftDeep = b->pLeft->leftDeep >= b->pLeft->rightDeep ? (b->pLeft->leftDeep+1) : (b->pLeft->rightDeep+1);
	}
	else
	{
		b->leftDeep = 0;
	}
	
	c->rightDeep = b->leftDeep >= b->rightDeep ? (b->leftDeep+1) : (b->rightDeep+1);

	//����
	bool bIsOK = rotationLeft(a);
	if (bIsOK)
	{
		return true;
	}

	return false;
}

/*
Desc: ��������������ת
Param: node //Ҫ��ת�ĸ��ڵ�  //��ʱnode�����һ����>2��
*/
bool rotationLR(Tree* node)
{
	if ((node == NULL) || (node->pLeft == NULL) || (node->pLeft->pRight == NULL))
	{
		assert((node != NULL) && (node->pLeft != NULL) && (node->pLeft->pRight != NULL));
		return false;
	}

	Tree* a = node;
	Tree* b = a->pLeft;
	Tree* c = b->pRight;

	//����
	a->pLeft = c;
	c->pParent = a;
	b->pRight = NULL;
	b->pParent = c;
	c->pLeft = b;
	b->rightDeep = 0;
	c->leftDeep = 1;

	//����
	bool bIsOK = rotationRight(a);
	if (bIsOK)
	{
		return true;
	}

	return false;
}

/*
	Desc: ����ƽ����
*/
void adjuestBalance(Tree* newNode)
{
	//�������ӵĽڵ㿪ʼ���������ϣ��ж����Ƿ�ƽ��
	//ƽ�������� ���������ĸ߶Ȳ�ľ���ֵ<=1
	Tree* pParent = newNode->pParent;
	while (pParent)
	{
		//�����µĸ��ڵ�

		int leftDeep = pParent->leftDeep;
		int rightDeep = pParent->rightDeep;

		bool bRotationIsOk = false;
		if (leftDeep- rightDeep > 1)
		{
			if ((pParent->leftDeep <= 2) && (pParent->rightDeep <= 2))
			{
				//��pParent�����<=2ʱ
				//��1������
				if ((pParent->pLeft->pLeft != NULL) && (pParent->pLeft->pRight == NULL))
				{
					bRotationIsOk = rotationRight(pParent);
				}
				else
				{
					//��2��������
					bRotationIsOk = rotationLR(pParent);
				}
			}
			else
			{
				//��pParent�����>2ʱ
				if ((pParent->pLeft->pLeft->pLeft != NULL) && (pParent->pLeft->pLeft->pRight == NULL) && (pParent->pLeft->pLeft->pLeft == newNode))
				{
					//��1��ֱ������(80��70��90��60��75��55)
					bRotationIsOk = rotationRight(pParent);
				}
				else if ((pParent->pLeft->pLeft->pLeft == NULL) && (pParent->pLeft->pLeft->pRight != NULL) && (pParent->pLeft->pLeft->pRight == newNode))
				{
					//(2)��LR��������(80��70��90��60��75��65)
					bRotationIsOk = rotationLR(pParent->pLeft);
					bRotationIsOk = rotationRight(pParent);
				}
				else if ((pParent->pLeft->pRight->pLeft != NULL) && (pParent->pLeft->pRight->pRight == NULL) && (pParent->pLeft->pRight->pLeft == newNode))
				{
					//(3)��RL��������(80��70��90��60��75��73)
					bRotationIsOk = rotationRL(pParent->pLeft);
					bRotationIsOk = rotationRight(pParent);
				}
				else
				{
					//(4)��������������(80��70��90��60��75��76)
					bRotationIsOk = rotationLeft(pParent->pLeft);
					bRotationIsOk = rotationRight(pParent);
				}
			}

			//���ƽ�⣬���˳�ѭ������������ڴ˸��ڵ�����ת��ֱ��ƽ��
			if (bRotationIsOk)
			{
				//�������
				subDeep(pParent);

				//��ƽ��
				break;
			}

		}
		else if (leftDeep - rightDeep < -1)
		{
			if ((pParent->leftDeep <= 2) && (pParent->rightDeep <= 2))
			{
				//��pParent�����<=2ʱ
				//��1������
				if ((pParent->pRight->pRight != NULL) && (pParent->pRight->pLeft == NULL))
				{
					bRotationIsOk = rotationLeft(pParent);
				}
				else
				{
					//��2��������
					bRotationIsOk = rotationRL(pParent);
				}
			}
			else
			{
				//��pParent�����>2ʱ
				if ((pParent->pRight->pRight->pRight != NULL) && (pParent->pRight->pRight->pLeft == NULL) && (pParent->pRight->pRight->pRight == newNode))
				{
					//��1��ֱ������(80��70��100��90��110��116)
					bRotationIsOk = rotationLeft(pParent);
				}
				else if ((pParent->pRight->pRight->pRight == NULL) && (pParent->pRight->pRight->pLeft != NULL) && (pParent->pRight->pRight->pLeft == newNode))
				{
					//(2)��RL��������(80��70��100��90��110��105)
					bRotationIsOk = rotationRL(pParent->pRight);
					bRotationIsOk = rotationLeft(pParent);
				}
				else if ((pParent->pRight->pLeft->pRight != NULL) && (pParent->pRight->pLeft->pLeft == NULL) && (pParent->pRight->pLeft->pRight == newNode))
				{
					//(3)��LR��������(80��70��100��90��110��96)
					bRotationIsOk = rotationLR(pParent->pRight);
					bRotationIsOk = rotationLeft(pParent);
				}
				else
				{
					//(4)��������������(80��70��100��90��110��86)
					bRotationIsOk = rotationRight(pParent->pRight);
					bRotationIsOk = rotationLeft(pParent);
				}
			}

			//���ƽ�⣬���˳�ѭ������������ڴ˸��ڵ�����ת��ֱ��ƽ��
			if (bRotationIsOk)
			{
				//�������
				subDeep(pParent);

				//��ƽ��
				break;
			}
		}
		else
		{
			pParent = pParent->pParent;
		}

	}

}

/*
	Desc: ���ӽڵ�

*/
void addNode(int value)
{
	if (m_rootNode == NULL)
	{
		Tree* node = new Tree(value);
		m_rootTree.push_back(node);
		m_rootNode = node;
		return;
	}

	//�Ƚ��ڵ���϶�����������ʽ���úã���ȥ�ж��Ƿ�ƽ�⣬��ƽ��͵���
	bool bIsAddOk = false;
	Tree* pParent = m_rootNode;
	Tree* node = new Tree(value);
	m_rootTree.push_back(node);

	while (!bIsAddOk)
	{
		if (pParent->value > value)
		{
			if (pParent->pLeft == NULL)
			{
				pParent->pLeft = node;
				node->pParent = pParent;

				//���+1
				pParent->leftDeep += 1;
				if (pParent->pRight == NULL)
				{
					AddDeep(pParent);
				}

				bIsAddOk = true;
			}
			else
			{
				pParent = pParent->pLeft;
			}
		}
		else if (pParent->value < value)
		{
			if (pParent->pRight == NULL)
			{
				pParent->pRight = node;
				node->pParent = pParent;

				//���+1
				pParent->rightDeep += 1;
				if (pParent->pLeft == NULL)
				{
					AddDeep(pParent);
				}

				bIsAddOk = true;
			}
			else
			{
				pParent = pParent->pRight;
			}
		}
		else
		{
			//����ֵ��ͬ�Ľڵ�
			pParent->AddEV(node);
			bIsAddOk = true;
		}

	}

	//���ƽ����
	if (!bIsAddOk)
	{
		return;
	}

	//����ƽ����
	adjuestBalance(node);
}

/*
	Desc: ����ƽ���������Self-balancing binary search tree����AVL����
*/
void creatAVLTree(int array[], int size)
{
	if ((array == NULL) || (size <= 1))
	{
		return;
	}

	for (int i=0; i<size; i++)
	{
		addNode(array[i]);
	}

}

//ʹ��AVL������ֵvalue,���ظ�ֵ�Ľڵ��ָ�룬��û�ҵ����򷵻�NULL
Tree* searchValue(int value)
{
	if (m_rootNode == NULL)
	{
		return NULL;
	}

	Tree* pTempNode = m_rootNode;
	while (pTempNode)
	{
		if (pTempNode->value > value)
		{
			pTempNode = pTempNode->pLeft;
		}
		else if (pTempNode->value < value)
		{
			pTempNode = pTempNode->pRight;
		}
		else
		{
			//�ҵ�
			return pTempNode;
		}
	}

	return NULL;
}

void deleteMemory()
{
	//�ͷ���ѵ���Կ���ʹ����������ͷţ�����ο�����������ǰ�����򡢺���������ʵ�ֵĲ��ͣ���
	for (int i=m_rootTree.size()-1; i>=0; i--)
	{
		delete m_rootTree[i];
	}
}

void deleteMemoryByValue(int value)
{
	vector<Tree*>::iterator iter = m_rootTree.begin();
	for (; iter!=m_rootTree.end();)
	{
		if (value == (*iter)->value)
		{
			Tree* node = (*iter);
			iter = m_rootTree.erase(iter);
			delete node;
			node = NULL;
			break;
		}
		else
		{
			iter++;
		}
	}

	cout << endl << "m_rootTree.size(): " << m_rootTree.size() << endl;
}

//ɾ��Ҷ�ӽڵ�
void deleteLeafNode(Tree* node)
{
	//�ж�ɾ���ڵ�ĸ��ڵ���������������������
	Tree* pParent = node->pParent;
	if (pParent)
	{
		if (pParent->pLeft == node)
		{
			if (pParent->leftDeep - pParent->rightDeep >= 0)
			{
				pParent->pLeft = NULL;
				pParent->leftDeep = 0;
			}
			else
			{
				//������������ȸ����ʱ��ɾȥ��ڵ㣬��������ȱض����1����������ƽ�⣬��ʱ��Ҫ����
				pParent->pLeft = NULL;
				pParent->leftDeep = 0;
				rotationLeft(pParent);
			}
		}
		else
		{
			if (pParent->rightDeep - pParent->leftDeep >= 0)
			{
				pParent->pRight = NULL;
				pParent->rightDeep = 0;
			}
			else
			{
				//������������ȸ����ʱ��ɾȥ�ҽڵ㣬��������ȱض����1����������ƽ�⣬��ʱ��Ҫ����
				pParent->pRight = NULL;
				pParent->rightDeep = 0;
				rotationRight(pParent);
			}
		}

		subDeep(pParent);
	}
	else
	{
		m_rootNode = NULL;
	}
}

//ɾ���Ľڵ�ֻ��������
void deleteNodeOnlyHaveLeftChild(Tree* node)
{
	Tree* pParent = node->pParent;
	if (pParent)
	{
		node->pLeft->pParent = pParent;
		pParent->pLeft = node->pLeft;
		pParent->leftDeep = pParent->leftDeep - 1;

		subDeep(pParent);
	}
	else
	{
		node->pLeft->pParent = NULL;
		m_rootNode = node->pLeft;
	}
}

//ɾ���Ľڵ�ֻ�����Һ���
void deleteNodeOnlyHaveRightChild(Tree* node)
{
	Tree* pParent = node->pParent;
	if (pParent)
	{
		node->pRight->pParent = pParent;
		pParent->pRight = node->pRight;
		pParent->rightDeep = pParent->rightDeep - 1;

		subDeep(pParent);
	}
	else
	{
		node->pRight->pParent = NULL;
		m_rootNode = node->pRight;
	}
}

//ɾ���Ľڵ������Һ���
void deleteNodeHaveDoubleKid(Tree* node)
{
	Tree* pParent = node->pParent;

	//�Ƚ�node�����Һ��ӵ���ȣ������С�Ĺҵ���ȴ�ĵط�
	Tree* pLeft = node->pLeft;
	Tree* pRight = node->pRight;
	if (node->leftDeep >= node->rightDeep)
	{
		//��node���Һ���ȫ������node��������
		if (pParent)
		{
			if (pParent->pLeft == node)
			{
				pParent->pLeft = pLeft;
			}
			else
			{
				pParent->pRight = pLeft;
			}
			
			pLeft->pParent = pParent;
		}
		else
		{
			pLeft->pParent = NULL;
		}

		//�ҵ�node�����ӵ����ҽڵ�
		Tree* temp = pLeft;
		while (temp->pRight)
		{
			temp = temp->pRight;
		}

		pRight->pParent = temp;
		temp->pRight = pRight;

		subDeep(pRight);
		bool bIsNeedRotation = abs(pLeft->leftDeep - pLeft->rightDeep) > 1;

		while (bIsNeedRotation)
		{
			//����
			rotationLeft(pLeft);
			bIsNeedRotation = abs(pLeft->pParent->leftDeep - pLeft->pParent->rightDeep) > 1;
			if (bIsNeedRotation)
			{
				//��ʱ���ܳ�����������ƽ������
				//����
				rotationRight(pLeft->pLeft);
				pLeft = pLeft->pParent;
			}
		}

		subDeep(pLeft);

	}
	else
	{
		//��node������ȫ������node���Һ�����
		if (pParent)
		{
			if (pParent->pLeft == node)
			{
				pParent->pLeft = pLeft;
			}
			else
			{
				pParent->pRight = pLeft;
			}

			pRight->pParent = pParent;
		}
		else
		{
			pRight->pParent = NULL;
		}

		//�ҵ�node�����ӵ����ҽڵ�
		Tree* temp = pRight;
		while (temp->pLeft)
		{
			temp = temp->pLeft;
		}

		pLeft->pParent = temp;
		temp->pLeft = pLeft;

		subDeep(pLeft);
		bool bIsNeedRotation = abs(pRight->leftDeep - pRight->rightDeep) > 1;
		while (bIsNeedRotation)
		{
			//����
			rotationRight(pRight);
			bIsNeedRotation = abs(pRight->pParent->leftDeep - pRight->pParent->rightDeep) > 1;
			if (bIsNeedRotation)
			{
				//��ʱ���ܳ�����������ƽ������
				//����
				rotationLeft(pRight->pRight);
				pRight = pRight->pRight;
			}
		}

		subDeep(pRight);
	}

}

//AVL��ɾ���ڵ�
void deleteNode(int value)
{
	//�ȴ�AVL�����ҵ��ýڵ�
	Tree* node = searchValue(value);
	if (node == NULL)
	{
		cout << endl << "AVL����û�д˽ڵ㣡����" << endl;
		return;
	}

	//���ҵ����������Ҫɾ���ڵ�node�ĸ��ڵ���ɾ��node������Һ��ӵ���Ⱥ�node�����Һ��ӵ�������ж�
	//1. ɾ����ΪҶ�ӽڵ�
	//2. ɾ���Ľڵ�������
	//3. ɾ���Ľڵ����Һ���
	//4. ɾ���Ľڵ����Һ��Ӷ���
	if ((node->pLeft == NULL) && (node->pRight == NULL))
	{
		//1. ɾ����ΪҶ�ӽڵ�
		deleteLeafNode(node);
	}
	else if ((node->pLeft != NULL) && (node->pRight == NULL))
	{
		//2. ɾ���Ľڵ�������
		deleteNodeOnlyHaveLeftChild(node);
	}
	else if ((node->pLeft == NULL) && (node->pRight != NULL))
	{
		//3. ɾ���Ľڵ����Һ���
		deleteNodeOnlyHaveRightChild(node);
	}
	else
	{
		//4. ɾ���Ľڵ����Һ��Ӷ���
		deleteNodeHaveDoubleKid(node);
	}
	

	//�ͷŽڵ���ڴ�
	deleteMemoryByValue(value);
}


//�������
void middleOrder(Tree* pRoot)
{
	if (pRoot != NULL)
	{
		middleOrder(pRoot->pLeft);
		cout << pRoot->value << " ";
		Tree* pEVNode = pRoot->pEqualValue;
		while (pEVNode)
		{
			cout << pEVNode->value << " ";
			pEVNode = pEVNode->pLeft;
		}
		middleOrder(pRoot->pRight);
	}
}


int main()
{
	int array[20] = { 0 };
	int size = sizeof(array) / sizeof(array[0]);
	cout << "������У�" << endl;
	for (int i=0; i<size; i++)
	{
		array[i] = rand() % 100;
		cout << array[i] << " ";
	}

	cout << endl;
	creatAVLTree(array, size);
	cout << "ʹ���������������£� " << endl;
	middleOrder(m_rootNode);

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
			addNode(value);
			cout << "ʹ���������������£� " << endl;
			middleOrder(m_rootNode);
			break;
		}
		case 2: 
		{
			cout << "��������Ҫ��ѯ��Ԫ�أ� " << endl;
			cin >> value;
			Tree* node = searchValue(value);
			if (node)
			{
				cout << "����������ֵΪ�� " << node->value << endl;
			}
			else
			{
				cout << "AVL�����޴����ݣ���" << endl;
			}

			break;
		}
		case 3:
		{
			cout << "��������Ҫɾ����Ԫ�أ� " << endl;
			cin >> value;
			deleteNode(value);
			cout << "ʹ���������������£� " << endl;
			middleOrder(m_rootNode);
			break;
		}	
		}
	}

	cout << endl;
	//�ͷ��ڴ�
	deleteMemory();
	system("pause");

    return 0;
}

