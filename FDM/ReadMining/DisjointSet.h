#pragma once
#include "Util.h"
#include <ctime>

class DisjointSet {
	// 优化：开array而不是vector
public:
	int sizeEC;
	//vector<int> vec;
	array<int, util::collen>* parr;
public:
	DisjointSet() : sizeEC(0), parr(NULL) {}
	~DisjointSet() {
		delete parr;
	}
	void clear() {
		//vec.clear();
		sizeEC = 0;
	}
	//void append(int val) {
	//	vec.push_back(val);
	//}
	void fromProduct(DisjointSet& ds1, DisjointSet& ds2) {
		parr = new array<int, util::collen>();
		//int size = ds1.vec.size();
		int size = util::collen;
		int i;
		if (ds1.sizeEC == size || ds2.sizeEC == size) {
			i = 0;
			for (auto &elem : *parr) {
				elem = i;
				++i;
			}
			sizeEC = size;
			return;
		}

		int partitionCount = 0;
		long long hashValue;
		map<long long, int>* proots = new map<long long, int>;
		map<long long, int>::iterator itFind;
		array<int, util::collen>::iterator it1, it2, itAsgn;
		for (i = 0, it1 = ds1.parr->begin(), it2 = ds2.parr->begin(), itAsgn = parr->begin(); i < size; ++i, ++it1, ++it2) {
			hashValue = util::hashRoot(*it1, *it2);
			itFind = proots->find(hashValue);
			if (itFind == proots->end()) {
				proots->insert(pair<long long, int>(hashValue, partitionCount));
				//vec.push_back(partitionCount);
				(*itAsgn) = partitionCount;
				++partitionCount;
			}
			else {
				//vec.push_back(itFind->second);
				(*itAsgn) = itFind->second;
			}
			++itAsgn;
		}
		sizeEC = partitionCount;

		delete proots;
	}
};