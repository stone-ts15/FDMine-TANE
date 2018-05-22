#pragma once
#include "Util.h"
#include <ctime>

using std::vector;
using std::cout;

class DisjointSet {
public:
	vector<int> vec;

public:
	DisjointSet() {
		
	}
	DisjointSet(unsigned size) : vec(size) {
		init();
	}
	int* find(int i) {
		// 优化：使用指针开vector，结束时释放
		// 优化：使用while压缩路径
		// 优化：不压缩路径
		
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

		if (*pRooti <= *pRootj) {
			*pRootj = *pRooti;
		}
		else {
			*pRooti = *pRootj;
		}
	}

	friend DisjointSet& operator * (DisjointSet& ds1, DisjointSet& ds2) {
		DisjointSet* pans = new DisjointSet();
		int size = ds1.vec.size();
		int root1, root2;

		for (int i = 0; i < size; ++i) {
			root1 = ds1.find(i);
			root2 = ds2.find(i);
			if (ds1.find(root2) == root1) {
				pans->vec.push_back(root2);
			}
			else if (ds2.find(root1) == root2) {
				pans->vec.push_back(root1);
			}
			else {
				pans->vec.push_back(-1);
			}
		}

		return *pans;
	}

private:
	void init() {
		int count = 0;
		for (auto &i : vec) {
			i = count;
			++count;
		}
	}

	

};

void testds() {
	DisjointSet ds;
	ds.vec.push_back(-100001);
	for (int i = 0; i <= 100000; ++i) {
		ds.vec.push_back(i);
	}
	/*
	double start = clock();
	cout << std::endl << (clock() - start) / CLOCKS_PER_SEC;
	*/

	DisjointSet ds1, ds2;
	ds1.vec.push_back(-3);
	ds1.vec.push_back(0);
	ds1.vec.push_back(0);
	ds1.vec.push_back(-3);
	ds1.vec.push_back(3);
	ds1.vec.push_back(3);

	ds2.vec.push_back(-2);
	ds2.vec.push_back(0);
	ds2.vec.push_back(-2);
	ds2.vec.push_back(2);
	ds2.vec.push_back(-2);
	ds2.vec.push_back(4);

	DisjointSet c = ds1 * ds2;


	double start = clock();
	// cout << ds.find(100000);

	cout << std::endl << (clock() - start) / CLOCKS_PER_SEC;
}