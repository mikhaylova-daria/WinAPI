#include <random>
#include <ctime>

void comp() {

	std::vector<void*> my;
	std::vector<void*> st;
	CHeapManager myHeap;
	myHeap.Create(1, 100 * 4096);
	HANDLE heap = HeapCreate(0, 1, 100 * 4096);

	time_t begining;
	time_t end;

	double myAlloc = 0;
	double stAlloc = 0;

	double myFree = 0;
	double stFree = 0;


	std::map<int, int> indexes;
	std::set<int> rand;
	std::default_random_engine generator;
	std::uniform_int_distribution<int> distribution(1, 3000);
	int weight = 0;
	while (rand.size() != 1000) {
		int n = distribution(generator);
		if (rand.insert(n).second == true) {
			weight += n;
			begining = clock();
			my.push_back(myHeap.Alloc(n));
			end = clock();
			myAlloc += (end - begining);
			begining = clock();
			st.push_back(HeapAlloc(heap, 0, n));
			end = clock();
			stAlloc += (end - begining);
			if (my[my.size() - 1] == NULL) {
				std::cout << "my heap is ended :(" << std::endl;
				//myHeap.Print();
				std::cout << "number of elements: " << my.size() << " summary weight: " << " " << weight << " weight of last element: " << n << std::endl;;
			}
			if (st[st.size() - 1] == NULL) {
				std::cout << "standart heap is ended :(" << std::endl;
				//myHeap.Print();
				std::cout << "number of elements: " << my.size() << " summary weight: " << " " << weight << " weight of last element: " << n << std::endl;;
			}
			if (my[my.size() - 1] == NULL || st[st.size() - 1] == NULL) {
				break;
			}
		}
	}

	std::set<int> forFree;
	std::uniform_int_distribution<int> distributionForInd(0, rand.size() - 1);

	for (int i = 0; i < rand.size() / 2; ++i) {
		forFree.insert(distributionForInd(generator));
	}

	std::set<int>::iterator itr = forFree.begin();
	begining = clock();
	for (; itr != forFree.end(); ++itr) {
		int i = *itr;
		HeapFree(heap, 0, st[i]);
	}
	end = clock();
	stFree = end - begining;
	
	itr = forFree.begin();
	begining = clock();
	for (; itr != forFree.end(); ++itr) {
		int i = *itr;
		myHeap.Free(my[i]);
	}
	end = clock();
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
		sizeOfBlock *= 0.95;
	}
	std::cout << "Time of my Alloc(): " << myAlloc << "\nTime of standart Alloc(): " << stAlloc << std::endl;
	std::cout << "Time of my Free(): " << myFree << "\nTime of standart Free(): " << stFree << std::endl;
	//myHeap.Print();
}