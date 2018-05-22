#pragma once
#include "Util.h"

class Node {

};

class StringNode : public Node {

};

class IntNode : public Node {

};

class Database {
public:
	static const unsigned rowlen = 210;
	//static const unsigned collen = 99918;
	vector<array<string, util::collen>> table;
	vector<map<string, list<int>>> partition;
	array<string, util::collen>::iterator *itArrs;
	int col;

public:
	Database() {}
	Database(unsigned vcol) : col(vcol), table(vcol), partition(vcol) {
		itArrs = new array<string, util::collen>::iterator[col];
		int i = 0;
		for (auto &column : table) {
			itArrs[i] = column.begin();
			++i;
		}
	}

public:
	void getTable(istream& is) {
		set<string> lines;
		string str;
		char incstr[rowlen];
		int index = 0;
		while (!is.eof()) {
			is.getline(incstr, rowlen);
			if (*incstr) {
				parse(incstr);
			}
				
		}
	}

	void parse(const char* cstr) {
		// to append one new row
		// vector<array<string, collen>>::iterator it = table.begin();
		string s;
		const char *pre = cstr, *cur = cstr;
		int index = 0;
		
		while (*cur) {
			if (*cur == ',' && *(cur + 1) != ' ') {
				*(itArrs[index]) = string(pre, cur);
				++(itArrs[index]);
				++index;
				index %= col;
				//it->push_back(string(pre, cur));
				//++it;
				pre = cur + 1;
			}
			++cur;
		}
		//it->push_back(string(pre, cur));
		//it->assign(string(pre, cur));
		*(itArrs[index]) = string(pre, cur);
		++(itArrs[index]);
	}
};