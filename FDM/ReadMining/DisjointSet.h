#pragma once
#include "Util.h"
#include <ctime>
using namespace util;

class DisjointSet {
public:
	int sizeEC;
	array<int, collen>* parr;
public:
	DisjointSet() : sizeEC(0), parr(NULL) {}
	~DisjointSet() {
		delete parr;
	}
	void clear() {
		sizeEC = 0;
	}
	void fromProduct(DisjointSet& ds1, DisjointSet& ds2) {
		int size = collen;
		
		// if super key
		if (ds1.sizeEC == size || ds2.sizeEC == size) {
			sizeEC = size;
			return;
		}

		parr = new array<int, collen>();
		hashRoots.clear();
		singleRoots.clear();

		int                                         i;
		int                                         partitionCount = 0;
		long long                                   hashValue;
		array<int, collen>::iterator                it1, it2, itAsgn;
		unordered_map<long long, int>::iterator     itHFind;

		for (i = 0, it1 = ds1.parr->begin(), it2 = ds2.parr->begin(), itAsgn = parr->begin(); i < size; ++i, ++it1, ++it2, ++itAsgn) {
			if (*it1 == -1 || *it2 == -1) {
				(*itAsgn) = -1;
				++partitionCount;
				continue;
			}
			hashValue = hashRoot(*it1, *it2);
			itHFind = hashRoots.find(hashValue);
			if (itHFind == hashRoots.end()) {
				hashRoots.insert(pair<long long, int>(hashValue, partitionCount));
				singleRoots.insert(partitionCount);
				(*itAsgn) = partitionCount;
				++partitionCount;
			}
			else {
				(*itAsgn) = itHFind->second;
				singleRoots.erase(itHFind->second);
			}
		}
		

		// let distinct element be -1
		set<int>::iterator itD;
		for (auto &r : *parr) {

			if (r == -1)
				continue;
			itD = singleRoots.find(r);
			if (itD != singleRoots.end())
				r = -1;
		}


		sizeEC = partitionCount;
		hashRoots.clear();
		singleRoots.clear();
	}
};