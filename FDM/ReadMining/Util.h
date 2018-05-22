#pragma once
#include <iostream>
#include <vector>

namespace util {
	const int hashRange = 10007;
	int hash(string str) {
		int count = 0;
		int sum = 0;
		for (auto& ch : str) {
			sum += ((ch - '0') * 31) ^ count;
		}
		return sum % hashRange;
	}
}
