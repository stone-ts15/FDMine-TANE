#pragma once
#include "Util.h"
#include <ctime>
using namespace util;

class DisjointSet {
public:
	int sizeEC;
	int sizeNDEC;
	array<int, collen>* parr;
public:
	DisjointSet() : sizeEC(0), parr(NULL) {}
	~DisjointSet() {
		if (parr)
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
			if (singleRoots.find(r) != singleRoots.end())
				r = -1;
		}


		sizeEC = partitionCount;
		hashRoots.clear();
		singleRoots.clear();
	}

	void fromTable(const Database& db, int index) {
		parr = new array<int, util::collen>();
		unordered_map<string, pair<int, int>>::iterator itFind;
		const array<string, util::collen>& column = db.table[index];
		array<int, util::collen>::iterator itAsgn = parr->begin();
		int row = 0;
		int partitionCount = 0;
		pair<int, int>* pInfo;
		for (auto &str : column) {
			itFind = smap.find(str);
			if (itFind == smap.end()) {
				smap[str] = pair<int, int>(hashRow(row), 0);
				*itAsgn = -1;
			}
			else {
				pInfo = &(itFind->second);
				if (pInfo->first < 0) {
					pInfo->first = inverseHashRow(pInfo->first);
					pInfo->second = partitionCount;
					(*parr)[pInfo->first] = partitionCount;
					++partitionCount;
				}
				*itAsgn = pInfo->second;
			}

			++row;
			++itAsgn;
		}
		sizeNDEC = partitionCount;
		sizeEC = smap.size();
		//cout << index << ": " << sizeEC << endl;

		smap.clear();
	}
};

class PartSet {
public:
	vector<pair<int, int>> *pvec;
	map<int, int> *pmap;
	int sizeEC;
public:
	PartSet() : pvec(NULL), sizeEC(0) {}
	~PartSet() {
		if (pvec)
			delete pvec;
	}

	void fromDatabase(const Database& db, int index) {
		pmap = new map<int, int>();
		int size = collen;
		int row = 0;
		int originRow;
		const array<string, util::collen>& column = db.table[index];
		//admap.clear();
		unordered_map<string, int>::iterator itFind;
		for (auto &str : column) {
			itFind = admap.find(str);
			if (itFind == admap.end()) {
				admap.insert(pair<string, int>(str, hashRow(row)));
			}
			else {
				--size;
				// inverse
				if (itFind->second < 0) {
					originRow = inverseHashRow(itFind->second);
					itFind->second = originRow;
					pmap->insert(pair<int, int>(originRow, originRow));
				}
				else {
					originRow = itFind->second;
				}
				pmap->insert(pair<int, int>(row, originRow));
			}
			++row;
		}
		sizeEC = size;
		admap.clear();
		cout << index << ": " << sizeEC - collen + pmap->size() << endl;
	}

	void fromProduct(const PartSet& ps1, const PartSet& ps2) {
		// fake

		// p1 = ps1.begin, p2 = ps2.begin
		// while p1.row != p2.row:
		//   drop min(p1, p2)
		//   min(p1, p2)++
		// (p1 == p2)
		// hash = p1.root & p2.root
		// if map.find(hash):
		//   this.push(p1.row, map[hash])
		// else:
		//   this.push(p1.row, count)
		//   ++count
		// (reorder)
		// drop distinct row
		



		singleRoots.clear();
	}



	/*void getPartitionFromTable(const Database& db, const AttributeSet& attr) {
		// 优化：toVector返回int*，减少一次赋值
		// 优化：一次取出table的列or取多次
		// 优化：直接向map的end插入,*itFind = pair<>...
		as = attr;
		singleRoots.clear();
		partition.parr = new array<int, util::collen>();

		vector<int> attrVec = attr.toVector();
		unordered_map<string, int>::iterator itFind;
		array<int, util::collen>::iterator itAsgn = partition.parr->begin();
		int partitionCount = 0;

		// only when 1 attr in AttributeSet
		for (auto &index : attrVec) {
			const array<string, util::collen>& column = db.table[index];
			partitionCount = 0;
			equivalenceClass.clear();
			for (auto& str : column) {
				itFind = equivalenceClass.find(str);
				if (itFind == equivalenceClass.end()) {
					equivalenceClass.insert(pair<string, int>(str, partitionCount));
					singleRoots.insert(partitionCount);
					(*itAsgn) = partitionCount;
					++partitionCount;
				}
				else {
					(*itAsgn) = itFind->second;
					singleRoots.erase(itFind->second);
				}
				++itAsgn;
			}
		}

		for (auto &r : *partition.parr) {
			if (singleRoots.find(r) != singleRoots.end())
				r = -1;
		}

		partition.sizeEC = partitionCount;
		equivalenceClass.clear();
		singleRoots.clear();
	}
*/
};

class ECSet {
public:
	//vector<unordered_set<int>> equivalentClassSet;
	vector<vector<int>> equivalentClassSet;
	//unordered_set<int> *pCandidate;
	//unordered_map<int, int> *pRoots;
	int sizeEC;
	int sizeNDEC;
public:
	ECSet() : sizeEC(0), sizeNDEC(collen) {

	}
public:
	int cardinality() {
		return sizeEC;
	}
	void fromTable(const Database& db, int index, unordered_map<string, int>& colmap) {
		//array<string, util::collen>::const_iterator it = db.table[index].cbegin();
		const array<string, util::collen>& column = db.table[index];
		unordered_map<string, int>::iterator itFind;
		vector<int> *newset = NULL;
		int row = 0;
		sizeNDEC = 0;
		int hashedRow;

		for (auto &str : column) {
			itFind = colmap.find(str);
			if (itFind == colmap.end()) {
				colmap[str] = hashRow(row);
			}
			else {
				hashedRow = itFind->second;
				if (hashedRow < 0) {
					newset = new vector<int>();
					newset->push_back(inverseHashRow(hashedRow));
					newset->push_back(row);
					equivalentClassSet.push_back(*newset);
					itFind->second = sizeNDEC;
					++sizeNDEC;
				}
				else {
					equivalentClassSet[hashedRow].push_back(row);
				}
			}

			++row;
		}

		//sizeNDEC = equivalentClassSet.size();
		sizeEC = colmap.size();
	}

	void fromProduct(const ECSet& ecs1, const ECSet& ecs2, vector<int>& pRoots) {
		if (ecs1.sizeNDEC == 0 || ecs2.sizeNDEC == 0) {
			sizeNDEC = 0;
			sizeEC = collen;
			return;
		}
		equivalentClassSet.clear();
		//unordered_map<int, int>::iterator itFind;

		// S
		//unordered_set<int> *pCandidate = new unordered_set<int>[ecs1.sizeNDEC];
		vector<int> *pCandidate = new vector<int>[ecs1.sizeNDEC];

		// T
		//int *pRoots = new int[collen];
		
		
		//memset(pRoots, 0, sizeof(pRoots));
		int root;
		unordered_set<int> *origin;
		int inecCount = 0;

		int ec1ID = 0, ec2ID = 0;
		for (auto &ec : ecs1.equivalentClassSet) {
			for (auto &row : ec) {
				//(*pRoots)[row] = ec1ID;
				pRoots[row] = ec1ID;
			}
			++ec1ID;
		}

		for (auto &ec : ecs2.equivalentClassSet) {
			for (auto &row : ec) {
				//itFind = pRoots->find(row);
				//if (itFind != pRoots->end()) {
				//	pCandidate[itFind->second].insert(row);
				//}
				root = pRoots[row];
				if (root != -1) {
					pCandidate[root].push_back(row);
				}
			}
			for (auto &row : ec) {
				root = pRoots[row];
				if (root != -1) {
					//origin = pCandidate + root;
					if (pCandidate[root].size() >= 2) {
						equivalentClassSet.push_back(pCandidate[root]);
						inecCount += pCandidate[root].size();
					}
					pCandidate[root].clear();
				}
			}
		}

		sizeNDEC = equivalentClassSet.size();
		sizeEC = collen - inecCount;
		delete[] pCandidate;
		//delete pRoots;
	}
};