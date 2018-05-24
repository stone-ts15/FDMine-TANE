#include "Database.h"
#include "DisjointSet.h"
#include "TANE-tree.h"
#include <fstream>
#include <ctime>
#include "Solver.h"

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
	cout << (clock() - start) / CLOCKS_PER_SEC << endl;

	Solver solver(pdb);
	cout << (clock() - start) / CLOCKS_PER_SEC << endl;
	solver.solve();
	cout << (clock() - start) / CLOCKS_PER_SEC << endl;


}

int main() {

	testdb();
	system("pause");
}