#include "CHeapManager.h"
#include "comp.h"
CHeapManager heap;

template<int S>
class Test {
	char x[S];
public:
	Test() {
		std::cout << "Constructot Test<"<<S<<">" << std::endl;
	}
	void *operator new(size_t size){
		void *p = heap.Alloc(size);
		(*(Test*)p) = Test();
		return p;
	}

	void operator delete(void *p) {
		heap.Free(p);
	}
};



int main() {
	comp();
	/*heap.Create(16, 4096 * 1000);
	Test<17> * t1[100];
	Test<450>* t2[100];
	for (int i = 0; i < 100; i++) {
		t1[i] = new Test<17>();
		t2[i] = new Test<450>();
	}
	heap.Print();
	
	for (int i = 0; i < 5; ++i) {
		delete(t1[i * 3]);
		delete(t2[i * 2]);
	}
	heap.Print();
	
	for (int i = 0; i < 50; ++i) {
		delete(t2[i + 46]);
		delete(t1[i + 46]);
	}
	heap.Print();
	heap.Destroy();*/
	return 0;
}