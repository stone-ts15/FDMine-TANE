#pragma once
#include "Util.h"
#include "DisjointSet.h"

typedef unordered_map<string, int> tablemap;

int finishCount;
mutex mtx;

void detect(int target) {
	//lock_guard<recursive_mutex> mtx_locker(mtx);
	while (true) {
		if (mtx.try_lock()) {
			if (finishCount >= target) {
				mtx.unlock();
				return;
			}
			mtx.unlock();
		}
		
	}
}

void ffillECSet(vector<string>& column, tablemap& colmap, tablemap::iterator& itFind, ECSet& ecs) {
	int row = 0;
	int hashedRow;
	vector<int>* newset;

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
				ecs.equivalentClassSet->push_back(*newset);
				itFind->second = ecs.sizeNDEC;
				++ecs.sizeNDEC;
			}
			else {
				ecs.equivalentClassSet->at(hashedRow).push_back(row);
			}
		}
		++row;
	}
	ecs.sizeEC = colmap.size();

	lock_guard<mutex> mtx_locker(mtx);
	//mtx.lock();
	//cout << finishCount;
	++finishCount;
	//mtx.unlock();
	//mtx_locker.~lock_guard();
}

class Database {
public:
	static const unsigned rowlen = 210;
	tablemap* colmaps;
	tablemap::iterator* itFinds;
	ECSet *initialCols;
	int col;
	int length;
	int hashedRow;
	vector<int> *newset;

	// for parallel
	vector<vector<string>> table;
	vector<string>::iterator *itArrs;

public:
	Database() {}
	Database(unsigned vcol) : col(vcol), length(0), colmaps(NULL), itFinds(NULL) {
			
	}

public:
	void prepareMap() {
		colmaps = new tablemap[col];
		itFinds = new tablemap::iterator[col];
		initialCols = new ECSet[col];
	}
	void getTableSeq(istream& is) {
		prepareMap();

		char incstr[rowlen];
		length = 0;
		while (!is.eof()) {
			is.getline(incstr, rowlen);
			if (*incstr) {
				parseSeq(incstr);
				++length;
			}
		}
		for (int i = 0; i < col; ++i) {
			initialCols[i].sizeEC = colmaps[i].size();
		}
		util::collen = length;
	}

	void parseSeq(const char* cstr) {
		// to append one new row
		string s;
		const char *pre = cstr, *cur = cstr;
		int index = 0;
		
		while (*cur) {
			if (*cur == ',' && *(cur + 1) != ' ') {
				putcol(colmaps[index], itFinds[index], initialCols[index], string(pre, cur));
				index = (index + 1) % col;
				pre = cur + 1;
			}
			++cur;
		}
		putcol(colmaps[index], itFinds[index], initialCols[index], string(pre, cur));
	}

	void putcol(tablemap& colmap, tablemap::iterator& itFind, ECSet& ecs, const string& str) {
		itFind = colmap.find(str);
		if (itFind == colmap.end()) {
			colmap[str] = hashRow(length);
		}
		else {
			hashedRow = itFind->second;
			if (hashedRow < 0) {
				newset = new vector<int>();
				newset->push_back(inverseHashRow(hashedRow));
				newset->push_back(length);
				ecs.equivalentClassSet->push_back(*newset);
				itFind->second = ecs.sizeNDEC;
				++ecs.sizeNDEC;
			}
			else {
				ecs.equivalentClassSet->at(hashedRow).push_back(length);
			}
		}
	}

	void preparePrl() {
		table = vector<vector<string>>(col, vector<string>(100000));
		itArrs = new vector<string>::iterator[col];
		int index = 0;
		for (auto &column : table) {
			itArrs[index] = column.begin();
			++index;
		}

		prepareMap();
	}

	void parsePrl(const char* cstr) {
		// to append one new row
		string s;
		const char *pre = cstr, *cur = cstr;
		int index = 0;

		while (*cur) {
			if (*cur == ',' && *(cur + 1) != ' ') {
				*(itArrs[index]) = string(pre, cur);
				++(itArrs[index]);
				index = (index + 1) % col;
				pre = cur + 1;
			}
			++cur;
		}
		*(itArrs[index]) = string(pre, cur);
		++(itArrs[index]);
	}

	void getTablePrl (istream& is) {
		preparePrl();

		// load table from file
		char incstr[rowlen];
		length = 0;
		while (!is.eof()) {
			is.getline(incstr, rowlen);
			if (*incstr) {
				parsePrl(incstr);
				++length;
			}
		}
		util::collen = length;
		for (auto &column : table) {
			column.resize(length);
		}

		finishCount = 0;

		// first layer
		for (int i = 0; i < col; ++i) {
			thread t(ffillECSet, ref(table[i]), ref(colmaps[i]), ref(itFinds[i]), ref(initialCols[i]));
			t.detach();
			//fillECSet(table[i], colmaps[i], itFinds[i], initialCols[i]);
		}

		thread(detect, col).join();
	}

	void fillECSet(vector<string>& column, tablemap& colmap, tablemap::iterator& itFind, ECSet& ecs) {
		int row = 0;

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
					ecs.equivalentClassSet->push_back(*newset);
					itFind->second = ecs.sizeNDEC;
					++ecs.sizeNDEC;
				}
				else {
					ecs.equivalentClassSet->at(hashedRow).push_back(row);
				}
			}
			++row;
		}
		ecs.sizeEC = colmap.size();
	}
};

