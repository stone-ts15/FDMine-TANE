#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <string>
#include <set>
#include <array>
using namespace std;

namespace util {
	const int hashRange = 10007;
	const int collen = 99918;
	map<long long, int> roots;
	set<int> singleRoots;
	unordered_map<long long, int> hashRoots;
	unordered_map<string, int> equivalenceClass;
	unordered_map<string, int> admap;
	unordered_map<string, pair<int, int>> smap;

	int hashString(string str) {
		int count = 0;
		int sum = 0;
		for (auto& ch : str) {
			sum += ((ch - '0') * 31) ^ count;
		}
		return sum % hashRange;
	}

	long long hashRoot(long long root1, long long root2) {
		return (root1 << 17) + root2;
	}

	inline int hashRow(int row) {
		return -(row + 1);
	}

	inline int inverseHashRow(int hashrow) {
		return -hashrow - 1;
	}

	long long hashn(long long arg0, long long arg1, long long arg2, long long arg3) {
		return (arg0 << 39) + (arg1 << 26) + (arg2 << 13) + arg3;
	}

	long long hashn(array<int, 4>& arg) {
		return (((long long)arg[0]) << 39) + (((long long)arg[1]) << 26) + (((long long)arg[2]) << 13) + arg[3];
	}
}