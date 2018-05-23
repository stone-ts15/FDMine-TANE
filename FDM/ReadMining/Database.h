#pragma once
#include "Util.h"

class Database {
public:
	static const unsigned rowlen = 210;
	vector<array<string, util::collen>> table;
	array<string, util::collen>::iterator *itArrs;
	int col;

public:
	Database() {}
	Database(unsigned vcol) : col(vcol), table(vcol) {
		itArrs = new array<string, util::collen>::iterator[col];
		int i = 0;
		for (auto &column : table) {
			itArrs[i] = column.begin();
			++i;
		}
	}

public:
	void getTable(istream& is) {
		char incstr[rowlen];
		while (!is.eof()) {
			is.getline(incstr, rowlen);
			if (*incstr) {
				parse(incstr);
			}
				
		}
	}

	void parse(const char* cstr) {
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
};