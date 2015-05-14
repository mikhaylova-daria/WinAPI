#include "CHeapManager.h"
#include <functional>


struct less : std::binary_function <int*, int*, bool> {
	bool operator() (const int*& x, const int*& y) const { return x<y; }
};

void CHeapManager::Create(int minSize, int maxSize) {
	memoryAddress = VirtualAlloc(NULL, maxSize, MEM_RESERVE, PAGE_READWRITE);
	VirtualAlloc(memoryAddress, minSize, MEM_COMMIT, PAGE_READWRITE);
	size = roundSize(minSize, BYTE_TO_INT_FOR_PAGE);
	capacity = roundSize(maxSize, BYTE_TO_INT_FOR_PAGE);
	std::cout << "size: " << size*sizeof(int) << " capacity: " << capacity*sizeof(int) << std::endl;
	pages.resize(capacity * sizeof(int) / 4096, -1);
	incPages(reinterpret_cast<int*>(memoryAddress), size, TRUE);
	putBlock(reinterpret_cast<int*>(memoryAddress), capacity);
}

void* CHeapManager::Alloc(int _size) {

	_size = roundSize(_size, BYTE_TO_INT_FOR_BLOCK);
	void* result = NULL;

	if (_size < SMALL_SIZE) {
		result = findBlock(smallBlocks, _size);
	}

	if (_size <= MID_SIZE && result == NULL) {
		result = findBlock(midBlocks, _size);
	}

	if (result == NULL) {
		result = findBlock(bigBlocks, _size);
	}

	return result;
}

void CHeapManager::Free(void* mem) {

	std::map<int*, int>::iterator itr = occupiedBlocks.find(reinterpret_cast<int*>(mem));
	if (itr == occupiedBlocks.end()) {
		throw "Error of Free";
	}

	decPages(itr->first, itr->second);

	int* addr = itr->first;

	std::pair<std::map<int*, int>*, std::map<int*, int>::iterator> pred = findPred(addr);
	std::pair<std::map<int*, int>*, std::map<int*, int>::iterator> fll = findFollow(addr);
	int* newAddress = addr;
	int newSize = itr->second;
	//попробовали слить с предыдущим

	if (pred.second != pred.first->end()) {
		newAddress = pred.second->first;
		newSize = pred.second->second;
		if (newAddress + newSize == addr) {
			newSize += itr->second;
			pred.first->erase(pred.second);
		}
		else {
			newAddress = addr;
			newSize = itr->second;
		}
	}
	
	//попробовали слить со следуюшим
	if (fll.second != fll.first->end()) {
		if (newAddress + newSize == fll.second->first) {
			newSize += fll.second->second;
			fll.first->erase(fll.second);
		}
	}
	//положили получившийся пустой блок в сеты
	putBlock(newAddress, newSize);
	occupiedBlocks.erase(itr);

}


void CHeapManager::Destroy() {
	std::map<int*, int>::iterator itr;
	std::cout << "unfree: \n";
	for (itr = occupiedBlocks.begin(); itr != occupiedBlocks.end(); ++itr) {
		std::cout << itr->first << " " << itr->second << std::endl;;
	}
	for (int i = 0; i < pages.size(); ++i) {
		if (pages[i] > -1) {
			VirtualFree(reinterpret_cast<char*>(memoryAddress)+i * 4096, 4096, MEM_DECOMMIT);
			pages[i] = -1;
		}
	}
	VirtualFree(memoryAddress, 4096 * pages.size(), MEM_RELEASE);
}



void CHeapManager::Print() {
	std::map<int*, int>::iterator itr;
	itr = occupiedBlocks.begin();
	std::cout << "\n\n occupiedBlocks:" << std::endl;
	for (; itr != occupiedBlocks.end(); ++itr) {
	std::cout << itr->first << " " << itr->second << std::endl;
	}
	itr = smallBlocks.begin();
	std::cout << " smallBlocks:" << std::endl;
	for (; itr != smallBlocks.end(); ++itr) {
		std::cout << itr->first << " " << itr->second << std::endl;
	}
	itr = midBlocks.begin();
	std::cout << " midBlocks:" << std::endl;
	for (; itr != midBlocks.end(); ++itr) {
		std::cout << itr->first << " " << itr->second << std::endl;
	}
	itr = bigBlocks.begin();
	std::cout << " bigBlocks:" << std::endl;
	for (; itr != bigBlocks.end(); ++itr) {
		std::cout << itr->first << " " << itr->second << std::endl;
	}
}

std::pair<std::map<int*, int>*, std::map<int*, int>::iterator> CHeapManager::findPred(int* ptr) {
	//std::cout << "findP" << std::endl;

	std::map<int*, int>::iterator itr;
	std::map<int*, int>::iterator itr1;
	std::map<int*, int>::iterator itr2;
	std::map<int*, int>::iterator itr3;
	std::map<int*, int>* blocks;
	int* ptr1 = 0;
	int* ptr2 = 0;
	int* ptr3 = 0;
	itr1 = smallBlocks.lower_bound(ptr);
	itr2 = midBlocks.lower_bound(ptr);
	itr3 = bigBlocks.lower_bound(ptr);
	if (itr1 != smallBlocks.begin()) {
		--itr1;
		ptr1 = itr1->first;
	}
	if (itr2 != midBlocks.begin()) {
		--itr2;
		ptr2 = itr2->first;
	}
	if (itr3 != bigBlocks.begin()) {
		--itr3;
		ptr3 = itr3->first;
	}

	int* maxPtr = ptr1;
	blocks = &smallBlocks;
	itr = itr1;
	if (maxPtr < ptr2) {
		maxPtr = ptr2;
		blocks = &midBlocks;
		itr = itr2;
	}

	if (maxPtr < ptr3) {
		maxPtr = ptr3;
		blocks = &bigBlocks;
		itr = itr3;
	}
	if (maxPtr == 0) {
		itr = blocks->end();
	}
	return std::pair<std::map<int*, int>*, std::map<int*, int>::iterator>(blocks, itr);
}


std::pair<std::map<int*, int>*, std::map<int*, int>::iterator> CHeapManager::findFollow(int* ptr) {
	//std::cout << "findF" << std::endl;

	std::map<int*, int>::iterator itr;
	std::map<int*, int>::iterator itr1;
	std::map<int*, int>::iterator itr2;
	std::map<int*, int>::iterator itr3;
	std::map<int*, int>* blocks;
	int* ptr1 = reinterpret_cast<int*> (memoryAddress) + capacity / sizeof(int);
	int* ptr2 = reinterpret_cast<int*> (memoryAddress) + capacity / sizeof(int);
	int* ptr3 = reinterpret_cast<int*> (memoryAddress) + capacity / sizeof(int);
	itr1 = smallBlocks.upper_bound(ptr);
	itr2 = midBlocks.upper_bound(ptr);
	itr3 = bigBlocks.upper_bound(ptr);
	if (itr1 != smallBlocks.end()) {
		ptr1 = itr1->first;
	}
	if (itr2 != midBlocks.end()) {
		ptr2 = itr2->first;
	}
	if (itr3 != bigBlocks.end()) {
		ptr3 = itr3->first;
	}

	int* minPtr = ptr1;
	blocks = &smallBlocks;
	itr = itr1;
	if (minPtr > ptr2) {
		minPtr = ptr2;
		blocks = &midBlocks;
		itr = itr2;
	}

	if (minPtr > ptr3) {
		minPtr = ptr3;
		blocks = &bigBlocks;
		itr = itr3;
	}
	return std::pair<std::map<int*, int>*, std::map<int*, int>::iterator>(blocks, itr);
}

//размер в интах!
void CHeapManager::putBlock(int* address, int _size) {
	//std::cout << "putBlock" << std::endl;
	if (_size < SMALL_SIZE) {
		smallBlocks.insert(std::pair<int*, int>(address, _size));
	}
	else if (_size <= MID_SIZE) {
		midBlocks.insert(std::pair<int*, int>(address, _size));
	}
	else {
		bigBlocks.insert(std::pair<int*, int>(address, _size));
	}
}

// возвращает в числе интов размер памяти на целое число страниц
int CHeapManager::roundSize(int _size, int flag) {
	//std::cout << "roundSize" << std::endl;

	int intInPage = 4096 / sizeof(int);
	if (flag == INT_FOR_PAGES) {
		if (_size % intInPage != 0) {
			return (_size / intInPage + 1) * intInPage;
		}
		else {
			return _size;
		}
	}
	if (flag == BYTE_TO_INT_FOR_BLOCK) {
		if (_size % sizeof(int) != 0) {
			return _size / sizeof(int) + 1;
		}
		else {
			return _size / sizeof(int);
		}
	}
	if (flag == BYTE_TO_INT_FOR_PAGE) {
		int sizeInInt;
		if (_size % sizeof(int) != 0) {
			sizeInInt = _size / sizeof(int) + 1;
		}
		else {
			sizeInInt = _size / sizeof(int);
		}
		if (sizeInInt % intInPage != 0) {
			return (sizeInInt / intInPage + 1) * intInPage;
		}
		else {
			return sizeInInt;
		}
	}
}

int CHeapManager::numberOfPage(int* address) {
	//std::cout << "numberOfP" << std::endl;

	return (address - reinterpret_cast<int*> (memoryAddress)) / (4096 / sizeof(int));
}

//размер в интах!
void CHeapManager::incPages(int* address, int _size, bool onlyReserve = FALSE) {
	//std::cout << "incP" << std::endl;

	int i = numberOfPage(address);
	int f = numberOfPage(address + _size);
	for (; i < f; ++i) {
		if (pages[i] == -1) {
			VirtualAlloc(reinterpret_cast<char*>(memoryAddress)+(i * 4096), 4096, MEM_COMMIT, PAGE_READWRITE);
			++pages[i];
		}
		if (!onlyReserve) {
			++pages[i];
		}
	}
}

void CHeapManager::decPages(int* address, int _size) {
	//std::cout << "decP" << std::endl;

	int i = numberOfPage(address);
	int f = numberOfPage(address + _size);
	for (; i < f; ++i) {
		--pages[i];
		if ((pages[i] == 0) && (i > size * sizeof(int) / 4096)) {
			VirtualFree(reinterpret_cast<char*>(memoryAddress)+i * 4096, 4096, MEM_DECOMMIT);
			--pages[i];
		}
	}
}

void* CHeapManager::findBlock(std::map<int*, int>& blocks, int _size){
	//std::cout << "findBlock" << std::endl;
	std::map<int*, int>::iterator itr = blocks.begin();
	while (itr != blocks.end()) {
		if (itr->second >= _size) {
			int* address = itr->first;
			if (itr->second > _size) {
				int newSize = itr->second - _size;
				int* newAddress = address + _size; //начало нового блока
				putBlock(newAddress, newSize);
			}
			blocks.erase(itr);
			incPages(address, _size);
			occupiedBlocks.insert(std::pair<int*, int>(address, _size));
			return (void*)(address);
		}
		++itr;
	}
	return NULL;
}
