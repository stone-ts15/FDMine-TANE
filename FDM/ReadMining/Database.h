#pragma once

#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <map>
#include <thread>
using namespace std;

void calcSing(list<string>& vit, map<string, list<int>>& vitp);

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

	void calcEquivalenceClass() {
		vector<list<string>>::iterator itt = table.begin();
		vector<map<string, list<int>>>::iterator itp = partition.begin();
		// map<string, list<int>>::iterator itmf;
		// list<string>::iterator itl;
		// int count;
		vector<thread*> pts(table.size());
		vector<thread*>::iterator ith = pts.begin();
		for (; itt != table.end(); ++itt, ++itp, ++ith) {
			// thread t(calcSing, itt);
			*ith = new thread(calcSing, ref(*itt), ref(*itp));
			(*ith)->detach();
			// (*ith)->join();
			/*for (count = 0, itl = itt->begin(); itl != itt->end(); ++itl, ++count) {
				itmf = itp->find(*itl);
				if (itmf == itp->end()) {
					itp->insert(pair<string, list<int>>(*itl, list<int>(1, count)));
				}
				else {
					itmf->second.push_back(count);
				}
			}*/
		}
	}

	
};

void calcSing(list<string>& vit, map<string, list<int>>& vitp) {
	// vector<map<string, list<int>>>::iterator itp = partition.begin();
	map<string, list<int>>::iterator itmf;
	list<string>::iterator itl;
	int count;
	for (count = 0, itl = vit.begin(); itl != vit.end(); ++itl, ++count) {
		itmf = vitp.find(*itl);
		if (itmf == vitp.end()) {
			vitp.insert(pair<string, list<int>>(*itl, list<int>(1, count)));
		}
		else {
			itmf->second.push_back(count);
		}
	}
}