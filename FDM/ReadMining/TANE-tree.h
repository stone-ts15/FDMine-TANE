#pragma once
#include <vector>
#include <map>
#include <list>
#include <string>

#include "Database.h"

using namespace std;

// assume the attribute_set's elements is in order
class AttributeSet {
public:
	vector<int> attribute_set;
	Database *db = nullptr;

public:
	AttributeSet() {}
	AttributeSet(int k) {
		this->clear();
		attribute_set.push_back(k);
	}
	AttributeSet(vector<int> &a) {
		this->clear();
		for (auto s : a) {
			attribute_set.push_back(s);
		}
	}

	void operator = (AttributeSet &k) {
		this->clear();
		for (auto &t : k.attribute_set) {
			attribute_set.push_back(t);
		}
	}

	bool operator == (const AttributeSet &k) const {
		int i = 0;
		int la = attribute_set.size();
		int lb = k.attribute_set.size();

		if (la != lb) return false;
		while (1) {
			if (i == la || i == lb)
				break;

			if (attribute_set[i] != k.attribute_set[i]) {
				return false;
			}
		}
		
		return true;
	}

	bool operator < (const AttributeSet &k) const {
		int i = 0;
		int la = attribute_set.size();
		int lb = k.attribute_set.size();
		while (1) {
			if (i == la || i == lb)
				break;

			if (attribute_set[i] == k.attribute_set[i]) {
				i++;
			}
			else if (attribute_set[i] < k.attribute_set[i]) {
				return true;
			}
			else return false;
		}

		if (i == la && la!=lb)
			return true;
		else
			return false;
		
	}

	void push(int v) {
		attribute_set.push_back(v);
	}

	void pop() {
		attribute_set.pop_back();
	}

	void clear() {
		attribute_set.clear();
	}

	void setDb(Database &adb) {
		db = &adb;
	}

	bool next(int count) {
		vector<int>::reverse_iterator it = attribute_set.rbegin();

		int c = count;

		while (it != attribute_set.rend()) {
			if (*it != c)
				break;
			it++;
			c--;
		}

		if (it == attribute_set.rend())
			return false;
		
		int begin = *it + 1;
		int end = *it + 1 + count - c;

		vector<int>::reverse_iterator itp = attribute_set.rbegin();

		while (itp != it) {
			*itp = end;
			end--;
			itp++;
		}
		*it = begin;
		return true;
	}

public:
	void intersect(AttributeSet &b,AttributeSet &result) {
		vector<int>::iterator pa = attribute_set.begin();
		vector<int>::iterator pb = b.attribute_set.begin();

		result.clear();
		while (pa != attribute_set.end() && pb != b.attribute_set.end()) {
			if (*pa == *pb) {
				result.push(*pa);
				pa++;
				pb++;
			}
			else if (*pa < *pb) {
				pa++;
			}
			else {
				pb++;
			}
		}
	}

	void combine(AttributeSet &b,AttributeSet &result) {
		vector<int>::iterator pa = attribute_set.begin();
		vector<int>::iterator pb = b.attribute_set.begin();

		result.clear();
		while (pa != attribute_set.end() || pb != b.attribute_set.end()) {
			if (pa == attribute_set.end()) {
				result.push(*pb);
				pb++;
				continue;
			}
			else if (pb == b.attribute_set.end()) {
				result.push(*pa);
				pa++;
				continue;
			}

			if (*pa == *pb) {
				result.push(*pa);
				pa++;
				pb++;
			}
			else if (*pa < *pb) {
				result.push(*pa);
				pa++;
			}
			else {
				result.push(*pb);
				pb++;
			}
		}
	}

	void substract(AttributeSet &b, AttributeSet & result) {
		vector<int>::iterator pa = attribute_set.begin();
		vector<int>::iterator pb = b.attribute_set.begin();

		result.clear();
		while (pa != attribute_set.end()) {
			if (pb == b.attribute_set.end()) {
				result.push(*pa);
				pa++;
				continue;
			}

			if (*pa == *pb) {
				pa++;
				pb++;
			}
			else if (*pa < *pb) {
				result.push(*pa);
				pa++;
			}
			else {
				pb++;
			}
		}
	}
};

typedef AttributeSet IndexSet;

typedef vector<string> StringKey;

class Partition {
public:
	map<StringKey,IndexSet> partition;
	AttributeSet * as;
public:
	Partition() { as = nullptr; }
	Partition(AttributeSet &k, Database &db) {
		this->doPartition(k, db);
	}

	void doPartition(AttributeSet &k, Database &db) {
		as = &k;

		int records_count = db.table[0].size();

		list<string>::iterator *ps = new list<string>::iterator[db.table.size()];

		for (auto attr : k.attribute_set) {
			ps[attr] = db.table[attr].begin();
		}

		int first = *k.attribute_set.begin();
		int index = 0;


		while (ps[first] != db.table[first].end()) {
			StringKey temp;

			for (auto attr : k.attribute_set) {
				temp.push_back(*ps[attr]);
				ps[attr] ++;
			}

			map<StringKey, IndexSet>::iterator it = partition.find(temp);

			if (it != partition.end()) {
				it->second.push(index);
			}
			else {
				partition.insert(pair<StringKey, IndexSet>(temp, IndexSet(index)));
			}

			index++;
			temp.clear();
		}

		delete[] ps;
	}
};

class TANE_Node{
public:
	AttributeSet as;
	Partition pt;
	Database *db_pointer = nullptr;

	AttributeSet RHS_plus;
public:
	TANE_Node() {}
	TANE_Node(AttributeSet &ast,Database &db) { 
		as = ast;
		as.setDb(db);
		db_pointer = &db;
		pt.doPartition(as, db);
	}
public:
	void calcualte_initial_RHS_plus(TANE_Layer &pre, AttributeSet &attr) {
		AttributeSet temp(attr.attribute_set);
		int len = temp.attribute_set.size();
		

	}
};

class TANE_Layer {
public:
	map<AttributeSet,TANE_Node> layer;

public:
	TANE_Layer() {}
	TANE_Layer(int n, int total_attribute_count, Database &db) {
		generate_new_layer(n, total_attribute_count, db);
	}

public:

	void generate_new_layer(int n, int total_attribute_count, Database &db) {
		AttributeSet temp;
		for (int i = 0; i < n; i++) {
			temp.push(i);
		}

		layer.insert(pair<AttributeSet, TANE_Node>(temp, TANE_Node(temp, db)));
		while (temp.next(total_attribute_count - 1)) {
			layer.insert(pair <AttributeSet, TANE_Node> (temp, TANE_Node(temp, db)));
		}
	}

	void clear() {
		layer.clear();
	}
};