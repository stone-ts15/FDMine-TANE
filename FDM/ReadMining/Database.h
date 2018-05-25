#pragma once
#include "Util.h"

class Database {
public:
	static const unsigned rowlen = 210;
	vector<vector<string>> table;
	vector<string>::iterator *itArrs;
	int col;
	int length;

public:
	Database() {}
	Database(unsigned vcol) : col(vcol), table(vcol), length(0) {
		itArrs = new vector<string>::iterator[col];
		int i = 0;
		for (auto &column : table) {
			column = vector<string>(100000);
			itArrs[i] = column.begin();
			++i;
		}
	}

public:
	void getTable(istream& is) {
		char incstr[rowlen];
		length = 0;
		while (!is.eof()) {
			is.getline(incstr, rowlen);
			if (*incstr) {
				parse(incstr);
				++length;
			}
		}
		util::collen = length;
		for (auto &column : table) {
			column.resize(length);
		}
	}

	void parse(const char* cstr) {
		// to append one new row
		string s;
		const char *pre = cstr, *cur = cstr;
		int index = 0;
		
		while (*cur) {
			if (*cur == ',' && *(cur + 1) != ' ') {
				//table[index]->push_back(string(pre, cur));
				*(itArrs[index]) = string(pre, cur);
				++(itArrs[index]);
				index = (index + 1) % col;
				pre = cur + 1;
			}
			++cur;
		}
		*(itArrs[index]) = string(pre, cur);
		++(itArrs[index]);
		//table[index]->push_back(string(pre, cur));
	}
};