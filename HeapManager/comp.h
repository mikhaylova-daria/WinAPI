#include <random>
#include <ctime>

void comp() {

	std::vector<void*> my;
	std::vector<void*> st;
	CHeapManager myHeap;
	myHeap.Create(1, 100 * 4096);
	HANDLE heap = HeapCreate(0, 1, 100 * 4096);

	int begining;
	int end;

	int myAlloc = 0;
	int stAlloc = 0;

	int myFree = 0;
	int stFree = 0;


	std::map<int, int> indexes;
	std::set<int> rand;
	std::default_random_engine generator;
	std::uniform_int_distribution<int> distribution(1, 3000);
	int weight = 0;
	std::vector<int> weights;
	while (weights.size() != 1000) {
		int n = distribution(generator);
		if (rand.insert(n).second == true) {
			weights.push_back(n);
		}
	}

	begining = __rdtsc();;
	int i;
	for (i = 0; i < 1000; ++i) {
		my.push_back(myHeap.Alloc(weights[i]));
		if (my[my.size() - 1] == NULL) {
			end = __rdtsc();;
			myAlloc = (end - begining);
			std::cout << "my heap is ended :(" << std::endl;
			//myHeap.Print();
			std::cout << "number of elements: " << my.size() << std::endl;
			break;
		}
	}
	if (i == 1000) {
		end = __rdtsc();
		myAlloc = (end - begining);
	}

	int minsize = my.size();
	begining = __rdtsc();
	for (i = 0; i < minsize; ++i) {
		st.push_back(HeapAlloc(heap, 0, weights[i]));
		if (st[st.size() - 1] == NULL) {
			end = __rdtsc();
			stAlloc = (end - begining);
			std::cout << "standart heap is ended :(" << std::endl;
			//myHeap.Print();
			std::cout << "number of elements: " << my.size() << std::endl;
			if (minsize > st.size()) {
				minsize = st.size();
			}
			break;
		}
	}
	if (i == minsize) {
		end = __rdtsc();
		stAlloc = (end - begining);
	}


	std::set<int> forFree;
	std::uniform_int_distribution<int> distributionForInd(0, minsize - 1);

	for (i = 0; i < minsize / 2; ++i) {
		forFree.insert(distributionForInd(generator));
	}

	std::set<int>::iterator itr = forFree.begin();
	begining = __rdtsc();
	for (; itr != forFree.end(); ++itr) {
		int i = *itr;
		HeapFree(heap, 0, st[i]);
	}
	end = __rdtsc();
	stFree = end - begining;
	
	itr = forFree.begin();
	begining = __rdtsc();
	for (; itr != forFree.end(); ++itr) {
		int i = *itr;
		myHeap.Free(my[i]);
	}
	end = __rdtsc();
	myFree = end - begining;
	myHeap.Print();
	int sizeOfBlock = 100 * 4096;
	bool flag = true;
	void* my_p;
	void* st_p;
	while (flag && (sizeOfBlock != 0)) {
		my_p = myHeap.Alloc(sizeOfBlock);
		st_p = HeapAlloc(heap, 0, sizeOfBlock);
		if (my_p != NULL) {
			flag = false;
			std::cout << "In my heap insert block of size: " << sizeOfBlock << std::endl;
		}
		if (st_p != NULL) {
			flag = false;
			std::cout << "In standart heap insertblock of size: " << sizeOfBlock << std::endl;
		}
		sizeOfBlock *= 0.99;
	}
	std::cout << "Time of my Alloc(): " << myAlloc << " for " << my.size() << " elements " << "\nTime of standart Alloc(): " << stAlloc << " for " << st.size() << " elements" << std::endl;
	std::cout << "Time of my Free(): " << myFree << "\nTime of standart Free(): " << stFree << std::endl;

}