#pragma once
#include "Util.h"
#include "DisjointSet.h"

typedef unordered_map<string, int> tablemap;

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
	void getTable(istream& is) {
		prepareMap();

		char incstr[rowlen];
		length = 0;
		while (!is.eof()) {
			is.getline(incstr, rowlen);
			if (*incstr) {
				parse(incstr);
				++length;
			}
		}
		for (int i = 0; i < col; ++i) {
			initialCols[i].sizeEC = colmaps[i].size();
		}
		util::collen = length;
	}

	void parse(const char* cstr) {
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

	void put(int index, const string& str) {
		putcol(colmaps[index], itFinds[index], initialCols[index], str);
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
};

