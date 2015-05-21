#include <windows.h>
#include <vector>
#include <set>
#include <map> 
#include <iostream>


const int INT_FOR_PAGES = 0;
const int BYTE_TO_INT_FOR_BLOCK = 1;
const int BYTE_TO_INT_FOR_PAGE = 2;


class CHeapManager {
	int SMALL_SIZE;
	int MID_SIZE;
	LPVOID memoryAddress;
	int size;
	int capacity;
	std::vector<int> pages;
	std::map<int*, int> smallBlocks;//блоки, умещающиеся на странице
	std::map<int*, int> midBlocks; // блоки, умещающиеся в 32 страницы
	std::map<int*, int> bigBlocks; // больше 32 страниц
	std::map<int*, int> occupiedBlocks;
	void putBlock(int* address, int _size);
	int roundSize(int _size, int flag);
	int numberOfPage(int* address); 
	void incPages(int* address, int _size, bool onlyReserve);
	void decPages(int* address, int _size);
	void* findBlock(std::map<int*, int>& blocks, int _size);
	std::pair<std::map<int*, int>*, std::map<int*, int>::iterator> findPred(int* ptr);
	std::pair<std::map<int*, int>*, std::map<int*, int>::iterator> findFollow(int* ptr);
	int pageSize;

public:
	CHeapManager(){};
	~CHeapManager(){};
	void Create(int minSize, int maxSize, int small_size = 0, int mid_size = 0);
	void* Alloc(int size);
	void Free(void* mem);
	void Destroy();
	void Print();
};