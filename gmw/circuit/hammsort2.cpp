#include "hammsort2.h"
#include <cassert>
#include <cmath>
using namespace std;

BOOL CHammSorting2::Create(int nParties, const vector<int>& vParams)
{
	// parse params
	// params: vector length
	if (vParams.size() < (unsigned)1)
	{
		cout << "Error! This circuit needs " << 1	<< "parameters for vector length"	<< endl;
		return FALSE;
	}

  InitParameters(nParties, vParams);
	InitGates();
	InitLayer();
	vector<vector<int> > v_listDistance = CalculateDistance();

	//v_listDistance = adjustData(v_listDistance,1);


	Sorting(v_listParties, v_listDistance);


	//Debug(temp);
	//

	//Debug(v_listDistance[0][3]);

  PatchParentFields();
	PrintInt("m_nNumXORs*",m_nNumXORs);
	cout << m_nNumGates << " - " << m_othStartGate << " - " << m_nNumXORs << endl;
	cout << (m_nNumGates-m_othStartGate-m_nNumXORs) << endl;
	cout << GetNumANDs() << " NumANDs " << endl;
	cout << p_totalGates << "total gates" << endl;

  return TRUE;
}

//Computation Functions
vector< vector<int> > CHammSorting2::CalculateDistance(){
  //cout << "Start calculating distance" << endl;
	vector<vector<int> > allOutput;
  vector< vector<int> > ret;

  for(int i=0;i<m_nNumParties;i++)
	{
		ret.resize(m_nNumParties);

    for(int j=0;j<m_nNumParties;j++){
			vector<int> v_hamm;
			v_hamm.resize(m_ndistBits,1);

			if(i!=j){
				v_hamm = PutHammDistCalculator(i,j,m_nRep);
			}

			ret[j] = v_hamm;
    }
		vector<int> min = FindMinValue(ret);
		vector<vector <int> >().swap(ret);
		min = Reorder(min);

		allOutput.push_back(min);
		vector<int>().swap(min);
  }

  return allOutput;
}

//Sort small to large
void CHammSorting2::Sorting(vector< vector<int> > party, vector< vector<int> > value){
//  cout << "Start sorting " << endl;

	int vectorSize = party.size();
	//vector<int> debug;
  vector< vector<int> > v_listComp;

	value = Reorder(value);
	value = adjustData(value,1);


	for(int front_index=0;front_index<vectorSize;front_index++){
		for(int back_index=(vectorSize-1);back_index>front_index;back_index--){
		//	cout << "front " << front_index << " back " << back_index <<endl;
			vector<int> v_a = ReverseBit(value[back_index-1]);
			vector<int> v_b = ReverseBit(value[back_index]);
			vector<int> p_a = party[back_index-1];
			vector<int> p_b = party[back_index];

			short int swapBit = PutNBitsComparator(v_a,v_b);
			//debug.push_back(swapBit);

			/*if((front_index == 0 )&&(back_index==3)){
				int swapBit2 = PutNBitsComparator(value[back_index-1],value[back_index]);
				vector<int> t;
				int ret = PutGTGate(value[back_index-1][0],value[back_index][0],value[back_index].size());
				int ret2 = PutGTGate(value[back_index][0],value[back_index-1][0],value[back_index].size());
				//int ret = PutGTGate(0,1,1);
				//int ret2 = PutGTGate(1,0,1);
				t.push_back(ret);
				t.push_back(ret2);
				//m_vOutputStart.resize(m_nNumParties,value[back_index].front());
				//m_vOutputEnd.resize(m_nNumParties, value[back_index].back());
				Debug(t);
			}*/


			vector< vector<int> > v_swapper = PutNBitsSwapper(swapBit,v_a,v_b);
			value[back_index] = ReverseBit(v_swapper[0]);
			value[back_index-1] = ReverseBit(v_swapper[1]);
			//vector<vector <int> >().swap(v_swapper);

			vector< vector<int> > p_swapper = PutNBitsSwapper(swapBit,p_a,p_b);
			party[back_index] = p_swapper[0];
			party[back_index-1] = p_swapper[1];
			//vector<vector <int> >().swap(p_swapper);

		}


	}
	//Debug(value);
	//cout << "End Sorting" << endl;

	ReorderOutput(party,value);
	vector<vector <int> >().swap(party);
	vector<vector <int> >().swap(value);
}

void CHammSorting2::ReorderOutput(vector< vector<int> > v){
	vector<int> newV;

	for(int i=0;i<v.size();i++){
		for(int j=0;j<v[i].size();j++){
			newV.push_back(PutXORGate(v[i][j],0));
		}
	}

	m_vOutputStart.resize(m_nNumParties, newV.front());
	m_vOutputEnd.resize(m_nNumParties, newV.back());

	vector<int>().swap(newV);
}

void CHammSorting2::ReorderOutput(vector< vector<int> > v1, vector< vector<int> > v2){
  vector<int> output;
	int outputCount = v1.size() - m_nRefParties;

	//PrintInt("outputCount",outputCount);

  for(int i=0;i<outputCount;i++){
    for(int j=0;j<v1[i].size();j++){
      int gate = v1[i][j];
      output.push_back(PutXORGate(gate,0));
    }
  }

  for(int i=0;i<outputCount;i++){
    for(int j=0;j<v2[i].size();j++){
      int gate = v2[i][j];
      output.push_back(PutXORGate(gate,0));
    }
  }

  m_vOutputStart.resize(m_nNumParties, output.front());
  m_vOutputEnd.resize(m_nNumParties, output.back());

	p_lastGateId = output.back();

	vector<int>().swap(output);
  //PrintVector("m_vOutputStart",m_vOutputStart);
  //PrintVector("m_vOutputEnd",m_vOutputEnd);
}

vector<int> CHammSorting2::ReverseBit(vector<int> v){
	vector<int> out;

	for(int i=v.size()-1; i>=0 ;i--){
		out.push_back(PutXORGate(v[i],0));
	}

	return out;
}

vector<int> CHammSorting2::Reorder(vector<int> v){
	vector<int> newV;

	for(int j=0;j<v.size();j++){
		newV.push_back(PutXORGate(v[j],0));
	}
	return newV;
}

vector< vector<int> > CHammSorting2::Reorder(vector< vector<int> > v){
	vector< vector<int> > newV;

	for(int i=0;i<v.size();i++){
		vector<int> tmp;
		for(int j=0;j<v[i].size();j++){
			tmp.push_back(PutXORGate(v[i][j],0));
		}
		newV.push_back(tmp);
	}

	return newV;
}

vector< vector<int> > CHammSorting2::adjustData(vector< vector <int> > data, int referValue){

	vector< vector<int> > v_listDistance;
	v_listDistance.resize(m_nNumParties);

	v_listRefer.resize(m_nNumParties);
	vector<int> value;
	value.resize(m_nNumParties);

//	cout << "Adjust Data" << endl;
	//Change input of referred parties to all 1.
	for(int i=0;i<m_nNumParties;i++){
		v_listRefer[i] = PutXORGate(m_vInputStart[i],referValue);

		value[i] = PutMUXGate(data[i][0],m_nForOne,v_listRefer[i],m_ndistBits,false);

	}

	//Reorder gates
	vector<int> temp;
	for(int i=0;i<m_nNumParties;i++){
		temp.resize(m_ndistBits);
		for(int j=0;j<m_ndistBits;j++){
			temp[j] = (value[i]+j);

		}
		v_listDistance[i] = temp;
	}
	vector<int>().swap(temp);

	return v_listDistance;
}

vector<int> CHammSorting2::FindMinValue(vector<vector<int> > listValue){

	int vectorSize = listValue.size();

	//vector<int> de;

	listValue = Reorder(listValue);
	listValue = adjustData(listValue, 0);
//	cout << "Start Find Min " << vectorSize << endl;

	//Sorting

	for(int front_index=0;front_index<vectorSize;front_index++){
		for(int back_index=(vectorSize-1);back_index>front_index;back_index--){
	//		cout << "Round " << front_index << " " << back_index << endl;
			vector<int> a = ReverseBit(listValue[back_index-1]);
			vector<int> b = ReverseBit(listValue[back_index]);

			short int swapBit = PutNBitsComparator(a,b);

			//de.push_back(swapBit);

			/*if((front_index==0) && (back_index==1)){
				vector< vector<int> > v_swapper = PutNBitsSwapper(swapBit,listValue[back_index-1],listValue[back_index]);

				Debug(v_swapper[1]);
			}*/
			vector< vector<int> > v_swapper = PutNBitsSwapper(swapBit,a,b);
			listValue[back_index] = ReverseBit(v_swapper[0]);
			listValue[back_index-1] = ReverseBit(v_swapper[1]);

			vector<int>().swap(a);
			vector<int>().swap(b);
			vector<vector <int> >().swap(v_swapper);
			//vector<vector <int> >().swap(v_swapper);
		}
	}





	return listValue[0];
}
//End computations functions



//Components
void CHammSorting2::InitParameters(int partiesCount, vector<int> params){
	//a = partiesCount;
  m_nNumParties = partiesCount;
  m_nRep = params[0];
  m_nRefParties = params[1];
  m_nSortedParties = m_nNumParties - m_nRefParties;


	m_nPartyLength = GetNumBits(m_nNumParties); // -1 in ()
  m_ndistBits = GetNumBits(m_nRep); //+1 out()
  totalInputBits = m_nPartyLength + m_nRep + 1; //Plus 1 for 1st bit (referred bit)
  totalOutputBits = (m_ndistBits + m_nPartyLength) * m_nSortedParties;
	m_vNumVBits.resize(m_nNumParties, 1);
	m_vInputStart.resize(m_nNumParties);
	m_vInputEnd.resize(m_nNumParties);
  m_nFrontier = 2; //Input Gate starts at 2, 0 and 1 are allocated for 0,1 bit

	//Debug
	p_nRefParties = m_nRefParties;
	p_ndistBits = m_ndistBits;
	p_nPartyLength= m_nPartyLength;
	p_nSortedParties = m_nSortedParties;
	//



  PrintInt("Input",m_nRep);
  PrintInt("m_nNumParties",m_nNumParties);
  PrintInt("m_nRefParties",m_nRefParties);
  PrintInt("m_nSortedParties", m_nSortedParties);
  PrintInt("m_nPartyLength",m_nPartyLength);
  PrintInt("m_ndistBits",m_ndistBits);
  PrintInt("totalInputBits",totalInputBits);
  PrintInt("totalOutputBits",totalOutputBits);


  //Count number of gates according to number of inputs.
  v_listPartiesStart.resize(m_nNumParties);
  v_listParties.resize(m_nNumParties);
	v_listRefParties.resize(m_nNumParties);

  for (int i = 0; i < m_nNumParties; i++)
  {
    m_vInputStart[i] = m_nFrontier;
    m_nFrontier = m_nFrontier+(m_nRep+m_nPartyLength)+1;
    m_vInputEnd[i] = m_nFrontier - 1;

    //if(i!=0) v_listPartiesStart[i-1] = (m_vInputEnd[i] - m_nPartyLength)+1;
		v_listPartiesStart[i] = (m_vInputEnd[i] - m_nPartyLength)+1;

    v_listRefParties[i] = m_nFrontier;
  }

  //Put party id gate to vector
  for(int i=0;i<(m_nNumParties);i++)
  {
    for(int j=0;j<m_nPartyLength;j++){
      int g = v_listPartiesStart[i]+j;
      v_listParties[i].push_back(g);

    }
    //PrintVector("v_listParties", v_listParties[i]);
  }

  //m_othStartGate = 1-st non-input gate
  m_othStartGate = m_nFrontier;

  //Component Estimation
  hammCalculatorCount = m_nRefParties*m_nSortedParties*m_nRep;
  counterCount = hammCalculatorCount;
  comparatorCount = (hammCalculatorCount * (hammCalculatorCount - 1))/2;
  swapperCount = comparatorCount*2;

  nonInputGates = CalculateNonInputGates();
	m_nNumGates =  nonInputGates + m_nFrontier;

  PrintInt("nonInputGates",nonInputGates);
	PrintInt("m_nNumGates",m_nNumGates);
	PrintInt("m_othStartGate",m_othStartGate);

	int diff = 2147483648 - m_nNumGates;
	PrintInt("diff",diff);


//  cout << "Finish Initial Params " << endl;
}

vector<int> CHammSorting2::PutHammDistCalculator(int partyId1, int partyId2, int inputSize){
  vector<int> ret;
  vector<int> hammDistBits;
  int noAdder = m_ndistBits;
  int start_1 = m_vInputStart[partyId1]+1;
  int start_2 = m_vInputStart[partyId2]+1;

  //Calculate Hamming distance
  for(int i=0;i<inputSize;i++)
  {
    int t = PutXORGate(start_1 + i,start_2 + i);
    hammDistBits.push_back(t);
  }
  //cout << "end " << inputSize << endl;

  ret = SumAllBits(hammDistBits);
	vector<int>().swap(hammDistBits);
  return ret;
}

vector<int> CHammSorting2::SumAllBits(vector<int> v_bits){
  int vectorSize = v_bits.size();
  int bitNum = GetNumBits(vectorSize);

  for (int i = vectorSize; i < pow(2, bitNum); i++){
    v_bits.push_back(0);
    //cout << "[b] add" << i << endl;
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
		int a = PutXORGate(v_bits[0]+i,0);
    ret.push_back(a);
  }
  return ret;
}

int CHammSorting2::PutNBitsComparator(vector<int> v_a, vector<int> v_b){
  int ret = PutGTGate(v_a[0],v_b[0],v_a.size());
  return ret;
}

vector< vector<int> > CHammSorting2::PutNBitsSwapper(int swapBit, vector<int> v_a, vector<int> v_b){
	//int n= 1;//m_ndistBits
	vector<int> v_outA;
	vector<int> v_outB;
	vector< vector<int> > ret;



	for(int i=0;i<v_a.size();i++){
		vector<int> v = PutBitSwapper(swapBit, v_a[i],v_b[i]);
		v_outA.push_back(v[0]); //a
    v_outB.push_back(v[1]); //b
	}

  ret.push_back(v_outA);
  ret.push_back(v_outB);
	ret = Reorder(ret);
  return ret;
}

//End Components

//4 AND gates, 2 ORs gates
vector<int> CHammSorting2::PutBitSwapper(int swapBit, int a, int b){
  vector<int> ret;
	int newA = PutMUXGate(a,b,swapBit, 1,false);
	int newB = PutMUXGate(b,a,swapBit, 1,false);

	ret.push_back(newA);
	ret.push_back(newB);
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

void CHammSorting2::InitGates(){
	m_othStartGate = m_nFrontier;
	m_pGates = new GATE[m_nNumGates];
	//m_nNumXORs = 0;

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

void CHammSorting2::InitLayer(){
	m_nForZero = m_nFrontier;
	for(int i=0;i<m_ndistBits;i++){
		PutXORGate(0,0);
	}

	m_nForOne = m_nFrontier;
	for(int i=0;i<m_ndistBits;i++){
		PutXORGate(1,0);
	}

}

int CHammSorting2::CalculateNonInputGates(){


  int oneComparatorGates = (1*m_ndistBits) + (3*m_ndistBits);

  for(int i=m_ndistBits;i>0;i--)
  {
    oneComparatorGates += i;
  }
	/*int t = pow(3*m_ndistBits,1.5);
  int count = 0;
  count += (hammCalculatorCount * m_nRep);
  count += ((m_ndistBits * m_nRep ) + m_nRep);
  int findMin = (m_ndistBits);
	int a = (2*m_ndistBits) +  m_nNumParties + t;//pow(3*m_ndistBits,2)) ;
	int b = 4*m_ndistBits; //GY
	int c = 2 * m_ndistBits * t;
	count += (a+b+c)*findMin;

	int sorting = (m_nSortedParties);
	int d = m_ndistBits;
	int e = 4 * m_ndistBits;
	int f = 4 * m_ndistBits;
	int g = 2 * m_ndistBits * 2 * t;
	count += (d+e+f+g) * sorting;

	//Reorder
	count += (m_nSortedParties * m_nPartyLength) + (m_nSortedParties * m_ndistBits);
	count += pow(m_ndistBits,m_ndistBits) + 100000;

	//count += (oneComparatorGates * comparatorCount);
  //count += ((11 * m_ndistBits) + (11 * m_nPartyLength)) * swapperCount;
  //count += totalOutputBits;
	//count *= 3;
	//count += 100*pow(m_nNumParties*m_nRep,2);
	//count = (-4763) + 1259*m_nNumParties + 204*m_nRep + 499*m_nRefParties;
	*/
	int count = 0;
	if(m_nNumParties < 4){
		count = (-4763) + 1259*m_nNumParties + 204*m_nRep + 499*m_nRefParties;
		//if(m_nRep <= 10) count = 5000;
	}
	else if((m_nNumParties >= 4 ) && (m_nNumParties < 15)){
		count = -21932.77 + (5712.52*m_nNumParties)
		- (816.36*m_nRep) - (413.15*m_nRefParties)
		+ (246.63*m_nNumParties*m_nRep);
		count = count *2;
		if(m_nRep <= 21 ) count = count *2;
	}else{
		count = 200000 + (5712.52*m_nNumParties)
		- (816.36*m_nRep) - (413.15*m_nRefParties)
		+ (246.63*m_nNumParties*m_nRep);
		count = count *2;
	}
	//count = count *2;
	cout << "est " << count << endl;
	//count = 5000;
	p_totalGates = count;

  return count;
}


//1 Not = 1 Gates
int CHammSorting2::PutNOTGate(int a){
  int x = PutXORGate(a,1);
  return x;
}

//1 OR = 3 Gates
int CHammSorting2::PutORGate(int a, int b){
  int x = PutXORGate(a,b);
  int y = PutANDGate(a,b);
  int z = PutXORGate(x,y);

  return z;
}

int CHammSorting2::PutNANDGate(int a, int b){
  int and_1 = PutANDGate(a,b);
  int ret = PutNOTGate(and_1);

  return ret;
}

int CHammSorting2::PutNAND2Gate(int a, int b, int c){
  int and_1 = PutANDGate(a,b);
  int and_2 = PutANDGate(and_1,c);
  int ret = PutNOTGate(and_2);

  return ret;
}

int CHammSorting2::PutNORGate(int a, int b){
  int or_1 = PutORGate(a,b);
  int ret = PutNOTGate(or_1);

  return ret;
}


int CHammSorting2::GetNumBits(int decimal){
    int num_bits = ceil(log2(decimal));
    if(decimal == 2) num_bits = 2;

    return num_bits;
}

void CHammSorting2::PrintInt(const string& name, int i){
  cout << "[BB] == " << name << " = "<< i << endl;
}

void CHammSorting2::PrintVector(const string& name, const vector<int>& params){
  cout << "== V: " << name << "(" << params.size() << ") :";
  for (vector<int>::const_iterator i = params.begin(); i != params.end(); ++i){
		cout << *i << " ";
	}
  cout << "=====" << endl;
}

void CHammSorting2::Debug(vector<int> v){
  cout << "Debugging" << endl;
  vector<int> t;
  for(int i=0;i<v.size();i++){
    int a = PutXORGate(v[i],0);
    t.push_back(a);
  }

  m_vOutputStart.resize(m_nNumParties,t.front());
  m_vOutputEnd.resize(m_nNumParties,t.back());

  //PrintVector("m_vOutputStart",m_vOutputStart);
  //PrintVector("m_vOutputEnd",m_vOutputEnd);


}

void CHammSorting2::Debug(vector<vector<int> > v){
	vector<int> out;
	//PrintVector("v",v[0]);
	//PrintVector("v",v[1]);
	//PrintVector("v",v[2]);
	//PrintVector("v",v[3]);

	for(int i=0;i<v.size();i++){
		for(int j=0;j<v[i].size();j++){
			int a = PutXORGate(v[i][j],0);
			out.push_back(a);
		}
	}

	m_vOutputStart.resize(m_nNumParties,out.front());
	m_vOutputEnd.resize(m_nNumParties,out.back());
	//m_vOutputStart.resize(m_nNumParties,v[0].front());
	//am_vOutputEnd.resize(m_nNumParties,v[0].back());


	//PrintVector("m_vOutputStart",m_vOutputStart);
	//PrintVector("m_vOutputEnd",m_vOutputEnd);

}
