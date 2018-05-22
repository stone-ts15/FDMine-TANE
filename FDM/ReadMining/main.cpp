#include "Database.h"
#include "DisjointSet.h"
#include "TANE-tree.h"
#include <fstream>
#include <ctime>

void testdb() {
	bool test = true;

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
	// DSPartition ds;
	// ds.getPartitionFromTable(*pdb, AttributeSet(1));
	TANE_search_FD(12, *pdb);
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
	
	system("pause");
}