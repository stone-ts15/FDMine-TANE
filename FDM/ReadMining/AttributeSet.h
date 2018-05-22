/*#pragma once
#include <vector>
using std::vector;


class AttributeSet {
public:
	vector<vector<vector<int>*>*>* pGraph;

private:
	vector<int>* pYangTriangle;

public:
	AttributeSet(unsigned size) : pGraph(NULL) {
		pGraph = new vector<vector<vector<int>*>*>(size);
		generateYang();
	}

private:
	void generateYang() {
		unsigned layers = size();
		pYangTriangle = new vector<int>(layers + 1);
		(*pYangTriangle)[0] = 1;
		for (int i = 1; i <= layers; ++i) {
			for (int j = i; j > 0; --j) {
				(*pYangTriangle)[j] += (*pYangTriangle)[j - 1];
			}
		}
	}

	inline unsigned size() {
		return pGraph ? pGraph->size() : 0;
	}

	void generateSingleLayer(unsigned layer) {
		// layer in [1, size]
		unsigned currentSize = 0;
	}


};*/