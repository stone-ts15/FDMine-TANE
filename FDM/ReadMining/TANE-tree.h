#pragma once

#include "Util.h"
#include "Database.h"
#include "DisjointSet.h"
#include "AttributeSet.h"

using namespace std;

class TANE_Node {
public:
	AttributeSet as;
	ECSet pt;

	Database * db = nullptr;
	TANE_Node * p1 = nullptr;
	TANE_Node * p2 = nullptr;
public:
	TANE_Node() {}
	TANE_Node(bool blank, int total_attribute_count) {
		if (blank == true) {

		}
	}
	TANE_Node(AttributeSet &ast, Database &db) {
		as = ast;
		this->db = &db;
	}
	TANE_Node(AttributeSet &ast, TANE_Node &p1, TANE_Node &p2) {
		as = ast;
		this->p1 = &p1;
		this->p2 = &p2;
	}

public:
	bool is_super_key() {
		//return false;
		return pt.sizeNDEC == 0;
	}
};

typedef map<AttributeSet, TANE_Node> ANmap;

class TANE_Layer {
public:
	ANmap layer;

public:
	TANE_Layer() {}
	TANE_Layer(bool blank, int total_attribute_count) { 
		layer[AttributeSet(0)] = TANE_Node(blank, total_attribute_count);
	}
	TANE_Layer(int total_attribute_count, Database & db) {
		generate_first_level(total_attribute_count, db);
	}
	TANE_Layer(TANE_Layer &pre, int total_attribute_count) {
		generate_next_level(pre, total_attribute_count);
	}


public:
	void generate_first_level(int total_attribute_count, Database &db) {
		AttributeSet temp;
		for (int i = 0; i < total_attribute_count; i++) {
			temp.insert(i);
			layer[temp] = TANE_Node(temp, db);
			temp.erase(i);
		}
	}

	void generate_next_level(TANE_Layer &pre, int total_attribute_count) {
		this->clear();

		ANmap::iterator p = pre.layer.begin();
		ANmap::iterator q = p;
		AttributeSet temp;
		AttributeSet t_temp;
		vector<int> tempVector;
		bool flag;

		while (p != pre.layer.end()) {
			q = p;
			++q;
			while (q != pre.layer.end()) {
				if (!p->second.as.common_prefix(q->second.as))
					break;

				temp = p->second.as.combine(q->second.as);

				tempVector = temp.toVector();
				flag = true;
				//forall A belongs to X , X \ A belongs to Ll(pre) then add X
				for (auto &at : tempVector) {
					t_temp = temp;
					
					// X - A
					t_temp.erase(at);
					if (pre.layer.find(t_temp.attribute_set) == pre.layer.end()){
						flag = false;
						break;
					}					
				}
				if (flag)
					// layer.insert(pair<AttributeSet, TANE_Node>(temp,TANE_Node(temp,p->second,q->second)));
					layer[temp] = TANE_Node(temp, p->second, q->second);
				++q;
			}

			++p;
		}
	}

	int size() {
		return layer.size();
	}

	void clear() {
		layer.clear();
	}
};

class FD {
public:
	vector<int> l;
	int r;
public:
	FD() {}
	FD(vector<int> &a, int b) : l(a), r(b) {}
};