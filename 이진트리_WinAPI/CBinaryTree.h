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
	중위 순회 출력
	*/
	void Print();
	/*
	iSrc는 insert하는 값
	iDest는 비교할 값.
	return : 0은 같은 값, -1은 작은 값, 그 외는 큰 값.
	*/
	int CompareData(int iSrc, int iDest);

	int GetUseCount();

////WinAPI Ver

	void GDIPrint(HWND hwnd);


private:
	void SearchParentNode(int iData, st_TREE_NODE** node);

	st_TREE_NODE* FindDeleteNode(st_TREE_NODE* node);

	void InOrder(st_TREE_NODE* node, int count); // console

	void PreOrder(st_TREE_NODE* node, int leftx, int lefty, HDC hdc); // Win API Node 출력

	st_TREE_NODE* mRootNode;
	int _iNodeCount;

};