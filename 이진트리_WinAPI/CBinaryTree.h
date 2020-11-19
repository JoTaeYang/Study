#pragma once
class CBinaryTree
{
private:
	struct st_TREE_NODE
	{
		int iData;
		st_TREE_NODE* left;
		st_TREE_NODE* right;
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

	st_TREE_NODE* mRootNode;
	int _iNodeCount;

};