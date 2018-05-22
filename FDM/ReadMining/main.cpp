#include "Database.h"
#include "DisjointSet.h"
// #include "TANE-tree.h"
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
	cout << (clock() - start) / CLOCKS_PER_SEC << endl;

	start = clock();
	pdb->calcEquivalenceClass();
	cout << (clock() - start) / CLOCKS_PER_SEC << endl;

	cout << pdb->partition[12].size();
	// cout << pdb->table[0].size();
}

void testattr() {
	// AttributeSet as(4);
	
}
int main() {

	testds();
	
	system("pause");
}