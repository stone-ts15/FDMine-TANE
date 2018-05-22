#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <list>
#include <string>
#include <set>
using namespace std;

namespace util {
	const int hashRange = 10007;

	int hashString(string str) {
		int count = 0;
		int sum = 0;
		for (auto& ch : str) {
			sum += ((ch - '0') * 31) ^ count;
		}
		return sum % hashRange;
	}

	long long hashRoot(int root1, int root2) {
		return (root1 << 17) + root2;
	}
}