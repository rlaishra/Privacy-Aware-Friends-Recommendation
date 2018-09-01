//testsorting.cpp

#include "testsorting.h"

#include <cassert>

using namespace std;

BOOL CTestSortingCircuit::Create(int nParties, const vector<int>& vParams)

{

	m_nNumParties = nParties;

	if (vParams.size() < 1)

	{

		cout << "Error! This circuit needs one parameter: #items." << endl;

		return FALSE;

	}

	m_vNumVBits.resize(m_nNumParties, 1); //# of each party's value





	// gates for inputs

	m_vInputStart.resize(nParties);

	m_vInputEnd.resize(nParties);



	m_nFrontier = 2;

	m_vInputStart[0] = m_nFrontier;

	m_nFrontier += 3;

	m_vInputEnd[0] = m_nFrontier - 1;

	m_vInputStart[1] = m_nFrontier;

	m_nFrontier += 3;

	m_vInputEnd[1] = m_nFrontier - 1;

	m_vInputStart[2] = m_nFrontier;

	m_nFrontier += 3;

	m_vInputEnd[2] = m_nFrontier - 1;

	//======================================================================

	// Computes roughly # of gates

	int num_GTgates = 3;

	int num_gate_GTgates = num_GTgates * (4+4);

	int num_exchanger = 12;

	int num_gate_exchanger = num_exchanger* 6;

	m_othStartGate=m_nFrontier;

	m_nNumGates = m_nFrontier + num_gate_exchanger+num_gate_GTgates +6;// 6 gates for output



	m_vLayerOutputs.resize(6, 0);

	m_vOutputStart.resize(m_nNumParties, 0);

	m_vOutputEnd.resize(m_nNumParties, 0);	

	InitGates();

	int compAB=PutGTGate(m_vInputStart[0], m_vInputStart[1], 1);// compare the value of party0 and party1, each party's value is just 1 bit.

	int xPrime[2];

	for (int i = 1; i < 3;i++)

		xPrime[i - 1] = exchangeParties(m_vInputStart[0]+i, m_vInputStart[1]+i, compAB);

	int yPrime[2];

	for (int i = 1; i < 3; i++)

		yPrime[i - 1] = exchangeParties(m_vInputStart[1] + i, m_vInputStart[0] + i, compAB);

	

	int aPrime = getLarger(m_vInputStart[0], m_vInputStart[1]);

	int bPrime = getSmaller(m_vInputStart[0], m_vInputStart[1]);



	int compBC = PutGTGate(bPrime, m_vInputStart[2], 1);

	int yPrimePrime[2];

	for (int i = 1; i < 3; i++)

		yPrimePrime[i-1] = exchangeParties(yPrime[i-1], m_vInputStart[2] + i, compBC);

	int zPrime[2];

	for (int i = 1; i < 3; i++)

		zPrime[i - 1] = exchangeParties( m_vInputStart[2] + i, yPrime[i-1], compBC);

		

	int bPrimePrime = getLarger(bPrime, m_vInputStart[2]);

	int cPrime = getSmaller(bPrime, m_vInputStart[2]);



	int compAPBP = PutGTGate(aPrime, bPrimePrime, 1);

	int xPrimePrime[2];

	

	for (int i = 1; i < 3; i++)

		xPrimePrime[i - 1] = exchangeParties(xPrime[i - 1], yPrimePrime[i - 1], compAPBP);

	int yPrimePrimePrime[2];

	for (int i = 1; i < 3; i++)

		yPrimePrimePrime[i - 1] = exchangeParties(yPrimePrime[i - 1], xPrime[i - 1], compAPBP);



	int aPrimePrime = getLarger(aPrime, bPrimePrime);

	int bPrimePrimePrime = getSmaller(aPrime, bPrimePrime);

	// do the output.



	for (int i = 0; i < 2; i++)

		m_vLayerOutputs[i] = xPrimePrime[i];

	

	for (int i = 2; i < 4; i++)

		m_vLayerOutputs[i] = yPrimePrimePrime[i - 2];

	for (int i = 4; i < 6; i++)

		m_vLayerOutputs[i] = zPrime[i - 4];

		

	PutOutputLayer();



	PatchParentFields();



	return TRUE;

}





//-----------------------------<function exchangeParties>----------------------

// exchange the gate according to the control bit. If control is 1, output a, otherwise, output b.

int CTestSortingCircuit::exchangeParties(int a, int b, int control)

{

	int tempANDAControl = PutANDGate(a, control);

	int negControl = PutXORGate(control, 1);

	int tempANDNegcontrolB = PutANDGate(negControl, b);

	return PutXORGate(PutANDGate(tempANDAControl, tempANDNegcontrolB), PutXORGate(tempANDAControl, tempANDNegcontrolB));

}



//---------------------------------<function getLarger>----------------------

// return a, if a is larger 

int CTestSortingCircuit::getLarger(int a, int b)

{

	return PutXORGate(PutXORGate(a, b), PutANDGate(a, b));

}



//--------------------------------<function getSmaller>---------------------------

int CTestSortingCircuit::getSmaller(int a, int b)

{

	return PutANDGate(a, b);

}



void CTestSortingCircuit::InitGates()

{

	m_othStartGate = m_nFrontier;

	m_pGates = new GATE[m_nNumGates];

	m_nNumXORs = 0;



	GATE* gate;



	for (int i = 0; i<m_othStartGate; i++)

	{

		gate = m_pGates + i;

		gate->p_ids = 0;

		gate->p_num = 0;

		gate->left = -1;

		gate->right = -1;

		gate->type = 0;

	}

}



void CTestSortingCircuit::PutOutputLayer()

{

	int o_start = m_nFrontier;



	for (int i = 0; i < 3; i++)

		for (int j = 0; j < 2;j++)

			PutXORGate(m_vLayerOutputs[i*2+j], 0);

	int o_end = m_nFrontier - 1;

	m_vOutputStart.resize(m_nNumParties, o_start);



for (int i=0;i<m_nNumParties;i++)

	m_vOutputStart[i]=o_start;



	m_vOutputEnd.resize(m_nNumParties, o_end);

for (int i=0;i<m_nNumParties;i++)

	m_vOutputEnd[i]=o_end;

	m_nNumGates = m_nFrontier;

}