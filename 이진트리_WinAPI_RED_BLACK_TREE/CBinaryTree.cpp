#include <iostream>
#include <Windows.h>
#include "CBinaryTree.h"

CBinaryTree::CBinaryTree()
{
	mRootNode = NULL;
	_iNodeCount = 0;
	Nil.Color = BLACK;
	Nil.Parent = NULL;
	Nil.left = NULL;
	Nil.right = NULL;

	redBrush = CreateSolidBrush( RGB(255,0,0));
	blackBrush = CreateSolidBrush(RGB(255, 255, 255));
}
CBinaryTree::~CBinaryTree()
{
	//동적할당 해제하기.
	mRootNode = NULL;
	_iNodeCount = 0;

}

void CBinaryTree::Print()
{
	InOrder(mRootNode, 0);
	//TestCheck();
}

void CBinaryTree::GDIPrint(HWND hwnd)
{
	int x = 20 * _iNodeCount;
	int y = 0;
	HDC hdc = GetDC(hwnd);
	PreOrder(mRootNode, x, y, hdc);
	ReleaseDC(hwnd, hdc);
}

void CBinaryTree::InOrder(st_TREE_NODE* node, int count)
{
	if (node)
	{		
		int tmp = count += 2;
		InOrder(node->left, tmp);		
		for (int i = 0; i < count; i++)
			printf(" ");
		printf("%d\n", node->iData);
		InOrder(node->right, tmp);
	}
}

void CBinaryTree::PreOrder(st_TREE_NODE* node, int leftx, int lefty, HDC hdc)
{
	int rightX = leftx + 50;
	int rightY = lefty + 50;
	
	WCHAR data[4];
	ZeroMemory(data, 4);
	
	if (node)
	{
		wsprintf(data, L"%d", node->iData);
		if (node->Color == BLACK)
			oldBrush = (HBRUSH)SelectObject(hdc, blackBrush);
		else
			oldBrush = (HBRUSH)SelectObject(hdc, redBrush);
		Ellipse(hdc, leftx, lefty, rightX, rightY);//출력
		SelectObject(hdc, oldBrush);
		TextOut(hdc, leftx + 15, lefty + 20, data,  wcslen(data));
		PreOrder(node->left, leftx - 70, lefty + 70, hdc);//왼쪽
		PreOrder(node->right, leftx + 70, lefty + 70, hdc);//오른쪽
	}
}

void CBinaryTree::InsertNode(int data)
{
	st_TREE_NODE* tmp = mRootNode;
	_iNodeCount++;
	st_TREE_NODE* insert = new st_TREE_NODE;
	insert->iData = data;
	insert->left = &Nil;
	insert->right = &Nil;
	insert->Color = RED;

	if (mRootNode == NULL)
	{
		insert->Parent = NULL;
		mRootNode = insert;
		return;
	}
	while (tmp != NULL)
	{
		if (CompareData(data, tmp->iData) < 0)
		{
			if (tmp->left != NULL)
			{
				tmp = tmp->left;
			}
			else
			{
				tmp->left = insert;
				insert->Parent = tmp;
				break;
			}
		}
		else if (CompareData(data, tmp->iData) > 0)
		{
			if (tmp->right != NULL)
			{
				tmp = tmp->right;
			}
			else
			{
				tmp->right = insert;
				insert->Parent = tmp;
				break;
			}
		}
		else
			break;
	}
	TreeBalance(insert);
	//밸런스 잡는 코드 추가.
}

bool CBinaryTree::DeleteNode(int data)
{
	st_TREE_NODE* node = NULL;
	st_TREE_NODE* tmpRoot = mRootNode;

	while (tmpRoot != NULL) // 삭제하려는 노드 찾기
	{
		if (CompareData(data, tmpRoot->iData) == 0)
		{
			node = tmpRoot;
			break;
		}
		if (tmpRoot->iData > data)
		{
			tmpRoot = tmpRoot->left;
		}
		else
			tmpRoot = tmpRoot->right;
	}
	if (tmpRoot == NULL) //못찾아서 return
		return false;

	st_TREE_NODE* freeNode = FindDeleteNode(node->right);

	st_TREE_NODE* parent = NULL;
	SearchParentNode(freeNode->iData, &parent);
	node->iData = freeNode->iData;
	parent->left = freeNode->right;
	
	delete freeNode;
	return false;
}

CBinaryTree::st_TREE_NODE* CBinaryTree::FindDeleteNode(st_TREE_NODE* node)
{
	if (node->left == NULL)
		return node;
	else if (node->left != NULL)
		FindDeleteNode(node->left);
	else
		return NULL;
}

int CBinaryTree::CompareData(int iSrc, int iDest)
{
	if (iSrc == iDest)
		return 0;
	else if (iSrc > iDest)
		return 1;
	else
		return -1;
}

int CBinaryTree::GetUseCount()
{
	return _iNodeCount;
}


void CBinaryTree::SearchParentNode(int iData, st_TREE_NODE** node)
{
	st_TREE_NODE* tmpRoot = mRootNode;
	while (tmpRoot != NULL)
	{
		if ((CompareData(iData, tmpRoot->left->iData) == 0))
		{
			*node = tmpRoot;
			return;
		}
		else if ((CompareData(iData, tmpRoot->right->iData) == 0))
		{
			*node = tmpRoot;
			return;
		}
		if (tmpRoot->iData > iData)
		{
			tmpRoot = tmpRoot->left;
		}
		else
			tmpRoot = tmpRoot->right;
	}
}

void CBinaryTree::NodeLeftTurn(st_TREE_NODE* node)
{
	st_TREE_NODE* right = node->right;
	if (node->Parent->left == node)
		node->Parent->left = right;
	else if (node->Parent->right == node)
		node->Parent->right = right;
	node->right = right->left;
	right->Parent = node->Parent;
	right->left->Parent = node;
	right->left = node;
	node->Parent = right;

}

void CBinaryTree::NodeRightTurn(st_TREE_NODE* node)
{
	st_TREE_NODE* left = node->left;
	if (node->Parent->left == node)
		node->Parent->left = left;
	else if (node->Parent->right == node)
		node->Parent->right = left;
	node->left = left->right;
	left->Parent = node->Parent;
	node->Parent = left;
	left->right->Parent = node;
	left->right = node;
}

void CBinaryTree::TreeBalance(st_TREE_NODE* node)
{
	st_TREE_NODE* uncle;
	st_TREE_NODE* tmpNode = node;
	while (tmpNode->Parent->Color == RED)
	{		
		if (tmpNode == mRootNode)
			break;
		if (tmpNode->Parent->Parent->left == tmpNode->Parent)
		{
			uncle = tmpNode->Parent->Parent->right;
			if (uncle->Color == RED)
			{
				tmpNode->Parent->Parent->left->Color = BLACK;
				tmpNode->Parent->Parent->right->Color = BLACK;
				tmpNode->Parent->Parent->Color = RED;
				tmpNode = tmpNode->Parent->Parent;
			}
			else if (uncle->Color == BLACK)
			{
				if (tmpNode->Parent->right == tmpNode)
				{
					NodeLeftTurn(tmpNode->Parent);
					tmpNode->Color = BLACK;
					tmpNode->Parent->Color = RED;
					NodeRightTurn(tmpNode->Parent);
				}
				else if (tmpNode->Parent->left == tmpNode)
				{
					tmpNode->Parent->Color = BLACK;
					tmpNode->Parent->Parent->Color = RED;
					NodeRightTurn(tmpNode->Parent->Parent);
				}
			}
		}
		else if (tmpNode->Parent->Parent->right == tmpNode->Parent)
		{
			uncle = tmpNode->Parent->Parent->left;
			if (uncle->Color == RED)
			{
				tmpNode->Parent->Parent->left->Color = BLACK;
				tmpNode->Parent->Parent->right->Color = BLACK;
				tmpNode->Parent->Parent->Color = RED;
				tmpNode = tmpNode->Parent->Parent;
			}
			else if (uncle->Color == BLACK)
			{
				if (tmpNode->Parent->right == tmpNode)
				{
					NodeLeftTurn(tmpNode->Parent);
					tmpNode->Color = BLACK;
					tmpNode->Parent->Color = RED;
					NodeRightTurn(tmpNode->Parent);
				}
				else if (tmpNode->Parent->left == tmpNode)
				{
					tmpNode->Parent->Color = BLACK;
					tmpNode->Parent->Parent->Color = RED;
					NodeRightTurn(tmpNode->Parent->Parent);
				}
			}
		}
	}
	mRootNode->Color = BLACK;
}