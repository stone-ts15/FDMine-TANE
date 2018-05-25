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
#include <thread>
#include <mutex>
using namespace std;


namespace util {
	const unsigned hashRange = 1000000007;
	const unsigned numth = 3;
	const unsigned rowlen = 210;
	const unsigned maxcollen = 100000;

	int hashString(string str) {
		int count = 0;
		int sum = 0;
		for (auto& ch : str) {
			sum += ((ch - '0') * 31) ^ count;
		}
		return sum % hashRange;
	}

	inline int hashRow(int row) {
		return -(row + 1);
	}

	inline int inverseHashRow(int hashrow) {
		return -hashrow - 1;
	}

	inline int mmin(int x, int y) {
		return x < y ? x : y;
	}

	size_t collen = 0;
}