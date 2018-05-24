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

	long long hashn(array<int, 4>& arg) {
		return (((long long)arg[0]) << 39) + (((long long)arg[1]) << 26) + (((long long)arg[2]) << 13) + arg[3];
	}

	long long hashn(long long* args) {
		return (args[0] << 39) | (args[1] << 26) | (args[2] << 13) | args[3];
	}
}