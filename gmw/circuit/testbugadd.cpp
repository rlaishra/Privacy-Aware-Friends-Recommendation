#include "testbugadd.h"
#include <cassert>
#include <cmath>
using namespace std;

BOOL TestBugAdd::Create(int nParties, const vector<int>& vParams)
{
	// parse params
	// params: vector length
	if (vParams.size() < (unsigned)1)
	{
		cout << "Error! This circuit needs " << 1
			<< "parameters for vector length"
			<< endl;
		return FALSE;
	}

  InitParameters(nParties, vParams);
	InitGates();


  vector<int> f;
  f.push_back(0);
  f.push_back(PutANDGate(0,0));
  f.push_back(PutANDGate(0,1));
  f.push_back(PutANDGate(1,0));
  f.push_back(PutANDGate(1,1));
  Debug(f);


  PatchParentFields();

  return TRUE;
}

int TestBugAdd::AND(int a, int b)
{
	int out = m_nFrontier++;
	GATE* gate;

	gate = m_pGates + out;
	gate->type = 1;
	gate->p_num = 0;
	gate->p_ids = 0;

	gate->left = a;
	m_pGates[gate->left].p_num++;
	gate->right = b;
	m_pGates[gate->right].p_num++;
  out = b;
	return out;
}

vector< vector<int> > TestBugAdd::CalculateDistance(){
  cout << "Start calculating distance" << endl;
  vector< vector<int> > ret;

  for(int i=1;i<m_nNumParties;i++)
  {
    vector<int> v_hamm = PutHammDistCalculator(0,i,m_nRep);
    ret.push_back(v_hamm);
  }

  cout << "End calculating distance" << endl;
  return ret;
}

void TestBugAdd::Sorting(vector< vector<int> > party, vector< vector<int> > value){
  cout << "Start sorting " << endl;
  int count = 0;
  int offset = 1;
  int startIndex = 0;
  vector< vector<int> > v_listComp;

  while(count<comparatorCount)
  {
    int a_index = startIndex;
    int b_index = startIndex+offset;
    vector<int> comp;

    if(b_index >= (m_nNumParties-1)) {
      offset++;
      startIndex = 0;
      a_index = startIndex;
      b_index = startIndex+offset;
    }
    cout << "a: " << a_index << " b: " << b_index << endl;//" swapbit: " << swapBit << endl;
    int swapBit = PutNBitsComparator(value[a_index],value[b_index]);


    vector< vector<int> > v_swapper = PutNBitsSwapper(swapBit,value[a_index],value[b_index]);
    value[a_index] = v_swapper[0];
    value[b_index] = v_swapper[1];

    vector< vector<int> > p_swapper = PutNBitsSwapper(swapBit,party[a_index],party[b_index]);
    party[a_index] = p_swapper[0];
    party[b_index] = p_swapper[1];

    startIndex++;
    count++;
  }

  ReorderOutput(party,value);
}


void TestBugAdd::ReorderOutput(vector< vector<int> > party, vector< vector<int> > value){
  vector<int> output;
  cout << "Reoder" << endl;

  for(int i=0;i<(m_nNumParties-1);i++)
  {
    PrintVector("v_listValues",value[i]);
    for(int j=0;j<value[i].size();j++){
      int gate = value[i][j];
      int a1 = PutANDGate(gate,gate);
      output.push_back(a1);
    }
  }


  for(int i=0;i<(m_nNumParties-1);i++)
  {
    PrintVector("v_listParties",party[i]);
    for(int j=0;j<party[i].size();j++){
      int gate = party[i][j];
      int a1 = PutANDGate(gate,gate);
      output.push_back(a1);
    }
  }

  m_vOutputStart.resize(m_nNumParties, output.front());
  m_vOutputEnd.resize(m_nNumParties, output.back());

  PrintVector("m_vOutputStart",m_vOutputStart);
  PrintVector("m_vOutputEnd",m_vOutputEnd);
}


void TestBugAdd::InitParameters(int partiesCount, vector<int> params){
  m_nNumParties = partiesCount;
  m_nRep = params[0];
	m_nPartyLength = GetNumBits(m_nNumParties); // -1 in ()
  m_ndistBits = GetNumBits(m_nRep); //+1 out()
  totalInputBits = m_nPartyLength + m_nRep;
  totalOutputBits = (m_ndistBits + m_nPartyLength) * (m_nNumParties-1);
	m_vNumVBits.resize(m_nNumParties, 1);
	m_vInputStart.resize(m_nNumParties);
	m_vInputEnd.resize(m_nNumParties);
  m_nFrontier = 2; //Input Gate starts at 2, 0 and 1 are allocated for 0,1 bit

  PrintInt("Input",m_nRep);
  PrintInt("m_nNumParties",m_nNumParties);
  PrintInt("m_nPartyLength",m_nPartyLength);
  PrintInt("m_ndistBits",m_ndistBits);
  PrintInt("totalInputBits",totalInputBits);
  PrintInt("totalOutputBits",totalOutputBits);


  //Count number of gates according to number of inputs.
  v_listPartiesStart.resize(m_nNumParties-1);
  v_listParties.resize(m_nNumParties-1);
  for (int i = 0; i < m_nNumParties; i++)
  {
    m_vInputStart[i] = m_nFrontier;
    m_nFrontier = m_nFrontier+(m_nRep+m_nPartyLength);
    m_vInputEnd[i] = m_nFrontier - 1;
    if(i!=0) v_listPartiesStart[i-1] = (m_vInputEnd[i] - m_nPartyLength)+1;
  }

  PrintVector("m_vInputStart",m_vInputStart);
  PrintVector("m_vInputEnd",m_vInputEnd);


  //Put party id gate to vector
  for(int i=0;i<(m_nNumParties-1);i++)
  {
    for(int j=0;j<m_nPartyLength;j++){
      int g = v_listPartiesStart[i]+j;
      v_listParties[i].push_back(g);

    }
    PrintVector("v_listParties", v_listParties[i]);
  }

  //m_othStartGate = 1-st non-input gate
  m_othStartGate = m_nFrontier;

  //Component Estimation
  hammCalculatorCount = m_nNumParties - 1;
  counterCount = hammCalculatorCount;
  comparatorCount = (hammCalculatorCount * (hammCalculatorCount - 1))/2;
  swapperCount = comparatorCount*2;

  nonInputGates = CalculateNonInputGates();
	m_nNumGates =  nonInputGates + m_nFrontier;

  PrintInt("nonInputGates",nonInputGates);

  cout << "Finish Initial Params " << endl;
}

vector<int> TestBugAdd::PutHammDistCalculator(int partyId1, int partyId2, int inputSize){
  vector<int> ret;
  vector<int> hammDistBits;
  int noAdder = m_ndistBits;
  int start_1 = m_vInputStart[partyId1];
  int start_2 = m_vInputStart[partyId2];

  //Calculate Hamming distance
  for(int i=0;i<inputSize;i++)
  {
    int t = PutXORGate(start_1 + i,start_2 + i);
    hammDistBits.push_back(t);
    cout << start_1+i << "  " << start_2+i << endl;
  }
  cout << "end " << inputSize << endl;

  ret = SumAllBits(hammDistBits);
  //ret = hammDistBits;
  return ret;
}

vector<int> TestBugAdd::SumAllBits(vector<int> v_bits){
  int vectorSize = v_bits.size();
  int bitNum = GetNumBits(vectorSize);

  for (int i = vectorSize; i < pow(2, bitNum); i++){
    v_bits.push_back(0);
    cout << "[b] add" << i << endl;
  }

  int newVectorSize = pow(2, bitNum);
  int length = 1;
  while (newVectorSize > 1)
  {
    vector<int> vectorTemp;
    for (int i = 0; i < newVectorSize ; i=i+2)
    {
      int out=PutAddGate(v_bits[i], v_bits[i + 1], length, true);
      vectorTemp.push_back(out);
    }
    newVectorSize = newVectorSize / 2;
    v_bits.resize(newVectorSize);
    v_bits = vectorTemp;
    length++;
  }

  vector<int> ret;

  for(int i=(bitNum-1);i>=0;i--){
    ret.push_back(v_bits[0]+i);
  }
  return ret;
}

int TestBugAdd::PutNBitsComparator(vector<int> v_a, vector<int> v_b){
  int ret = PutGTGate(v_a[0],v_b[0],v_a.size());
  /*int ret = 0;
  int bitSize = v_a.size();
  int noAndGates = bitSize;
  int noOrGates = bitSize - 1;

  vector<int> v_and;

  //Adding the and gates
  for(int i=0;i<bitSize;i++){
    int not_b = PutNOTGate(v_b[i]);
    int and_1 = PutANDGate(v_a[i],not_b);

    if(i!=0){
      int tmp = and_1;
      for(int index=0;index<v_and.size();index++){
        int and_not = PutNOTGate(v_and[index]);
        tmp = PutANDGate(tmp, and_not);
      }
      and_1 = tmp;
    }
    v_and.push_back(and_1);
  }

  //Adding the or gates
  int tmp = v_and[0];

  for(int i=1;i<v_and.size();i++){
    tmp = PutORGate(tmp, v_and[i]);
  }

  ret = tmp;*/
  return ret;
}

vector< vector<int> > TestBugAdd::PutNBitsSwapper(int swapBit, vector<int> v_a, vector<int> v_b){
  int newA = PutMUXGate(v_a[0],v_b[0],swapBit, m_ndistBits,false);
  //int newB = PutMUXGate(v_b[0],v_a[0],swapBit, m_ndistBits,false);

  vector<int> v_outA;
  vector<int> v_outB;

  for(int i=0;i<m_ndistBits;i++) {
    v_outA.push_back(newA+i);
    //v_outB.push_back(newB+i);
  }
  //for(int i=0;i<m_ndistBits;i++)

  PrintVector("v_outA",v_outA);
  //PrintVector("v_outB",v_outB);


  vector< vector<int> > ret;
  /*int bitsCount = v_a.size();
  int bitsCount2 = v_b.size();

  //PrintInt("Swapper - bit count", bitsCount);

  vector<int> v_outA;
  vector<int> v_outB;
  for(int i=0;i<bitsCount;i++)
  {

    vector<int> v = PutBitSwapper(swapBit, v_a[i],v_b[i]);
    if(bitsCount != 2) {
      PrintInt("ccccccc",i);
      PrintVector("dddd", v);
    }
    v_outA.push_back(v[0]); //a
    v_outB.push_back(v[1]); //b
  }*/

  ret.push_back(v_outA);
  //ret.push_back(v_outB);
  PrintVector("v_outA",v_outA);
  return ret;
}

//4 AND gates, 2 ORs gates
vector<int> TestBugAdd::PutBitSwapper(int swapBit, int a, int b){
  vector<int> ret;

  //int newA = PutMUXGate(a,b,)
  /*int not_swapBit = PutNOTGate(swapBit);

  int and_1 = PutANDGate(not_swapBit, a);
  int and_2 = PutANDGate(swapBit,b);

  int or_1 = PutXORGate(and_1,and_2);


  int and_3 = PutANDGate(swapBit,a);
  int and_4 = PutANDGate(not_swapBit,b);


  int or_2 = PutXORGate(and_3,and_4);

  ret.push_back(or_1); // a'
  ret.push_back(or_2); // b'*/

  return ret;
}

void TestBugAdd::InitGates(){
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

int TestBugAdd::CalculateNonInputGates(){


  int oneComparatorGates = (1*m_ndistBits) + (3*m_ndistBits);

  for(int i=m_ndistBits;i>0;i--)
  {
    oneComparatorGates += i;
  }

  int count = 0;
  count += (hammCalculatorCount * m_nRep);
  count += (7 * m_ndistBits * m_nRep ) * counterCount;
  count += (oneComparatorGates * comparatorCount);
  count += ((11 * m_ndistBits) + (11 * m_nPartyLength)) * swapperCount;
  count += totalOutputBits;
  return count;
}


//1 Not = 1 Gates
int TestBugAdd::PutNOTGate(int a){
  int x = PutXORGate(a,1);
  return x;
}

//1 OR = 3 Gates
int TestBugAdd::PutORGate(int a, int b){
  int x = PutXORGate(a,b);
  int y = PutANDGate(a,b);
  int z = PutXORGate(x,y);

  return z;
}

int TestBugAdd::PutNANDGate(int a, int b){
  int and_1 = PutANDGate(a,b);
  int ret = PutNOTGate(and_1);

  return ret;
}

int TestBugAdd::PutNAND2Gate(int a, int b, int c){
  int and_1 = PutANDGate(a,b);
  int and_2 = PutANDGate(and_1,c);
  int ret = PutNOTGate(and_2);

  return ret;
}

int TestBugAdd::PutNORGate(int a, int b){
  int or_1 = PutORGate(a,b);
  int ret = PutNOTGate(or_1);

  return ret;
}


int TestBugAdd::GetNumBits(int decimal){
    int num_bits = ceil(log2(decimal));
    if(decimal == 2) num_bits = 2;

    return num_bits;
}

void TestBugAdd::PrintInt(const string& name, int i){
  cout << "[BB] == " << name << " = "<< i << endl;
}

void TestBugAdd::PrintVector(const string& name, const vector<int>& params){
  cout << "== V: " << name << "(" << params.size() << ") :";
  for (vector<int>::const_iterator i = params.begin(); i != params.end(); ++i){
		cout << *i << " ";
	}
  cout << "=====" << endl;
}

void TestBugAdd::Debug(vector<int> v){
  cout << "Debugging" << endl;
  vector<int> t;

  PrintInt("m_nFrontier",m_nFrontier);
  for(int i=0;i<v.size();i++){
    int a = PutXORGate(v[i],0);
    t.push_back(a);
  }

  m_vOutputStart.resize(m_nNumParties,t.front());
  m_vOutputEnd.resize(m_nNumParties,t.back());

  PrintVector("m_vOutputStart",m_vOutputStart);
  PrintVector("m_vOutputEnd",m_vOutputEnd);


}
