//HammSortRef.h


#ifndef __HAMM_SORT_REF_H__
#define __HAMM_SORT_REF_H__

#include "circuit.h"
#include <cassert>


class CHammSortRef : public CCircuit
{
public:
	BOOL Create(int nParties, const vector<int>& vParams);

public:
	BOOL InitGates();
	int GetNumBits(int decimal);
	int PutXORGates(int a, int b, int rep);
	int PutBits(int vec, int rep);
	int calcDist(int pid);
	int ReverseBit(int a, int rep);
	int CountOnesNumber(int vecStart, int rep);
	void vecSort(vector<int>& vec, int rep);
	int PutGTGates(int a, int b, int rep);
	int calcGatesNum(int a, int la, int b, int lb);

	//caculate layer
	void InitLayer();
	void RunLayer();
	int PutLayerOutput();
	
private:
	int m_nVecLen;		//input vector length
	int m_nVectorLengthBits;//vector length bits length
	int m_nNumRef;		//number of reference parties
	int m_nNumNoRef;	// number of non-reference parties
	int m_nPartyNumBits;	//parties number bits length
	int m_nIDBits; 		//party id bits number
	
	vector<int> m_vRefBit;		//parties' reference bit
	vector<int> m_vInputVector;	//parties' input vector, m_nVecLen bits
	vector<int> m_vPartyID; 	//pid, m_nIDBits bits

	//layer
	vector<int> m_vLayerBuff;
	int m_nNumZero; //m_nVectorLengthBits length
	int m_nNumOne;	//m_nVectorLengthBits length

	//debug
	//int oStart, oEnd;
	//vector<int> mout;
};

#endif //__HAMM_SORT_REF_H__

