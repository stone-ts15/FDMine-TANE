#pragma once
#include "Util.h"
#include <ctime>

using std::vector;
using std::cout;

class DisjointSet {
	// 优化：开array而不是vector
public:
	int sizeEC;
private:
	vector<int> vec;
	

public:
	DisjointSet() : sizeEC(0) {
		
	}
	DisjointSet(unsigned size) : vec(size) {
		initTest();
	}
	void clear() {
		vec.clear();
		sizeEC = 0;
	}
	int* find(int i) {
		// 优化：使用指针开vector，结束时释放
		// 优化：使用while压缩路径
		// 优化：不压缩路径
		// 优化：是否返回iterator
		vector<int*> oldElems;
		
		int* root;
		while (true) {
			root = &(vec[i]);

			// root found directly
			if (*root == i) {
				for (auto p : oldElems) {
					*p = i;
				}
				return root;
			}
			oldElems.push_back(root);
			i = *root;
		}
	}

	void join(int i, int j) {
		int *pRooti = find(i);
		int *pRootj = find(j);

		if (*pRooti != *pRootj) {
			--sizeEC;
			if (*pRooti < *pRootj) {
				*pRootj = *pRooti;
			}
			else {
				*pRooti = *pRootj;
			}
		}
	}

	friend DisjointSet& productOf (DisjointSet& ds1, DisjointSet& ds2) {
		// 优化：返回指针，否则需要拷贝
		DisjointSet* pResultSet = new DisjointSet();
		int size = ds1.vec.size();
		int *root1, *root2;

		for (int i = 0; i < size; ++i) {
			root1 = ds1.find(i);
			root2 = ds2.find(i);
			if (*(ds1.find(*root2)) == *root1) {
				pResultSet->vec.push_back(*root2);
			}
			else if (*(ds2.find(*root1)) == *root2) {
				pResultSet->vec.push_back(*root1);
			}
			else {
				pResultSet->vec.push_back(i);
			}
		}

		return *pResultSet;
	}

	friend void getProductFrom(DisjointSet& ds1, DisjointSet& ds2, DisjointSet& result) {
		int size = ds1.vec.size();
		int *root1, *root2;
		int sizeEC = 0;

		for (int i = 0; i < size; ++i) {
			root1 = ds1.find(i);
			root2 = ds2.find(i);
			if (*(ds1.find(*root2)) == *root1) {
				result.append(*root2);
				if (*root2 == i)
					++sizeEC;
			}
			else if (*(ds2.find(*root1)) == *root2) {
				result.append(*root1);
				if (*root1 == i)
					++sizeEC;
			}
			else {
				result.append(i);
				sizeEC++;
			}
		}

		result.sizeEC = sizeEC;
	}

	void append(int val) {
		vec.push_back(val);
	}

private:
	// just for test
	void initTest() {
		int count = 0;
		for (auto &i : vec) {
			i = count;
			++count;
		}
	}

	

};

void testds() {
	DisjointSet ds;
	ds.append(-100001);
	for (int i = 0; i <= 100000; ++i) {
		ds.append(i);
	}
	/*
	double start = clock();
	cout << std::endl << (clock() - start) / CLOCKS_PER_SEC;
	*/

	DisjointSet ds1, ds2;
	ds1.append(0);
	ds1.append(0);
	ds1.append(0);
	ds1.append(3);
	ds1.append(3);
	ds1.append(3);

	ds2.append(0);
	ds2.append(0);
	ds2.append(2);
	ds2.append(2);
	ds2.append(4);
	ds2.append(4);

	DisjointSet *c = &productOf(ds1, ds2);


	double start = clock();
	// cout << ds.find(100000);

	cout << std::endl << (clock() - start) / CLOCKS_PER_SEC;
}