#pragma once

#include "Util.h"

// maximum of 32 attributes
class AttributeSet {
public:
	unsigned int attribute_set = 0;

public:
	AttributeSet() {}
	AttributeSet(unsigned int k) {
		attribute_set = k;
	}
	AttributeSet(const AttributeSet& src) : attribute_set(src.attribute_set) {}

	int operator [] (int index) {
		unsigned int k = 1 << index;
		return (k & attribute_set) >> index;
	}

	bool in(const AttributeSet &k) const {
		return !((attribute_set & k.attribute_set) ^ attribute_set);
	}

	const AttributeSet& operator = (const AttributeSet &k) {
		attribute_set = k.attribute_set;
		return k;
	}

	bool operator == (const AttributeSet &k) const {
		return attribute_set == k.attribute_set;
	}

	bool operator < (const AttributeSet &k) const {
		// has low index attribute's set will be put in the front
		unsigned int t = attribute_set;
		unsigned int tk = k.attribute_set;
		for (int i = 0; i < 32; i++) {
			unsigned int t_1 = t & 1;
			unsigned int tk_1 = tk & 1;
			if (t_1 > tk_1) {
				return true;
			}
			else if (t_1 < tk_1) {
				return false;
			}
			else {
				t = t >> 1;
				tk = tk >> 1;
			}
		}

		return false;
	}

	int size() {
		int count = 0;
		unsigned t = attribute_set;
		for (int i = 0; i < 32; i++) {
			if (t & 1)
				count++;

			t = t >> 1;
		}
		return count;
	}

	void assign(AttributeSet &k) {
		attribute_set = k.attribute_set;
	}

	void insert(int k) {
		attribute_set = attribute_set | (1 << k);
	}

	void erase(int k) {
		attribute_set = attribute_set & (~(1 << k));
	}

	vector<int> toVector() const {
		vector<int> vec;
		unsigned t = attribute_set;
		for (int i = 0; i < 32; i++) {
			if (t & 1)
				vec.push_back(i);
			t = t >> 1;
		}

		return vec;
	}

	void clear() {
		attribute_set = 0;
	}

	bool common_prefix(AttributeSet &k) {
		int sz = size();

		int count = 0;
		unsigned t = attribute_set;
		unsigned tk = k.attribute_set;

		for (int i = 0; i < 32; i++) {
			if ((t & 1) && (tk & 1))
				count++;
			else if ((t & 1) || (tk & 1))
				break;

			t = t >> 1;
			tk = tk >> 1;
		}

		if (count == sz - 1)
			return true;
		else
			return false;

	}

public:
	AttributeSet intersect(const AttributeSet &b) {
		return AttributeSet(attribute_set & b.attribute_set);
	}

	AttributeSet combine(const AttributeSet &b) {
		return AttributeSet(attribute_set | b.attribute_set);
	}

	AttributeSet substract(const AttributeSet &b) {
		unsigned int temp = ~b.attribute_set;
		return AttributeSet(attribute_set & temp);
	}
};