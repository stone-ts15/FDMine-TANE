#pragma once
#include "Util.h"
using namespace util;

class ECSet {
public:
	vector<vector<int>>* equivalentClassSet;
	int sizeEC;
	int sizeNDEC;
public:
	ECSet() : sizeEC(0), sizeNDEC(0) {
		equivalentClassSet = new vector<vector<int>>();
	}
public:
	int cardinality() {
		return sizeEC;
	}
	
	void fromExisted(ECSet& src) {
		equivalentClassSet = src.equivalentClassSet;
		sizeEC = src.sizeEC;
		sizeNDEC = src.sizeNDEC;
	}

	void fromProduct(const ECSet& ecs1, const ECSet& ecs2, vector<int>& T) {
		if (ecs1.sizeNDEC == 0 || ecs2.sizeNDEC == 0) {
			sizeNDEC = 0;
			sizeEC = ecs1.sizeEC;
			return;
		}
		equivalentClassSet->clear();

		// S
		vector<int> *S = new vector<int>[ecs1.sizeNDEC];

		// T
		int root;
		vector<int> *origin;
		int inecCount = 0;

		int ec1ID = 0, ec2ID = 0;
		for (auto &ec : *ecs1.equivalentClassSet) {
			for (auto &row : ec) {
				T[row] = ec1ID;
			}
			++ec1ID;
		}

		for (auto &ec : *ecs2.equivalentClassSet) {
			for (auto &row : ec) {
				root = T[row];
				if (root != -1) {
					S[root].push_back(row);
				}
			}
			for (auto &row : ec) {
				root = T[row];
				if (root != -1) {
					origin = S + root;
					if (origin->size() >= 2) {
						equivalentClassSet->push_back(*origin);
						inecCount += origin->size();
					}
					origin->clear();
				}
			}
		}

		sizeNDEC = equivalentClassSet->size();
		sizeEC = collen - inecCount + sizeNDEC;
		delete[] S;

		// reset T
		for (auto &ec : *ecs1.equivalentClassSet) {
			for (auto &row : ec) {
				T[row] = -1;
			}
		}
	}
};