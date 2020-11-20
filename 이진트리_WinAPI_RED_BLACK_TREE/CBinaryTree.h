#pragma once

enum NODE_COLOR
{
	BLACK = 0,
	RED
};

class CBinaryTree
{
private:
	struct st_TREE_NODE
	{		
		st_TREE_NODE* Parent;
		st_TREE_NODE* left;
		st_TREE_NODE* right;
		
		NODE_COLOR Color;

		int iData;
	};
public:
	CBinaryTree();
	~CBinaryTree();


	void InsertNode(int data);

	bool DeleteNode(int data);

	/*
	���� ��ȸ ���
	*/
	void Print();
	/*
	iSrc�� insert�ϴ� ��
	iDest�� ���� ��.
	return : 0�� ���� ��, -1�� ���� ��, �� �ܴ� ū ��.
	*/
	int CompareData(int iSrc, int iDest);

	int GetUseCount();

////WinAPI Ver

	void GDIPrint(HWND hwnd);


private:
	void SearchParentNode(int iData, st_TREE_NODE** node);

	st_TREE_NODE* FindDeleteNode(st_TREE_NODE* node);

	void InOrder(st_TREE_NODE* node, int count); // console

	void PreOrder(st_TREE_NODE* node, int leftx, int lefty, HDC hdc); // Win API Node ���

	/*
	��� ��ȸ��
	*/
	void NodeLeftTurn(st_TREE_NODE* node);

	/*
	��� ��ȸ��
	*/
	void NodeRightTurn(st_TREE_NODE* node);

	void TreeBalance(st_TREE_NODE* node);

	st_TREE_NODE* mRootNode;
	int _iNodeCount;


	st_TREE_NODE Nil;

	HBRUSH oldBrush;
	HBRUSH redBrush;
	HBRUSH blackBrush;
};