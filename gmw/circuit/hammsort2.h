// sorting.h
#ifndef __SORTING_HAMM2_H_BY_BEBE_
#define __SORTING_HAMM2_H_BY_BEBE_

#include "circuit.h"
#include <cassert>

class CHammSorting2 : public CCircuit
{
public:
	BOOL Create(int nParties, const vector<int>& vParams);

public:

  void InitGates();
  void InitLayer();
  void InitParameters(int partiesCount, vector<int> params);
	int GetNumBits(int);
  int CalculateNonInputGates();

  //Computations
  vector< vector<int> > CalculateDistance();
  void Sorting(vector< vector<int> > party,vector< vector<int> > value);
  void ReorderOutput(vector< vector<int> > v);
  void ReorderOutput(vector< vector<int> > v1,vector< vector<int> > v2);
  vector< vector<int> > adjustData(vector< vector <int> > data, int referValue);
  vector<int> FindMinValue(vector<vector<int> > listValue);
  vector<int> ReverseBit(vector<int> v);
  vector<int> Reorder(vector<int> v);
  vector< vector<int> > Reorder(vector< vector<int> > v);

  //Components
  vector<int> PutHammDistCalculator(int partyId1, int partyId2, int inputSize);
  vector<int> SumAllBits(vector<int> v_bits);
  int PutNBitsComparator(vector<int> v_a, vector<int> v_b);
  vector<int> PutBitSwapper(int swapBit, int a, int b);
  vector< vector<int> > PutNBitsSwapper(int swapBit, vector<int> v_a, vector<int> v_b);

  //Additional gates;
  int PutNOTGate(int a);
  int PutORGate(int a, int b);
  int PutNANDGate(int a, int b);
  int PutNAND2Gate(int a, int b, int c);
  int PutNORGate(int a, int b);

  //For debugging
  void PrintInt(const string& name, int i);
  void PrintVector(const string& name, const vector<int>& params);
  void Debug(vector<int> v);
  void Debug(vector<vector<int> > v);

private:
  int m_nRep; //number of bits represents input value
  int m_nPartyLength; //number of bits represents party id
  int m_ndistBits; //number of bits represents hamming distance
  int m_nRefParties; //number of reference parties
  int m_nSortedParties; //number of non-reference parties;
  int m_nForZero;
  int m_nForOne;

  int nonInputGates;
  int totalInputBits; //number of bits represent party id + value
  int totalOutputBits;
  int hammCalculatorCount; //number of hamming calculator
  int counterCount; //number of counter
  int comparatorCount; //number of comparator
  int swapperCount; //number of swapper

  vector<int> v_listRefParties;
  vector<int> v_listSortedParties;
  vector<int> v_listPartiesStart;
  vector<int> v_listRefer;
  vector< vector<int> > v_listParties;


};

#endif //__SORTING_HAMM2_H_BY_BEBE_
