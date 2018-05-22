#include "Database.h"
#include "DisjointSet.h"
#include "TANE-tree.h"
#include <fstream>
#include <ctime>

void testdb() {
	bool test = false;

	Database *pdb;
	ifstream *pfin;

	if (test) {
		pdb = new Database(12);
		pfin = new ifstream("test_data.txt");
	}
	else {
		pdb = new Database(15);
		pfin = new ifstream("data.txt");
	}

	double start;

	start = clock();
	pdb->getTable(*pfin);
	TANE_search_FD(pdb->table.size(), *pdb);
	cout << (clock() - start) / CLOCKS_PER_SEC << endl;

	//start = clock();
	//pdb->calcEquivalenceClass();
	//cout << (clock() - start) / CLOCKS_PER_SEC << endl;

	/*start = clock();
	DSPartition *pp[15];
	AttributeSet as;
	for (unsigned i = 0; i < 15; ++i) {
		as.clear();
		as.insert(i);
		pp[i] = new DSPartition(*pdb, as);
	}
	cout << (clock() - start) / CLOCKS_PER_SEC << endl;*/


	//cout << pdb->partition[12].size();
	// cout << pdb->table[0].size();
}

void testattr() {
	// AttributeSet as(4);
	
}

void testhashpartition() {

}
int main() {

	testdb();
	//cout << util::hashRoot(63564, 108630) << endl;
	//cout << util::hashRoot(30796, 108630) << endl;
	
	system("pause");
}