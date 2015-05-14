#include "CHeapManager.h"
#include "comp.h"
CHeapManager heap;

template<int S>
class Test {
	char x[S];
public:
	Test(){ ; }
	~Test(){}
	void *operator new(size_t size){
		void *p = heap.Alloc(size);
		return p;
	}

		void operator delete(void *p) {
		heap.Free(p);
	}
};



int main() {
	int n;
	comp();
	//heap.Create(16, 4096 * 1000);
	//Test<17> * t1[100];
	//Test<450>* t2[100];
	//for (int i = 0; i < 100; i++) {
	//	t1[i] = new Test<17>();
	//	t2[i] = new Test<450>();
	//}
	//for (int i = 0; i < 5; ++i) {
	//	delete(t2[i * 2]);
	//	delete(t1[i * 3]);
	//}

	//for (int i = 0; i < 50; ++i) {
	//	delete(t2[i + 46]);
	//	delete(t1[i + 46]);
	//}
	////heap.Print();
	//heap.Destroy();

	std::cin >> n;
	return 0;
}