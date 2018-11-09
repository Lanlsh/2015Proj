// RB_Tree.cpp : 定义控制台应用程序的入口点。
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
	红黑树（Red Black Tree）：是一种“自平衡二叉查找树（BST）”，是在计算机科学中用到的一种数据结构，典型的用途是实现“关联数组”。
	红黑树与AVL树类似，都是在进行插入和删除操作时通过特殊操作保持二叉查找树的平衡，从而获得较高的查找性能。
	红黑树虽然是复杂的，但它在最坏的情况下运行时间也是非常良好的，并且在实践中是高效的：它可以在O(logn)时间内做查找、插入和删除，这里的n是树中元素的数目

	红黑树使用场景：
		在C++ STL中，很多部分(包括set, multiset, map, multimap)应用了红黑树的变体(SGI STL中的红黑树有一些变化，这些修改提供了更好的性能，以及对set操作的支持)。

	RB_Tree和AVL树的比较：
		1. 查找性能，AVL一般情况下是优于RB树的，因为AVL树的左右子树深度的绝对值<=1.
		2. 插入和删除，RB树一般是优于AVL树的，因为RB树在进行插入或删除操作后“需要少量的颜色变更并且不超过三次树旋转（对于插入是两次）”就能使其达到平衡！！

	红黑树的性质： 当满足二叉查找树的性质后再满足以下5点性质
		1. 节点时红色或黑色
		2. 根节点是黑色
		3. 每个叶子节点（NIL节点，空节点）是黑色
		4. 每个红色节点的两个子节点都是黑色。（即每个叶子节点到根的所有路径上不能有连续的红色节点）
		5. 从任一节点到其每个叶子节点的所有路径都包含相同数目的黑色节点。
		
	由1~5可得从根到叶子的最长的可能路径不多于最短的可能路径的两倍长）

	所以，红黑树是“大致平衡的”！！
	红黑树最短的可能路径都是“黑色节点”，最长的可能路径是有交替的红色和黑色节点。

*/

struct TreeNode 
{
	int nValue;
	TreeNode* pLeft;	//左孩子
	TreeNode* pRight;	//右孩子
	TreeNode* pParent;	//父节点
	bool bIsBlack;	//是否为黑色,false即为红色

	TreeNode(int value)
	{
		//默认新增节点为红色
		nValue = value;
		pLeft = NULL;
		pRight = NULL;
		pParent = NULL;
		bIsBlack = false;
	}
};

static TreeNode* m_rootNode = NULL;

/*
Desc: 右旋转
Param: node //要旋转的跟节点
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
		//根节点
		m_rootNode = c;
	}

}

/*
Desc: 左旋转
Param: node //要旋转的跟节点
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

	//情况1： 当a节点的子节点只存在a的右孩子节点上时，旋转一次就OK
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
		//根节点
		m_rootNode = c;
	}
}

/*
Desc: 先右旋，再左旋转
Param: node //要旋转的跟节点
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

	//右旋
	a->pRight = c;
	c->pParent = a;
	b->pLeft = c->pRight;
	b->pParent = c;
	c->pRight = b;

	//左旋
	RotationLeft(a);
}

/*
Desc: 先左旋，再右旋转
Param: node //要旋转的跟节点 //此时node的深度一定是>2的
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

	//左旋
	a->pLeft = c;
	c->pParent = a;
	b->pRight = NULL;
	b->pParent = c;
	c->pLeft = b;

	//右旋
	RotationRight(a);
}

/*
	Desc: 当祖父节点只有一个孩子时
	Param： pGrandFather 祖父节点
*/
void AdjustWhenOnlyOneKid(TreeNode* pGrandFather)
{
	//当只有一个孩子时，最终都会将新的子根节点设为黑色，从而达到符合RB树规则！！！
	//如果祖父节点的所有孩子都在同一边，则直接旋转一次就行,否则，就需要使用左右旋或右左旋
	if (pGrandFather->pLeft && pGrandFather->pLeft->pLeft)
	{
		//直接使用右旋
		RotationRight(pGrandFather);
	}
	else if (pGrandFather->pRight && pGrandFather->pRight->pRight)
	{
		//直接使用左旋
		RotationLeft(pGrandFather);
	}
	else if(pGrandFather->pLeft && pGrandFather->pLeft->pRight)
	{
		//使用左右旋
		RotationLR(pGrandFather);
	}
	else
	{
		//使用右左旋
		RotationRL(pGrandFather);
	}
}

/*
	Desc: 当叔叔节点时黑色时，需要将父节点作为当前的子树根节点，同时设置父节点为黑色，自己和祖父节点为红色！！
	Param：pParent 父节点
	Param: pUncle 叔叔节点
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
Desc: 当叔叔节点时黑色时，先将父节点和叔叔节点设为黑色，看是否符合，不符合，继续往上递归
Param：pParent 父节点
Param: pUncle 叔叔节点
Param: pChild 当前的孩子节点
*/
void AdjustWhenUncleIsBlack(TreeNode* pParent, TreeNode* pUncle, TreeNode* pChild)
{
	//1. 父节点在祖父节点的左侧
	if (pParent->pParent->pLeft == pParent)
	{
		//新增节点都在一测，一次旋转
		if (pParent->pLeft == pChild)
		{
			RotationRight(pChild);
		}
		else
		{
			//新增节点不在同一测，先左旋，再右旋
			RotationLR(pChild);
		}
	}
	else
	{
		//2. 父节点在祖父节点的右侧
		if (pParent->pRight == pChild)
		{
			//新增节点都在一测，一次旋转
			RotationLeft(pChild);
		}
		else
		{
			//新增节点不在同一测，先右旋，再左旋
			RotationRL(pChild);
		}
	}

	
}

/*
Desc: 当祖父节点有两个个孩子时
Param：pGrandFather 祖父节点
Param: pRedNode 出现连续的红节点的第二个红节点
*/
void AdjustWhenHaveDoubleKid(TreeNode* pGrandFather, TreeNode* pUncle, TreeNode* pRedNode)
{
	//分两种情况
	//1. pRedNode的叔叔是红色，此时先通过改变pRedNode的父节点和叔叔节点为黑色...
	//2. pRedNode的叔叔是黑色, 此时先旋转，将父节点设为黑色，自己和祖父节点设为红色
	if (pGrandFather->pLeft == pRedNode->pParent)
	{
		if (pGrandFather->pRight->bIsBlack == false)
		{
			//叔叔为红色
			AdjustWhenUncleIsRed(pRedNode->pParent, pUncle);
		}
		else
		{
			//叔叔为黑色
			AdjustWhenUncleIsBlack(pRedNode->pParent, pUncle, pRedNode);
		}
	}
	else
	{
		if (pGrandFather->pLeft->bIsBlack == false)
		{
			//叔叔为红色
			AdjustWhenUncleIsRed(pRedNode->pParent, pUncle);
		}
		else
		{
			//叔叔为黑色
			AdjustWhenUncleIsBlack(pRedNode->pParent, pUncle, pRedNode);
		}
	}

	
}

/*
	Desc: 调整树，使其符合RB树的规则
	Param: node 新增的节点
*/
void AdjustTree(TreeNode* node)
{
	//只有当新增的node的父节点也是红色时，才需要调整
	if (node->pParent == NULL)
	{
		return;
	}

	if (node->pParent->bIsBlack == true)
	{
		return;
	}

	TreeNode* pTempParent = node->pParent->pParent;  //祖父节点
	if (pTempParent == NULL)
	{
		return;
	}

	TreeNode* pChild = node; //孩子节点
	TreeNode* pParent = node->pParent;	//父节点
	TreeNode* pUncle = pTempParent->pLeft; //叔叔节点
	if(pTempParent->pLeft == pParent)
	{
		pUncle = pTempParent->pRight;
	}

	while (pTempParent)
	{
		//1. 当pTempParent只有一个孩子时
		//2. 当pTempParent有两个孩子时
		if ((pTempParent->pLeft != NULL) && (pTempParent->pRight != NULL))
		{
			//2. 当pTempParent有两个孩子时
			AdjustWhenHaveDoubleKid(pTempParent, pUncle, pChild);
		}
		else
		{
			//1. 当pTempParent只有一个孩子时
			AdjustWhenOnlyOneKid(pTempParent);
		}

		//判断是否调整好
		if (pTempParent->pParent)
		{
			if (pTempParent->pParent->bIsBlack != pTempParent->bIsBlack)
			{
				//调整好了
				break;
			}
			else
			{
				pChild = pTempParent; //孩子节点
				pParent = pChild->pParent;	//父节点
				pTempParent = pParent->pParent;
				if (pTempParent == NULL)
				{
					assert(pTempParent == NULL);
					break;
				}

				TreeNode* pUncle = pTempParent->pLeft; //叔叔节点
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
	Desc: 增加节点
*/
void AddNode(int value)
{
	//1. 先将节点按照BST规则添加，且新增节点的颜色设置为红色！！！
	//2. 再去调整树，使其符合RB_Tree的原则

	TreeNode* node = new TreeNode(value);
	if (m_rootNode == NULL)
	{
		node->bIsBlack = true;
		m_rootNode = node;

		return;
	}

	//首先，先将节点按照BST规则添加，且新增节点的颜色设置为红色！！！
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

	//再去调整树，使其符合RB_Tree的原则
	AdjustTree(node);
}

/*
Desc: 创建RB_Tree
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

//中序遍历 
void MiddleOrder(TreeNode* pRoot)
{
	if (pRoot != NULL)
	{
		MiddleOrder(pRoot->pLeft);
		string color = pRoot->bIsBlack ? "black" : "red";
		cout <<"值： " << pRoot->nValue << "  颜色：  " << color << endl;
		MiddleOrder(pRoot->pRight);
	}
}

int main()
{
	int array[10] = { 0 };
	int size = sizeof(array) / sizeof(array[0]);
	cout << "随机数列：" << endl;
	for (int i = 0; i < size; i++)
	{
		array[i] = rand() % 100;
		cout << array[i] << " ";
	}

	cout << endl;
	CreatAVLTree(array, size);
	cout << "使用中序遍历输出如下： " << endl;
	MiddleOrder(m_rootNode);

	int kind = 1;
	int value = 0;
	while (kind)
	{
		cout << endl << "需要进行哪种业务： 1、增加元素  2、查询数据  3、删除数据  0、退出" << endl;
		cin >> kind;
		switch (kind)
		{
		case 1:
		{
			cout << "请输入增加的元素： " << endl;
			cin >> value;
			AddNode(value);
			cout << "使用中序遍历输出如下： " << endl;
			MiddleOrder(m_rootNode);
			break;
		}
		//case 2:
		//{
		//	cout << "请输入需要查询的元素： " << endl;
		//	cin >> value;
		//	TreeNode* node = searchValue(value);
		//	if (node)
		//	{
		//		cout << "已搜索到，值为： " << node->value << endl;
		//	}
		//	else
		//	{
		//		cout << "AVL树中无此数据！！" << endl;
		//	}

		//	break;
		//}
		//case 3:
		//{
		//	cout << "请输入需要删除的元素： " << endl;
		//	cin >> value;
		//	deleteNode(value);
		//	cout << "使用中序遍历输出如下： " << endl;
		//	middleOrder(m_rootNode);
		//	break;
		//}
		}
	}

	cout << endl;
	//释放内存
//	deleteMemory();
	system("pause");

    return 0;
}

