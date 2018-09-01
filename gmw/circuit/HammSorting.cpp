/////////////////////////////////////////////////////////////////////////
// HammSorting.cpp - Implementation of hamming distance sorting.		   //
// ver 1.0                                                             //
//---------------------------------------------------------------------//
// Author: Haoyi Shi											       //
// hshi05@syr.edu											           //
/////////////////////////////////////////////////////////////////////////
#include "HammSorting.h"
#include <cmath> 
#include <cassert>
using namespace std;
BOOL CHammSorting::Create(int nParties, const vector<int>& vParams)
{
	m_nNumParties = nParties;

	// parse params
	// params: vector length
	if (vParams.size() < (unsigned)1)
	{
		cout << "Error! This circuit needs " << 1
			<< "parameters for vector length"
			<< endl;
		return FALSE;
	}
cout<<"vParams[0] "<< vParams[0]<<endl;
	m_nRep = vParams[0];
	m_nPartyLength = GetNumBits(m_nNumParties-1	);
	m_vNumVBits.resize(m_nNumParties, 1);
	int distBits = GetNumBits(m_nRep)+1;
	m_vInputStart.resize(nParties);
	m_vInputEnd.resize(nParties);

	m_nFrontier = 2;
	for (int i = 0; i < nParties; i++)
	{
		m_vInputStart[i] = m_nFrontier;
		m_nFrontier = m_nFrontier+(m_nRep+m_nPartyLength);
		m_vInputEnd[i] = m_nFrontier - 1;
	}
	m_othStartGate = m_nFrontier;
	int gates_hamming =m_nNumParties*( m_nRep +4*distBits *pow(2, distBits));
	int gates_sorting = m_nNumParties*m_nNumParties*(4 * distBits+2*(7*m_nPartyLength)+2*(7*distBits));
	int gate_noninput = gates_hamming + gates_sorting+m_nNumParties*m_nPartyLength;
	
	m_nNumGates = gate_noninput + m_nFrontier;


	InitGates();

	//calculate the hamming distance between party1 and other party.
	
	vector<int> hammDist(m_nNumParties);
	for (int i = 1; i < m_nNumParties; i++)
	{
		hammDist[i]= HammDistanceCalculator(0, i, m_nRep);
	}


	//sort the parties according to the hamming distances
	vector<int> sortedParties(m_nNumParties);
	for (int i = 1; i < m_nNumParties; i++)
		sortedParties[i] = m_vInputStart[i] + m_nRep;

	for (int i = 1; i <m_nNumParties; i++)
	{
		for (int j =m_nNumParties-1 ; j >i; j--)
		{
			int gt= PutGTGate(hammDist[j], hammDist[j - 1], distBits);

			

			int newJ=PutMUXGate(sortedParties[j], sortedParties[j-1], gt, m_nPartyLength, false);
			//int newJ=exchange(sortedParties[j], sortedParties[j - 1], gt, m_nPartyLength);

			//int newJ_2=exchange(sortedParties[j-1], sortedParties[j], gt, m_nPartyLength);

			int newJ_2=PutMUXGate(sortedParties[j-1], sortedParties[j], gt, m_nPartyLength, false);

			sortedParties[j-1] = newJ;
			sortedParties[j] = newJ_2;


			//newJ = exchange(hammDist[j], hammDist[j - 1], gt, distBits);
			//newJ_2 = exchange(hammDist[j-1], hammDist[j], gt, distBits);
			newJ=PutMUXGate(hammDist[j], hammDist[j-1], gt, distBits, false);
			newJ_2=PutMUXGate(hammDist[j-1], hammDist[j], gt, distBits, false);

			hammDist[j-1] = newJ;
			hammDist[j] = newJ_2;
		}
	}


	PutOutputLayer(sortedParties);





	PatchParentFields();
	return TRUE;
}
//----------------------<function HammDistanceCalculator>-----------------------
// calculate the hamming distance between the vectors of two parties
// input: two parties ids.
int CHammSorting:: HammDistanceCalculator(int partyId1, int partyId2, int rep)
{
	int start_1 = m_vInputStart[partyId1];
	int start_2 = m_vInputStart[partyId2];

	vector<int> layer1;
	for (int i = 0; i < rep; i++)
	{
		int t = PutXORGate((start_1 + i), (start_2 + i));
		layer1.push_back(t);
	}
	
	return distCounter(layer1);
}

//------------------------<function distCounter>---------------------------
// count the 1s in a vector, the result starts from the lower bit
// eg. 2 is 01, 6 is 011.
int CHammSorting:: distCounter(vector<int> distVector)
{
	int vectorSize = distVector.size();
	int bitNum = GetNumBits(vectorSize);
	for (int i = vectorSize; i < pow(2, bitNum); i++)
	{
		distVector.push_back(0);
	}

	int newVectorSize = pow(2, bitNum);
	int length = 1;
	while (newVectorSize > 1)
	{ 
		vector<int> vectorTemp;
		for (int i = 0; i < newVectorSize ; i=i+2)
		{
			int out=PutAddGate(distVector[i], distVector[i + 1], length, true);
			vectorTemp.push_back(out);
		}
		newVectorSize = newVectorSize / 2;
		distVector.resize(newVectorSize);
		distVector = vectorTemp;
		length++;
	}
	
	return distVector[0];
}





int CHammSorting:: exchange(int a, int b, int control, int rep)
{
	vector<int> exchangedVector(rep);

	for (int i = 0; i < rep; i++)
	{
		int tempANDAControl = PutANDGate(a+i, control);
		int negControl = PutXORGate(control, 1);
		int tempANDNegcontrolB = PutANDGate(negControl, b+i);
		exchangedVector[i] = PutXORGate(PutANDGate(tempANDAControl, tempANDNegcontrolB), PutXORGate(tempANDAControl, tempANDNegcontrolB));
	}
	int result = m_nFrontier;
	for (int i = 0; i < rep; i++)
		PutXORGate(exchangedVector[i], 0);
//cout<<"exchange result: "<<result<<"  end:"<<m_nFrontier-1<<endl;
	return result;
}


//-----------------------<function GetNumBits>--------------------------
// gets # of bits for a decimal number
// input: decimal number
// output: # of bits for the input
// note: input should not be a negative int
int CHammSorting::GetNumBits(int decimal)
{
	int num_bits = 1;
	int tmp;

	if (decimal < 0){
		puts("CP2PCircuit::GetNumBits(int): negative input");
		exit(-1);
	}
	else if (decimal == 0){
		return 0;
	}
	else if (decimal == 1){
		return 1;
	}

	tmp = decimal;
	while (1 == 1){
		num_bits++;
		tmp = tmp / 2;
		if (tmp <= 1){
			break;
		}
	}

	return num_bits;
}
void CHammSorting:: PutOutputLayer(vector<int> sortedParties)
{
	int o_start = m_nFrontier;
	for (int i = 1; i < m_nNumParties; i++)
		for (int j = 0; j < m_nPartyLength; j++){
			PutXORGate(sortedParties[i]+j, 0);
		}
	int o_end = m_nFrontier - 1;
	m_vOutputStart.resize(m_nNumParties, o_start);
	m_vOutputEnd.resize(m_nNumParties, o_end);
	m_nNumGates = m_nFrontier;
}
void CHammSorting:: InitGates()
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