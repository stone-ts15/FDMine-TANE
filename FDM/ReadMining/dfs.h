#pragma once

#include "Util.h"
#include "DisjointSet.h"
#include "TANE-tree.h"
#include "AttributeSet.h"

class S {
public:
	int col;
	vector<AttributeSet> dependency;
	vector<int> sing;
	Database* pdb;
	DisjointSet* pds;
	unordered_map<long long, int> gmap;
	bool dep[15][1 << 16];
	ofstream fout;
public:
	S(int vcol) : col(vcol), pdb(NULL), dependency(vcol), sing(vcol), fout("result.txt", ios::out) {
		pds = new DisjointSet[vcol];
		int i = 0;
		for (auto &a : dependency) {
			a.insert(i);
			++i;
		}
		i = 0;
		for (auto &a : sing) {
			a = i;
			++i;
		}
		
	}

	void loadds(Database* vpdb) {
		pdb = vpdb;
		for (int i = 0; i < col; ++i) {
			pds[i].fromTable(*pdb, i);
		}
	}

	bool searchSingle(const AttributeSet& left, const AttributeSet& right, set<AttributeSet>& sk) {
		for (auto &s : sk) {
			if (s.in(left)) {
				return true;
			}
		}
		if (left.attribute_set == 0)
			return false;

		vector<int> leftAttrs = left.toVector();
		int size = leftAttrs.size();
		vector<DisjointSet*> leftds(size);
		int i, j, k;

		for (i = 0; i < size; ++i) {
			if (pds[leftAttrs[i]].sizeEC == 0)
				return true;
			leftds[i] = (pds + leftAttrs[i]);
		}

		//sort(leftds);

		bool issuper = true;

		vector<array<int, collen>::iterator> visit(size);
		for (i = 0; i < size; ++i) {
			visit[i] = leftds[i]->parr->begin();
		}

		array<int, collen>::iterator rGroup = pds[right.toVector()[0]].parr->begin();

		unordered_map<long long, int>::iterator itFind;
		long long hashValue;
		array<int, 4> hashArg;
		hashArg[0] = 0;
		hashArg[1] = 0;
		hashArg[2] = 0;
		hashArg[3] = 0;
		
		bool pass;
		for (i = 0; i < collen; ++i, ++rGroup) {
			pass = false;
			for (j = 0; j < size; ++j) {
				if (*(visit[j]) == -1) {
					pass = true;
				}
				hashArg[4 - size + j] = *(visit[j]);
				++visit[j];
			}
			if (pass)
				continue;
			hashValue = hashn(hashArg);
			itFind = gmap.find(hashValue);
			if (itFind == gmap.end()) {
				gmap[hashValue] = *rGroup;
			}
			else {
				issuper = false;
				if (*rGroup == -1 || itFind->second != *rGroup) {
					//delete gmap;
					gmap.clear();
					return false;
				}
			}
		}
		//delete gmap;
		gmap.clear();
		if (issuper)
			sk.insert(left);
		return true;
	}

	/*void dfs() {
		int i, j;
		int i1, i2, i3, i4;
		int size;
		int cnt = 0;
		AttributeSet right;
		AttributeSet left;
		double start = clock();
		
		
		

		for (i = 0; i < col; ++i) {
		right.clear();
		right.insert(i);

		for (j = 0; j < col; ++j) {
		if (i == j)
		continue;
		left.clear();
		left.insert(j);
		if (searchSingle(left, right)) {
		fout << j + 1 << " -> " << i + 1 << endl;
		dep[i][1 << j] = true;
		++cnt;
		//dependency[i].insert(j);
		}
		}
		}
		cout << (clock() - start) / CLOCKS_PER_SEC << endl;

		left.clear();
		right.clear();
		for (j = 0; j < col; ++j) {

		right.insert(j);

		for (i1 = 0; i1 < col; ++i1) {
		if (i1 == j || dep[j][1 << i1])
		continue;
		left.insert(i1);
		for (i2 = i1 + 1; i2 < col; ++i2) {
		if (i2 == j || dep[j][1 << i2])
		continue;
		left.insert(i2);
		if (searchSingle(left, right)) {
		fout << i1 + 1 << " " << i2 + 1 << " -> " << j + 1 << endl;
		dep[j][(1 << i1) + (1 << i2)] = true;
		++cnt;
		}
		left.erase(i2);
		}
		left.clear();
		}
		right.clear();
		}
		left.clear();
		right.clear();

		cout << (clock() - start) / CLOCKS_PER_SEC << endl;

		for (j = 0; j < col; ++j) {
		right.insert(j);
		for (i1 = 0; i1 < col; ++i1) {
		if (i1 == j || dep[j][1 << i1])
		continue;
		for (i2 = i1 + 1; i2 < col; ++i2) {
		if (i2 == j || dep[j][1 << i2] || dep[j][(1 << i1) + (1 << i2)])
		continue;
		for (i3 = i2 + 1; i3 < col; ++i3) {
		if (i3 == j || dep[j][1 << i3])
		continue;

		if (dep[j][(1 << i1) + (1 << i3)] || dep[j][(1 << i2) + (1 << i3)])
		continue;

		left.insert(i1);
		left.insert(i2);
		left.insert(i3);

		if (searchSingle(left, right)) {
		fout << i1 + 1 << " " << i2 + 1 << " " << i3 + 1 << " -> " << j + 1 << endl;
		dep[j][(1 << i1) + (1 << i2) + (1 << i3)] = true;
		++cnt;
		}
		left.clear();
		}
		}
		//left.erase(i2);
		}
		right.erase(j);
		}

		cout << (clock() - start) / CLOCKS_PER_SEC << endl;
		left.clear();
		right.clear();

		

		for (j = 0; j < col; ++j) {
			right.insert(j);
			for (i1 = 0; i1 < col; ++i1) {
				if (i1 == j || dep[j][1 << i1])
					continue;
				for (i2 = i1 + 1; i2 < col; ++i2) {
					if (i2 == j || dep[j][1 << i2] || dep[j][(1 << i1) + (1 << i2)])
						continue;
					for (i3 = i2 + 1; i3 < col; ++i3) {
						if (i3 == j || dep[j][1 << i3])
							continue;

						if (dep[j][(1 << i1) + (1 << i3)] || dep[j][(1 << i2) + (1 << i3)] 
							|| dep[j][(1 << i1) + (1 << i2) + (1 << i3)])
							continue;

						for (i4 = i3 + 1; i4 < col; ++i4) {
							if (i4 == j)
								continue;
							if (dep[j][(1 << i1) + (1 << i4)]
								|| dep[j][(1 << i2) + (1 << i4)]
								|| dep[j][(1 << i3) + (1 << i4)]
								|| dep[j][(1 << i1) + (1 << i3) + (1 << i4)]
								|| dep[j][(1 << i2) + (1 << i3) + (1 << i4)]
								|| dep[j][(1 << i1) + (1 << i2) + (1 << i4)])
								continue;
							left.insert(i1);
							left.insert(i2);
							left.insert(i3);
							left.insert(i4);

							if (searchSingle(left, right)) {
								fout << i1 + 1 << " " << i2 + 1 << " " << i3 + 1 << " " << i4 + 1 << " -> " << j + 1 << endl;
								// dep[j][left.attribute_set] = true;
								++cnt;
							}
							left.clear();
						}

						
					}
				}
				//left.erase(i2);
			}
			right.erase(j);
		}

		cout << (clock() - start) / CLOCKS_PER_SEC << endl;
		cout << cnt;
		
	}*/



	void sort(vector<DisjointSet*>& cand) {
		DisjointSet* ptemp;
		for (int i = 0; i < cand.size(); ++i) {
			for (int j = i + 1; j < cand.size(); ++j) {
				if (cand[i]->sizeEC < cand[j]->sizeEC) {
					ptemp = cand[i];
					cand[i] = cand[j];
					cand[j] = ptemp;
				}
			}
		}
	}

	void test() {
		/*vector<DisjointSet*> v;
		for (int i = 0; i < col; ++i)
			v.push_back(pds + i);
		sort(v);
		for (int i = 0; i < col; ++i)
			cout << i << ": " << v[i]->sizeEC << endl;*/
		/*cout << searchSingle(AttributeSet(1), AttributeSet(2)) << endl;
		cout << searchSingle(AttributeSet(1), AttributeSet(4)) << endl;
		cout << searchSingle(AttributeSet(1), AttributeSet(8)) << endl;
		cout << searchSingle(AttributeSet(34), AttributeSet(4)) << endl;
		AttributeSet l;
		l.insert(1);
		l.insert(7);
		cout << searchSingle(AttributeSet(4), AttributeSet(1)) << endl;
		cout << searchSingle(AttributeSet(130), AttributeSet(1)) << endl;*/
		//dfs();
	}
};