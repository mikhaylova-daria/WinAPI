#include <windows.h>
#include <vector>
#include <set>
#include <map> 
#include <iostream>


#define INT_FOR_PAGES 0
#define BYTE_TO_INT_FOR_BLOCK 1
#define BYTE_TO_INT_FOR_PAGE 2

// ��������� ������� ������ � ����� �����
#define SMALL_SIZE (4096 / sizeof(int))
#define MID_SIZE (1024 * 128 / sizeof(int))

class CHeapManager {
	LPVOID memoryAddress;
	int size;
	int capacity;
	std::vector<int> pages;
	//std::set<int*> smallBlocks; //������ 4Kb
	std::map<int*, int> smallBlocks;
	std::map<int*, int> midBlocks; // �� 4 �� 128 ��
	std::map<int*, int> bigBlocks; // ������ 128
	std::map<int*, int> occupiedBlocks;
	void putBlock(int* address, int _size);
	int roundSize(int _size, int flag);
	int numberOfPage(int* address); 
	void incPages(int* address, int _size, bool onlyReserve);
	void decPages(int* address, int _size);
	void* findBlock(std::map<int*, int>& blocks, int _size);
	std::pair<std::map<int*, int>*, std::map<int*, int>::iterator> findPred(int* ptr);
	std::pair<std::map<int*, int>*, std::map<int*, int>::iterator> findFollow(int* ptr);
	void merge(int* first, int* next);
public:
	CHeapManager(){};
	~CHeapManager(){};
	void Create(int minSize, int maxSize);
	void* Alloc(int size);
	void Free(void* mem);
	void Destroy();
	void Print();
};