#pragma once
#pragma once

#include "Util.h"
#include "Database.h"
#include "DisjointSet.h"
#include "AttributeSet.h"

using namespace std;

/*class Partition {
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
};*/

// using disjoint set
class DSPartition {
public:
	DisjointSet partition;
	AttributeSet as;
	PartSet psp;
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
	void pfromTable(const Database& db, const AttributeSet& attr) {
		// 优化：toVector返回int*，减少一次赋值
		// 优化：一次取出table的列or取多次
		// 优化：直接向map的end插入,*itFind = pair<>...
		as = attr;
		singleRoots.clear();
		partition.parr = new array<int, util::collen>();

		vector<int> attrVec = attr.toVector();
		unordered_map<string, int>::iterator itFind;
		array<int, util::collen>::iterator itAsgn = partition.parr->begin();
		int partitionCount = 0;

		// only when 1 attr in AttributeSet
		for (auto &index : attrVec) {
			const array<string, util::collen>& column = db.table[index];
			partitionCount = 0;
			equivalenceClass.clear();
			for (auto& str : column) {
				itFind = equivalenceClass.find(str);
				if (itFind == equivalenceClass.end()) {
					equivalenceClass.insert(pair<string, int>(str, partitionCount));
					singleRoots.insert(partitionCount);
					(*itAsgn) = partitionCount;
					++partitionCount;
				}
				else {
					(*itAsgn) = itFind->second;
					singleRoots.erase(itFind->second);
				}
				++itAsgn;
			}
		}

		for (auto &r : *partition.parr) {
			if (singleRoots.find(r) != singleRoots.end())
				r = -1;
		}

		partition.sizeEC = partitionCount;
		equivalenceClass.clear();
		singleRoots.clear();
		cout << attr.toVector()[0] << ": " << partition.sizeEC << endl;
	}

	void getPartitionFromTable(const Database& db, const AttributeSet& attr) {
		as = attr;
		//psp.fromDatabase(db, attr.toVector()[0]);
		//partition.fromTable(db, attr.toVector()[0]);
		//pfromTable(db, attr);
	}

	void getPartitionFromProduct(DSPartition& p1, DSPartition& p2) {
		//partition.fromProduct(p1.partition, p2.partition);
		//as = p1.as.combine(p2.as);
	}
};

class TANE_Node {
public:
	AttributeSet as;
	//DSPartition pt;
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
		//pt.doPartition(as, db);
		this->db = &db;
	}
	TANE_Node(AttributeSet &ast, TANE_Node &p1, TANE_Node &p2) {
		as = ast;
		//pt.doPartition(p1.pt, p2.pt);
		this->p1 = &p1;
		this->p2 = &p2;
	}

public:
	bool is_super_key() {
		return pt.sizeNDEC == 0;
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