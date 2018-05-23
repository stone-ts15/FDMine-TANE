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
	unordered_map<long long, int> hashRoots;
	set<int> singleRoots;

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
}