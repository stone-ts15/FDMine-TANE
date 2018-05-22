#pragma once
#pragma once
#include <vector>
#include <map>
#include <list>
#include <string>
#include <fstream>

#include "Database.h"
#include "DisjointSet.h"

using namespace std;

// maximum of 32 attributes
class AttributeSet {
public:
	unsigned int attribute_set = 0;

public:
	AttributeSet() {}
	AttributeSet(unsigned int k) {
		attribute_set = k;
	}
	AttributeSet(const AttributeSet& src) : attribute_set(src.attribute_set) {}

	int operator [] (int index) {
		unsigned int k = 1 << index;
		return (k & attribute_set) >> index;
	}

	void operator = (AttributeSet &k) {
		attribute_set = k.attribute_set;
	}

	bool operator == (const AttributeSet &k) const {
		return attribute_set == k.attribute_set;
	}

	bool operator < (const AttributeSet &k) const {
		// has low index attribute's set will be put in the front
		unsigned int t = attribute_set;
		unsigned int tk = k.attribute_set;
		for (int i = 0; i < 32; i++) {
			unsigned int t_1 = t & 1;
			unsigned int tk_1 = tk & 1;
			if (t_1 > tk_1) {
				return true;
			}
			else if (t_1 < tk_1) {
				return false;
			}
			else {
				t = t >> 1;
				tk = tk >> 1;
			}
		}

		return false;
	}

	int size() {
		int count = 0;
		unsigned t = attribute_set;
		for (int i = 0; i < 32; i++) {
			if (t & 1)
				count++;

			t = t >> 1;
		}
		return count;
	}

	void assign(AttributeSet &k) {
		attribute_set = k.attribute_set;
	}

	void insert(int k) {
		attribute_set = attribute_set | (1 << k);
	}

	void erase(int k) {
		attribute_set = attribute_set & (~(1 << k));
	}

	vector<int> toVector() const{
		vector<int> vec;
		unsigned t = attribute_set;
		for (int i = 0; i < 32; i++) {
			if (t & 1)
				vec.push_back(i);
			t = t >> 1;
		}

		return vec;
	}

	void clear() {
		attribute_set = 0;
	}

	bool common_prefix(AttributeSet &k) {
		int sz = size();

		int count = 0;
		unsigned t = attribute_set;
		unsigned tk = k.attribute_set;

		for (int i = 0; i < 32; i++) {
			if ((t & 1) && (tk & 1))
				count++;
			else if ((t & 1) || (tk & 1))
				break;

			t = t >> 1;
			tk = tk >> 1;
		}

		if (count == sz - 1)
			return true;
		else 
			return false;

	}

public:
	AttributeSet intersect(AttributeSet &b) {
		return AttributeSet(attribute_set & b.attribute_set);
	}

	AttributeSet combine(AttributeSet &b) {
		return AttributeSet(attribute_set | b.attribute_set);
	}

	AttributeSet substract(AttributeSet &b) {
		unsigned int temp = ~b.attribute_set;
		return AttributeSet(attribute_set & temp);
	}
};

typedef AttributeSet IndexSet;

typedef vector<string> StringKey;

class Partition {
public:
	map<StringKey, IndexSet> partition;
	AttributeSet * as;
public:
	Partition() { as = nullptr; }
	Partition(AttributeSet &k, Database &db) {
		this->doPartition(k, db);
	}
public:

	int cardinality() {
		return partition.size();
	}

	void doPartition(AttributeSet &k, Database &db) {
		as = &k;

		int records_count = db.table[0].size();

		list<string>::iterator *ps = new list<string>::iterator[db.table.size()];

		vector<int> kVector = k.toVector();

		for (auto attr : kVector) {
			ps[attr] = db.table[attr].begin();
		}

		int first = *kVector.begin();
		int index = 0;


		while (ps[first] != db.table[first].end()) {
			StringKey temp;

			for (auto attr : kVector) {
				temp.push_back(*ps[attr]);
				ps[attr] ++;
			}

			map<StringKey, IndexSet>::iterator it = partition.find(temp);

			if (it != partition.end()) {
				it->second.insert(index);
			}
			else {
				partition.insert(pair<StringKey, IndexSet>(temp, IndexSet(index)));
			}

			index++;
			temp.clear();
		}

		delete[] ps;
	}

	void doPartition(Partition &p1, Partition &p2) {
		
	}
};

// using disjoint set
class DSPartition {
public:
	DisjointSet partition;
	AttributeSet as;
public:
	DSPartition() : as(NULL) {}
	DSPartition(const Database& db, const AttributeSet& attr) : as(attr) {
		getPartitionFromTable(db, as);
	}
public:

	int cardinality() {
		return partition.sizeEC;
	}


	// read lines of <db> specified by <attr>, into this <ds>
	void getPartitionFromTable(const Database& db, const AttributeSet& attr) {
		// 优化：toVector返回int*，减少一次赋值
		// 优化：一次取出table的列or取多次
		// 优化：每一列开新的map
		// 优化：直接向map的end插入,*itFind = pair<>...
		map<string, int> equivalenceClass;
		vector<int> attrVec = attr.toVector();
		map<string, int>::iterator itFind;
		int partitionCount;
		// only when 1 attr in AttributeSet
		for (auto &index : attrVec) {
			const list<string>& column = db.table[index];
			partitionCount = 0;
			equivalenceClass.clear();
			for (auto& str : column) {
				itFind = equivalenceClass.find(str);
				if (itFind == equivalenceClass.end()) {
					equivalenceClass.insert(pair<string, int>(str, partitionCount));
					partition.append(partitionCount);
					++partitionCount;
					partition.sizeEC++;
				}
				else {
					partition.append(itFind->second);
				}
			}
		}
	}

	void getPartitionFromProduct(DSPartition& p1, DSPartition& p2) {
		partition.fromProduct(p1.partition, p2.partition);
	}
};

class TANE_Node {
public:
	AttributeSet as;
	DSPartition pt;
	AttributeSet RHS_plus;

	Database * db = nullptr;
	TANE_Node * p1 = nullptr;
	TANE_Node * p2 = nullptr;
public:
	TANE_Node() {}
	TANE_Node(bool blank, int total_attribute_count) {
		if (blank == true) {
			for (int i = 0; i < total_attribute_count; i++) {
				RHS_plus.insert(i);
			}
		}
	}
	TANE_Node(AttributeSet &ast, Database &db) {
		as = ast;
		//pt.doPartition(as, db);
		this->db = &db;
	}
	TANE_Node(AttributeSet &ast, TANE_Node &p1, TANE_Node &p2) {
		as = ast;
		//pt.doPartition(p1.pt, p2.pt);
		this->p1 = &p1;
		this->p2 = &p2;
	}
};

class TANE_Layer {
public:
	map<AttributeSet, TANE_Node> layer;

public:
	TANE_Layer() {}
	TANE_Layer(bool blank, int total_attribute_count) { 
		layer.insert(pair<AttributeSet, TANE_Node>(AttributeSet(0), TANE_Node(blank, total_attribute_count)));
	}
	TANE_Layer(int total_attribute_count, Database & db) {
		generate_first_level(total_attribute_count, db);
	}
	TANE_Layer(TANE_Layer &pre, int total_attribute_count) {
		generate_next_level(pre, total_attribute_count);
	}


public:
	void generate_first_level(int total_attribute_count, Database &db) {
		for (int i = 0; i < total_attribute_count; i++) {
			AttributeSet temp;

			temp.insert(i);
			layer.insert(pair<AttributeSet, TANE_Node>(temp, TANE_Node(temp, db)));

			temp.clear();
		}
	}

	void generate_next_level(TANE_Layer &pre, int total_attribute_count) {
		this->clear();

		map<AttributeSet, TANE_Node>::iterator p = pre.layer.begin();
		map<AttributeSet, TANE_Node>::iterator q = p;

		while (p != pre.layer.end()) {
			q = p;
			q++;
			while (q != pre.layer.end()) {
				if (!(*p).second.as.common_prefix((*q).second.as))
					break;

				AttributeSet temp = (*p).second.as.combine((*q).second.as);

				vector<int> tempVector = temp.toVector();
				bool flag = true;
				//forall A belongs to X , X \ A belongs to Ll(pre) then add X
				for (auto &at : tempVector) {
					AttributeSet t_temp = temp;
					
					// X - A
					t_temp.erase(at);
					map<AttributeSet, TANE_Node> ::iterator it = pre.layer.find(t_temp);
					if (it == pre.layer.end()){
						flag = false;
						break;
					}					
				}
				if (flag)
					layer.insert(pair<AttributeSet, TANE_Node>(temp,TANE_Node(temp,(*p).second,(*q).second)));
				q++;
			}

			p++;
		}
	}

	int size() {
		return layer.size();
	}

	void clear() {
		layer.clear();
	}
};

void calcualte_initial_RHS_plus(int total_attribute_count, TANE_Layer &pre, TANE_Layer &cur) {
	for (auto &layer_record : cur.layer) {

		TANE_Node &node = layer_record.second;

		int len = node.as.size();

		node.RHS_plus.clear();

		vector<int> asVector = node.as.toVector();
		AttributeSet result;
		for (int i = 0; i < len; i++) {
			AttributeSet temp = node.as;
			temp.erase(asVector[i]);

			map<AttributeSet, TANE_Node>::iterator it = pre.layer.find(temp);

			if (it == pre.layer.end()) {
				result.clear();
				break;
			}

			AttributeSet t = it->second.RHS_plus;

			if (i == 0) {
				result = t;
			}
			else {
				// result = result intersect RHS( X - E )
				AttributeSet a = t.intersect(result);
				result = a;

				// if result = \phi break;
				if (result.size() == 0)
					break;
			}
		}

		node.RHS_plus = result;
	}
}

void compute_dependecies(int total_attribute_count, TANE_Layer &pre, TANE_Layer &cur,ofstream * of) {
	calcualte_initial_RHS_plus(total_attribute_count, pre, cur);

	AttributeSet R( (1 << total_attribute_count) - 1 );
	
	for (auto &layer_record : cur.layer) {

		TANE_Node &node = layer_record.second;

		AttributeSet choice = node.RHS_plus.intersect(node.as);

		int len = choice.size();

		if (len > 0) {
			if (node.db != nullptr)
				node.pt.getPartitionFromTable(*node.db, node.as);
			else
				node.pt.getPartitionFromProduct(node.p1->pt, node.p2->pt);
		}

		vector<int> choiceVector = choice.toVector();
		for (int i = 0; i < len; i++) {
			int E = choiceVector[i];
			AttributeSet Eset(1 << E);
			//E belongs to X intersect RHS+(X)

			AttributeSet X_E = node.as.substract(Eset);

			map<AttributeSet, TANE_Node>::iterator it = pre.layer.find(X_E);

			if (it->second.pt.cardinality() == node.pt.cardinality()) {
				//new FD found 
				//output
				for (auto &t : X_E.toVector()) {
					cout << t + 1 << " ";
					(*of) << t + 1 << " ";
				}
				cout << "-> " << E + 1 << endl;
				(*of) << "-> " << E + 1 << endl;


				//remove E from RHS+ i is E 's index
				node.RHS_plus.erase(E);

				//remove F belongs to R \ X from RHS+ 

				//a2 = R - X
				AttributeSet a2 = R.substract(node.as);

				//a3 = RHS+ - a2 = RHS+ - (R - X)
				AttributeSet a3 = node.RHS_plus.substract(a2);

				node.RHS_plus = a3;
			}
		}
	}
}

void prune(int total_attribute_count, TANE_Layer &pre, TANE_Layer & cur) {
	map<AttributeSet, TANE_Node>::iterator p_layer = cur.layer.begin();
	vector<map<AttributeSet, TANE_Node>::iterator> remove_set;

	while (p_layer != cur.layer.end()) {
		if (p_layer->second.RHS_plus.size() == 0) {
			//cur.layer.erase(p_layer);
			//p_layer++;
			//continue;

			remove_set.push_back(p_layer);
		}

		//X is a super key?

		p_layer++;
	}

	for (auto &layer_it : remove_set) {
		cur.layer.erase(layer_it);
	}
}

void TANE_search_FD(int total_attribute_count, Database &db) {

	ofstream *of = new ofstream("result.txt");

	TANE_Layer *pre;
	TANE_Layer *cur;

	pre = new TANE_Layer(true, total_attribute_count);
	cur = new TANE_Layer(total_attribute_count, db);

	while(cur->size()!=0) {
		compute_dependecies(total_attribute_count, *pre, *cur, of);
		prune(total_attribute_count, *pre, *cur);


		delete pre;
		pre = cur;

		cur = new TANE_Layer(*pre, total_attribute_count);
	}
	
	delete pre;
	delete cur;
}