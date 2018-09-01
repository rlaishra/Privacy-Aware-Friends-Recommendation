//HanmmSortRef.cpp


#include "HammSortRef.h"
#include <cassert>
#include <cmath>


using namespace std;


int CHammSortRef::calcGatesNum(int a, int la, int b, int lb)
{
	int rtn = 0;
	/*
	//input
	rtn += 2 + a * (1 + b + la);
	
	//init
	rtn += 2 * (lb);

	//run (rep)
	rtn += a * (
			(a-1) * (
				(b + 9*b*lb + 1 + 2*lb + la) + //xor and cont one's number
				9*(1 + lb + la)			//find minimal
			) + 
			
			(1 + lb + 2 * la)		//merge to the result
		);

	//run (sort)	
	rtn += a * (a-1) * 6 * (1 + lb + 2*la);
	
	//output
	rtn += (lb + 2*la) * m_nNumNoRef;
	*/

	//simplify
	rtn = 	2 - 14*a + 16*a*a + a*a*b - 19*a*la + 22*a*a*la + 
		2*lb - 16*a*lb + 17*a*a*lb - 9*a*b*lb + 9*a*a*b*lb +
		+ (2*la+lb)*m_nNumNoRef;
		
	return rtn;
}


BOOL CHammSortRef::Create(int nParties, const vector<int>& vParams){

	//vParams[] = {num_parties, num_vector_length, num_reference}
	/*
	cout << "nParties = " << nParties << endl;
	cout << "Number of vParams = " << vParams.size() << endl;
	for (int i = 0; i < vParams.size(); i++)
	{
		cout << i << ' ' << vParams[i] << endl;
	}
	*/

	if (vParams.size() < 3)
	{
		cout << "Error! This circuit needs "
			"3 parameters for vector length" << endl;
		return false;
	}

	m_nNumParties = nParties;
	if (m_nNumParties < 2)
	{
		cout << "Error! The parties number should larger than or equal to 2" << endl;
		return false;
	}
	m_vNumVBits.resize(m_nNumParties, 1);

	m_nVecLen = vParams[1];
	m_nNumRef = vParams[2];
	if (m_nVecLen < 1)	
	{
		cout << "Error! Input vector length should larger than 0!" << endl;
		return false;
	}


	m_nNumNoRef = nParties - m_nNumRef;
	if (m_nNumNoRef < 1 || m_nNumRef < 1)	
	{
		cout << "Error! Reference parties number should larger than 0 and less than parties number!" << endl;
		return false;
	}
	m_nVectorLengthBits = GetNumBits(m_nVecLen);
	m_nPartyNumBits = GetNumBits(nParties-1);
	m_nIDBits = m_nPartyNumBits;
	
	
	//put input layer
	m_vInputStart.resize(nParties);
	m_vInputEnd.resize(nParties);

	m_vRefBit.resize(nParties);
	m_vInputVector.resize(nParties);
	m_vPartyID.resize(nParties);

	m_nFrontier = 2;
	for (int i = 0; i < nParties; i++)
	{
		m_vInputStart[i] = m_nFrontier;
		m_nFrontier += 1 + m_nVecLen + m_nIDBits;
		m_vInputEnd[i] = m_nFrontier - 1;

		m_vRefBit[i] = m_vInputStart[i];
		m_vInputVector[i] = m_vInputStart[i] + 1;
		m_vPartyID[i] = m_vInputStart[i] + 1 + m_nVecLen;
	}

	//calculate total gates number
	m_nNumGates = calcGatesNum(nParties, m_nPartyNumBits, m_nVecLen, m_nVectorLengthBits);

	m_nNumXORs = 0;

	if (!InitGates())
	{
		return false;
	}
	//cout << "m_othStartGate = " << m_othStartGate << endl;
	
	//caculate layer
	InitLayer();
	
	RunLayer();
	
	int OutStart = PutLayerOutput();
	int outEnd = m_nFrontier-1;

	m_vOutputStart.resize(m_nNumParties, OutStart);
	m_vOutputEnd.resize(m_nNumParties, outEnd);
	
	//cout << "start patch" << endl;
	cout << "total gates: " << m_nNumGates << endl;
	if (m_nNumGates != m_nFrontier)
	{
		cout << "Error! This circuit total gates number " << m_nNumGates <<
		"is not equal to real alloc gates number" << m_nFrontier << endl;
		return false;
	}
	PatchParentFields();
	return true;
	
}


int CHammSortRef::PutGTGates(int a, int b, int rep)
{
	a = ReverseBit(a, rep);
	b = ReverseBit(b, rep);
	return PutGTGate(a, b, rep);
}


BOOL CHammSortRef::InitGates()
{
	m_pGates = new GATE[m_nNumGates + 10];
	if (m_pGates == NULL)
	{
		cout << "Need Alloc " << m_nNumGates << "+10 gates." << endl;
		cout << "Alloc failed due to m_pGates == NULL." << endl;
		return false;
	}
	m_othStartGate = m_nFrontier;
	GATE* gate;
	for (int i = 0; i < m_othStartGate; i++)
	{
		gate = m_pGates + i;
		gate->p_ids = 0;
		gate->p_num = 0;
		gate->left = -1;
		gate->right = -1;
		gate->type = 0;
	}
	return true;
}


int CHammSortRef::GetNumBits(int decimal){
	int num_bits = 0;
	if (decimal < 0)
	{
		cout << "GetNumBits(int): negative input" << endl;
		exit(-1);
	}
	if (decimal == 0) return 0;
	while (decimal)
		decimal >>= 1,
		num_bits++;
	return num_bits;
}


int CHammSortRef::PutXORGates(int a, int b, int rep)
{
	int out = m_nFrontier;
	for (int i=0; i<rep; i++)
		PutXORGate(a+i, b+i);
	return out;
}


int CHammSortRef::PutBits(int vec, int rep)
{
	int out = m_nFrontier;
	for (int i=0; i<rep; i++)
		PutXORGate(vec+i, 0);
	return out;
}


int CHammSortRef::calcDist(int pid)
{
	vector<int> buff;
	int rst, rep, gt;
	for (int i = 0; i < m_nNumParties; i++) if (i != pid)
	{
		rst=PutXORGates(m_vInputVector[i], m_vInputVector[pid], m_nVecLen);
		rst=CountOnesNumber(rst, m_nVecLen); //count one's number, the result is m_nVectorLengthBits length
		//buff[]={reference bit + distance between current party + n-th id}
		buff.push_back(PutBits(m_vRefBit[i], 1));
		
		PutBits(rst, m_nVectorLengthBits);
		PutBits(m_vPartyID[i], m_nIDBits);
	}
	//find minimal distance
	rep = 1 + m_nVectorLengthBits + m_nIDBits;
	rst = buff[0];
	for (int i = 0; i < buff.size(); i++)
	{
		gt = PutGTGates(buff[i], rst, rep);
		rst = PutMUXGate(rst, buff[i], gt, rep, 0);
	}
	return rst+1; //return vector without reference bit
}


int CHammSortRef::ReverseBit(int a, int rep)
{
	int out = m_nFrontier;
	for(int i = rep-1; i >= 0; i--)
		PutXORGate(a+i, 0);
	return out;
}


int CHammSortRef::CountOnesNumber(int vecStart, int len)
{
	int a = m_nNumZero, b;
	int rep = m_nVectorLengthBits;

	for (int i = 0; i < len; i++)
	{
		b = PutMUXGate(m_nNumOne, m_nNumZero, vecStart+i, rep);
		a = PutAddGate(a, b, rep, true);
	}
	a = ReverseBit(a, rep);
	return a;
}


void CHammSortRef::vecSort(vector<int>& vec, int rep)
{
	int n = vec.size();
	int i, j, a, b, gt;
	for (i = 1; i < n; i++)
	{
		for (j = n-1; j >= i; j--)
		{
			gt = PutGTGates(vec[j-1], vec[j], rep);
			a = PutMUXGate(vec[j], vec[j-1], gt, rep);
			b = PutMUXGate(vec[j-1], vec[j], gt, rep);
			
			vec[j-1] = a;
			vec[j] = b;	
		}
	}
}


void CHammSortRef::InitLayer(){
	m_vLayerBuff.resize(m_nNumParties);
	
	m_nNumZero = m_nFrontier;
	for (int i = 0; i < m_nVectorLengthBits; i++)
	{
		PutXORGate(0,0);
	}
	
	m_nNumOne = m_nFrontier;
	PutXORGate(1,0);
	for (int i = 0; i < m_nVectorLengthBits-1; i++)
	{
		PutXORGate(0,0);
	}	
}


void CHammSortRef::RunLayer(){
	int rst;
	
	//rep
	for (int i = 0; i < m_nNumParties; i++)
	{
		//calculate i-th party's minimal distance between reference parties
		rst = calcDist(i); //distance + minimal reference id
		m_vLayerBuff[i] = PutXORGate(m_vRefBit[i], 1);
		PutBits(rst, m_nVectorLengthBits + m_nIDBits);
		PutBits(m_vPartyID[i], m_nIDBits);
	}
	
	//sort
	vecSort(m_vLayerBuff, 1 + m_nVectorLengthBits + m_nIDBits + m_nIDBits);
}


int CHammSortRef::PutLayerOutput()
{
	int out = m_nFrontier;
	for (int i = 0; i < m_nNumNoRef; i++)
	{
		PutBits(m_vLayerBuff[i]+1, m_nVectorLengthBits + 2 * m_nIDBits);
	}
	return out;
}

