#ifndef HAMMSORTING_H
#define HAMMSORTING_H
/////////////////////////////////////////////////////////////////////////
// HammSorting.h - The circuit for 4 party hamming distance sorting	   //
// ver 1.0                                                             //
//---------------------------------------------------------------------//
// Author: Haoyi Shi											       //
// hshi05@syr.edu											           //
/////////////////////////////////////////////////////////////////////////
#include "circuit.h"
#include <cassert>
class CHammSorting :public CCircuit{
public:
	BOOL Create(int nParties, const vector<int>& vParams);
	int	GetNumBits(int decimal);
	int HammDistanceCalculator(int partyId1, int partyId2, int rep);
	int distCounter(vector<int> distVector);
	int exchange(int a, int b, int control, int rep);
	void PutOutputLayer(vector<int> sortedParties);
	void InitGates();
private:
	int m_nRep;
	int m_nPartyLength;
};
#endif