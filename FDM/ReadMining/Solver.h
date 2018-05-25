#pragma once

#include "Util.h"
#include "DisjointSet.h"
#include "TANE-tree.h"
#include "AttributeSet.h"
#include <ctime>

typedef unordered_map<AttributeSet, AttributeSet, AttributeSetHash> asmap;

static bool cmp_lt(FD & a, FD & b) {
	size_t la = a.l.size();
	size_t lb = b.l.size();

	for (size_t i = 0; i < la && i < lb; ++i) {
		if (a.l[i] != b.l[i])
			return a.l[i] < b.l[i];
	}

	return (la < lb) || (la == lb && a.r < b.r);
}

class Solver {
public:
	int col;
	Database* pdb;
	asmap RHS_plus_map;
	vector<int> T;
	vector<int> *S;
	vector<FD> fds;
public:
	Solver(int vcol) : col(vcol), pdb(NULL) {}

	Solver(Database* vpdb) : col(vpdb->col), pdb(vpdb), T(collen, -1) {
		S = new vector<int>[vpdb->length + 1];
	}

	~Solver() { delete[] S; }

	AttributeSet& get_RHS_plus(AttributeSet k) {
		//try to find 
		AttributeSet X = k;
		asmap::iterator it = RHS_plus_map.find(X.attribute_set);

		if (it != RHS_plus_map.end()) {
			return (*it).second;
		}

		vector<int> X_choices = X.toVector();

		AttributeSet result;
		bool first = true;

		for (auto &A : X_choices) {
			X.erase(A);

			AttributeSet X_A_RHS_plus = get_RHS_plus(X);

			if (first) {
				result = X_A_RHS_plus;
				first = false;
			}
			else {
				result = result.intersect(X_A_RHS_plus);
			}

			X.insert(A);
		}

		RHS_plus_map[X] = result;
		it = RHS_plus_map.find(X);
		return it->second;
	}

	void calcualte_initial_RHS_plus(TANE_Layer &pre, TANE_Layer &cur) {
		for (auto &layer_record : cur.layer) {

			TANE_Node &node = layer_record.second;

			get_RHS_plus(node.as);
		}
	}

	void compute_dependecies(int col, TANE_Layer &pre, TANE_Layer &cur, ofstream& of) {

		calcualte_initial_RHS_plus(pre, cur);
		
		AttributeSet R((1 << col) - 1);
		AttributeSet choice;
		vector<int> choiceVector;
		ANmap::iterator it;
		AttributeSet X_E;
		vector<int> X_E_vector;

		double product_time = 0;
		double cal_denpendency_time = 0;

		for (auto &layer_record : cur.layer) {

			TANE_Node &node = layer_record.second;
			AttributeSet & node_RHS_plus = get_RHS_plus(node.as);

			choice = node_RHS_plus.intersect(node.as);

			double start = clock();
			
			if (choice.attribute_set != 0) {
				if (node.db != nullptr) {
					node.pt.fromExisted(node.db->initialCols[node.as.toVector()[0]]);
				}	
				else {
					node.pt.fromProduct(node.p1->pt, node.p2->pt, T, S);
				}
					
			}

			product_time += clock() - start;
			
			choiceVector = choice.toVector();
			
			start = clock();
			for (auto &E : choiceVector) {

				//E belongs to X intersect RHS+(X)

				X_E = node.as;
				X_E.erase(E);

				it = pre.layer.find(X_E.attribute_set);

				if (it->second.pt.cardinality() == node.pt.cardinality()) {

					//new FD found 
					//output
					X_E_vector = X_E.toVector();

					for (auto &t : X_E_vector) {
						t = t + 1;
					}
					fds.push_back(FD(X_E_vector, E + 1));



					//remove E from RHS+ i is E 's index
					node_RHS_plus.erase(E);

					//remove F belongs to R \ X from RHS+ 

					//a2 = R - X
					//AttributeSet a2 = R.substract(node.as);

					//a3 = RHS+ - a2 = RHS+ - (R - X)
					//AttributeSet a3 = node.RHS_plus.substract(a2);

					//node.RHS_plus = a3;
					node_RHS_plus = node_RHS_plus.substract(R.substract(node.as));
				}

				cal_denpendency_time += clock() - start;
			}
		}

		cout << "product time:" << product_time / CLOCKS_PER_SEC << endl;
		cout << "calc dependency time:" << cal_denpendency_time / CLOCKS_PER_SEC << endl;
	}

	void prune(TANE_Layer &pre, TANE_Layer & cur, ofstream& of) {
		ANmap::iterator p_layer = cur.layer.begin();
		vector<ANmap::iterator> remove_set;
		bool first_RHS = true;
		bool blank_RHS = false;
		AttributeSet judge_set;
		AttributeSet t;
		vector<int> X_vector;

		double start = clock();
		while (p_layer != cur.layer.end()) {
			TANE_Node &X = p_layer->second;
			AttributeSet &X_RHS_plus = get_RHS_plus(X.as);

			bool X_removed = false;

			if (X_RHS_plus.size() == 0) {
				X_removed = true;
				remove_set.push_back(p_layer);
			}

			//X is a super key?
			if (X.is_super_key()) {

				AttributeSet &X_RHS_plus = get_RHS_plus(X.as);

				// A belongs to RHS+(X) - X
				for (auto &A : X_RHS_plus.substract(X.as).toVector()) {
					first_RHS = true;
					blank_RHS = false;

					
					// judge set = forall B beglongs to X , intersect RHS+(X + A - B) 
					t = X.as;
					t.insert(A);

					for (auto &B : X.as.toVector()) {
						// t = X + A - B
						t.erase(B);

						//RHS+ (X + A -B)
						AttributeSet &r = get_RHS_plus(t);

						if (first_RHS == true) {
							judge_set = r;
							first_RHS = false;
						}
						else
							judge_set = judge_set.intersect(r);

						if (judge_set.attribute_set == 0) {
							break;
						}

						t.insert(B);
					}


					// A in judeg_set = intersect { RHS+(X+A-B) }
					if (judge_set[A] == 1) {
						//output X -> A
						//B_vector == X

						X_vector = X.as.toVector();
						for (auto &x : X_vector) {
							x = x+1;
						}

						fds.push_back(FD(X_vector,A+1));
					}

				}
				if (!X_removed)
					remove_set.push_back(p_layer);
				
			}

			p_layer++;
		}

		for (auto &layer_it : remove_set) {
			cur.layer.erase(layer_it);
		}

		cout << "prune time:" << (clock() - start)/CLOCKS_PER_SEC << endl;
	}

	void TANE_search_FD() {

		ofstream of("result.txt", ios::out);

		TANE_Layer *pre;
		TANE_Layer *cur;

		RHS_plus_map[AttributeSet(0)] = AttributeSet((1 << col) - 1);
		pre = new TANE_Layer(true, col);
		cur = new TANE_Layer(col, *pdb);
		int layerCount = 0;

		while (cur->size() != 0) {
			cout << layerCount << endl;
			++layerCount;
			compute_dependecies(col, *pre, *cur, of);

			prune(*pre, *cur, of);
			
			delete pre;
			pre = cur;
			cur = new TANE_Layer(*pre, col);

		}

		delete pre;
		delete cur;

		sort_FDs();
		print_FDs(of);
	}

	void solve() {
		TANE_search_FD();
	}

	void sort_FDs() {
		std::sort(fds.begin(), fds.end(), cmp_lt);
	}

	void print_FDs(ofstream &of) {
		for (auto &fd : fds) {
			for (auto &lv : fd.l) {
				of << lv << " ";
			}
			of << "->";
			of << " " << fd.r << endl;
		}
	}
};