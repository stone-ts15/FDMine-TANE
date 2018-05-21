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
	int find(int i) {
		vector<int*> oldElems;
		int* root;
		while (true) {
			root = &(vec[i]);

			// root found directly
			if (*root < 0) {
				for (auto p : oldElems) {
					*p = i;
				}
				return i;
			}
			oldElems.push_back(root);
			i = *root;
		}
	}

	void join(int i, int j) {
		int ri = find(i);
		int rj = find(j);
		if (ri == rj)
			return;

		int &vi = vec[ri];
		int &vj = vec[rj];
		if (vi <= vj) {
			vi = vi + vj;
			vj = ri;
		}
		else {
			vj = vi + vj;
			vi = rj;
		}
	}

	friend const DisjointSet& operator * (const DisjointSet& ds1, const DisjointSet& ds2) {
		DisjointSet* pans = new DisjointSet();



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

	double start = clock();
	// cout << ds.find(100000);

	cout << std::endl << (clock() - start) / CLOCKS_PER_SEC;
}