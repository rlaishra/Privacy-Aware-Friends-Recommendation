//testsorting.h



#ifndef __TEST_SORTING_H__BY_HAOYI_

#define __TEST_SORTING_H__BY_HAOYI_

#include "circuit.h"

#include <cassert>

class CTestSortingCircuit : public CCircuit{

public:

	BOOL Create(int nParties, const vector<int>& vParams);

public:

	void InitGates();

	int exchangeParties(int a, int b, int control);

	int getLarger(int a, int b);

	int getSmaller(int a, int b);

	void PutOutputLayer();



	vector<int> m_vLayerOutputs;

};



#endif