#pragma once

#include "Util.h"
#include "DisjointSet.h"
#include "TANE-tree.h"
#include "AttributeSet.h"

class Solver {
public:
	int col;
	Database* pdb;
	DisjointSet* pds;
	unordered_map<long long, int> gmap;
	//array<int, 4> initHashArg;
	unordered_set<int> superKey;
	int sc;
	unordered_map<string, int>* pColmaps;
	vector<int> *pcRoots;
public:
	Solver(int vcol) : col(vcol), pdb(NULL){
		pds = new DisjointSet[vcol];
	}

	Solver(Database* vpdb) : col(vpdb->col), pdb(vpdb), pds(NULL), pcRoots(NULL) {
		pds = new DisjointSet[col];
		pColmaps = new unordered_map<string, int>[col];
		//loadInitialColumn(pdb);
		sc = 0;
	}

	void loadInitialColumn(Database* vpdb) {
		pdb = vpdb;
		for (int i = 0; i < col; ++i) {
			pds[i].fromTable(*pdb, i);
		}
	}

	bool searchSingle(const AttributeSet& left, const AttributeSet& right, unordered_set<int>& sk) {
		
		
		for (auto &s : sk) {
			if (!((s & left.attribute_set) ^ s) )
				return true;
		}
		
		
		if (left.attribute_set == 0)
			return false;

		vector<int> leftAttrs = left.toVector();
		int size = leftAttrs.size();
		vector<DisjointSet*> leftds;
		int i, j, k;
		int rv;

		for (i = 0; i < size; ++i) {
			if (pds[leftAttrs[i]].sizeEC == 0)
				return true;
			leftds.push_back(pds + leftAttrs[i]);
		}

		// if containing col[1]
		if (!((2 & left.attribute_set) ^ 2))
			sort(leftds);

		bool issuper = true;

		vector<array<int, collen>::iterator> visit;
		for (i = 0; i < size; ++i)
			visit.push_back(leftds[i]->parr->begin());
		

		array<int, collen>::iterator rGroup = pds[right.toVector()[0]].parr->begin();

		unordered_map<long long, int>::iterator itFind;
		long long hashValue;
		//array<int, 4> hashArg(initHashArg);

		++sc;
		long long *pargs = new long long[4]{ 0 };
		
		bool pass;
		for (i = 0; i < collen; ++i, ++rGroup) {
			pass = false;
			for (j = 0; j < size; ++j) {
				pass = pass || (*(visit[j]) == -1);
				//if (*(visit[j]) == -1)
				//	pass = true;
				// hashArg[4 - size + j] = *(visit[j]);
				pargs[4 - size + j] = *(visit[j]);
				++visit[j];
			}
			if (pass)
				continue;
			// hashValue = hashn(hashArg);
			hashValue = hashn(pargs);
			itFind = gmap.find(hashValue);
			rv = *rGroup;
			if (itFind == gmap.end()) {
				//gmap[hashValue] = *rGroup;
				gmap.insert(pair<long long, int>(hashValue, rv));
			}
			else {
				issuper = false;
				if (rv == -1 || itFind->second != rv) {
					gmap.clear();
					return false;
				}
			}
		}
		gmap.clear();
		if (issuper)
			sk.insert(left.attribute_set);
			
		return true;
	}

	void sort(vector<DisjointSet*>& cand) {
		DisjointSet* ptemp;
		int size = cand.size();

		for (int i = 0; i < size; ++i) {
			for (int j = i + 1; j < size; ++j) {
				if (cand[i]->sizeEC < cand[j]->sizeEC) {
					ptemp = cand[i];
					cand[i] = cand[j];
					cand[j] = ptemp;
				}
			}
		}
	}

	void calcualte_initial_RHS_plus(TANE_Layer &pre, TANE_Layer &cur) {
		vector<int> asVector;
		AttributeSet result;
		AttributeSet temp;
		AttributeSet t;
		map<AttributeSet, TANE_Node>::iterator it;
		int len;
		for (auto &layer_record : cur.layer) {

			TANE_Node &node = layer_record.second;

			len = node.as.size();

			node.RHS_plus.clear();
			asVector = node.as.toVector();
			
			for (int i = 0; i < len; i++) {
				temp = node.as;
				temp.erase(asVector[i]);

				it = pre.layer.find(temp);

				if (it == pre.layer.end()) {
					result.clear();
					break;
				}

				t = it->second.RHS_plus;

				if (i == 0) {
					result = t;
				}
				else {
					// result = result intersect RHS( X - E )
					//AttributeSet a = ;
					result = t.intersect(result);

					// if result = \phi break;
					if (result.size() == 0)
						break;
				}
			}

			node.RHS_plus = result;
		}
	}

	void compute_dependecies(int col, TANE_Layer &pre, TANE_Layer &cur, ofstream& of) {
		calcualte_initial_RHS_plus(pre, cur);

		AttributeSet R((1 << col) - 1);
		AttributeSet choice;
		int len;
		vector<int> choiceVector;
		map<AttributeSet, TANE_Node>::iterator it;
		AttributeSet X_E;
		AttributeSet Eset;

		for (auto &layer_record : cur.layer) {

			TANE_Node &node = layer_record.second;

			choice = node.RHS_plus.intersect(node.as);

			len = choice.size();

			
			
			if (len > 0) {
				if (node.db != nullptr)
					//node.pt.getPartitionFromTable(*node.db, node.as);
					node.pt.fromTable(*(node.db), node.as.toVector()[0], pColmaps[node.as.toVector()[0]]);
				else {
					//node.pt.getPartitionFromProduct(node.p1->pt, node.p2->pt);
					pcRoots = new vector<int>(collen, -1);
					node.pt.fromProduct(node.p1->pt, node.p2->pt, *pcRoots);
					//cout << node.as.toVector()[0]
					delete pcRoots;
				}
					
			}
			

			choiceVector = choice.toVector();
			

			for (int i = 0; i < len; i++) {
				int E = choiceVector[i];
				//Eset.insert(E);
				Eset = 1 << E;
				//E belongs to X intersect RHS+(X)

				X_E = node.as.substract(Eset);

				it = pre.layer.find(X_E);

				if (it->second.pt.cardinality() == node.pt.cardinality()) {
				
				//if (searchSingle(X_E, Eset, superKey)) {
					//new FD found 
					//output
					for (auto &t : X_E.toVector()) {
						// cout << t + 1 << " ";
						of << t + 1 << " ";
					}
					// cout << "-> " << E + 1 << endl;
					of << "-> " << E + 1 << endl;


					//remove E from RHS+ i is E 's index
					node.RHS_plus.erase(E);

					//remove F belongs to R \ X from RHS+ 

					//a2 = R - X
					//AttributeSet a2 = R.substract(node.as);

					//a3 = RHS+ - a2 = RHS+ - (R - X)
					//AttributeSet a3 = node.RHS_plus.substract(a2);

					//node.RHS_plus = a3;
					node.RHS_plus = node.RHS_plus.substract(R.substract(node.as));
				}
			}
		}
	}

	void prune(TANE_Layer &pre, TANE_Layer & cur) {
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

	void TANE_search_FD() {

		ofstream of("result.txt");

		TANE_Layer *pre;
		TANE_Layer *cur;

		pre = new TANE_Layer(true, col);
		cur = new TANE_Layer(col, *pdb);
		int layerCount = 0;

		while (cur->size() != 0) {
			cout << layerCount << endl;
			++layerCount;
			compute_dependecies(col, *pre, *cur, of);

			prune(*pre, *cur);

			delete pre;
			pre = cur;
			cur = new TANE_Layer(*pre, col);

		}

		delete pre;
		delete cur;
	}

	void solve() {
		TANE_search_FD();
		/*
		ECSet pts[15];
		for (int i = 0; i < 15; ++i) {
			pts[i].fromTable(*pdb, i, pColmaps[i]);
			//cout << i << " " << pts[i].sizeEC << " " << pts[i].sizeNDEC << endl;
		}

		ECSet p12;
		p12.fromProduct(pts[0], pts[1]);
		cout << sc << endl;
		*/
	}
};