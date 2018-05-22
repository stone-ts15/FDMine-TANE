#pragma once
#include "Util.h"
#include <ctime>

class DisjointSet {
	// 优化：开array而不是vector
public:
	int sizeEC;
private:
	vector<int> vec;
public:
	DisjointSet() : sizeEC(0) {}
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
	void append(int val) {
		vec.push_back(val);
	}
	void fromProduct(DisjointSet& ds1, DisjointSet& ds2) {
		clear();

		int size = ds1.vec.size();
		int *root1, *root2;
		int partitionCount = 0;
		long long hashValue;
		

		map<long long, int>* proots = new map<long long, int>;
		map<long long, int>::iterator itFind;
		for (int i = 0; i < size; ++i) {
			// root1 = ds1.find(i);
			// root2 = ds2.find(i);
			root1 = &(ds1.vec[i]);
			root2 = &(ds2.vec[i]);
			hashValue = util::hashRoot(*root1, *root2);
			itFind = proots->find(hashValue);
			if (itFind == proots->end()) {
				proots->insert(pair<long long, int>(hashValue, partitionCount));
				vec.push_back(partitionCount);
				++partitionCount;
			}
			else {
				vec.push_back(itFind->second);
			}
			
		}
		sizeEC = partitionCount;
		
		delete proots;
	}
};