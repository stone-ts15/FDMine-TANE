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
	vector<list<string>> table;
	vector<map<string, list<int>>> partition;
	static const unsigned rowlen = 210;

public:
	Database() {}
	Database(unsigned col) : table(col), partition(col) {}

public:
	void getTable(istream& is) {
		set<string> lines;
		string str;
		char incstr[rowlen];
		while (!is.eof()) {
			is.getline(incstr, rowlen);
			if (*incstr)
				parse(incstr);
		}
	}

	void parse(const char* cstr) {
		// to append one new row
		vector<list<string>>::iterator it = table.begin();
		string s;
		const char *pre = cstr, *cur = cstr;
		while (*cur) {
			if (*cur == ',' && *(cur + 1) != ' ') {
				it->push_back(string(pre, cur));
				++it;
				pre = cur + 1;
			}
			++cur;
		}
		it->push_back(string(pre, cur));
	}
};