#include <iostream>
#include <Windows.h>
#include "CBinaryTree.h"

CBinaryTree::CBinaryTree()
{
	mRootNode = NULL;
	_iNodeCount = 0;

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
		Ellipse(hdc, leftx, lefty, rightX, rightY);//출력
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
	insert->left = NULL;
	insert->right = NULL;

	if (mRootNode == NULL)
	{
		mRootNode = insert;
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
				break;
			}
		}
		else
			break;
	}
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