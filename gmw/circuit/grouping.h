//  grouping.h

#ifndef __GROUPING__BY_KLOU_
#define __GROUPING__BY_KLOU_

#include "circuit.h"
#include <cassert>

class CGrouping:public CCircuit
{
public:
	BOOL Create(int nParties, const vector<int>& vParams);
public:
	void InitGates();	
	int PutORGate(int a, int b);
	void PutOuputLayer();	
private:
	int m_nItems;
	int gates_noninput;
	vector<int> m_vLayerOutput;
};


#endif //__GROUPING__BY_KLOU_
