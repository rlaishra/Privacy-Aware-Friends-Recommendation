//  hammdist.cpp

#include "hammdist.h"
#include <cassert>
#include <cmath> 
using namespace std;

// creates the binary circuit for computing hamming distance
// input: the number of parties
//        the number of values of each party
// output: bool

BOOL CHammDist::Create(int nParties, const vector<int>& vParams)
{
	m_nNumParties=nParties;
	if( vParams.size() < 1 )
		{
			cout << "Error! This circuit needs one parameter: #items." << endl;
			return FALSE;
		}
	m_nItems=vParams[0];           // # of items of each party
	m_nServers=vParams[1];
	m_nTopK=vParams[2];
	m_nConsumers=m_nNumParties-m_nServers;
	m_vNumVBits.resize(m_nNumParties, 1);	// # of bits of each party's value
	m_nRep=GetNumBits(m_nItems);
	IDBits=GetNumBits(m_nNumParties);

	//======================================================================    
	//Gates for input
	m_vInputStart.resize(m_nNumParties);
	m_vInputEnd.resize(m_nNumParties);
    
	m_nFrontier=2;
	for (int i=0; i<m_nNumParties; i++) 
	{
		m_vInputStart[i]=m_nFrontier;
        	for (int j=0; j<m_nItems+1; j++, m_nFrontier++) {}
		m_vInputEnd[i]=m_nFrontier-1;
	}
	//======================================================================
	// Computes roughly # of gates
	int num_gatesinput=2+m_nNumParties*m_nItems;

	int num_eachaddgate_ID=6*IDBits;
	int num_gate_generateID=2*IDBits+(m_nNumParties-1)*num_eachaddgate_ID;
	
	int num_eachGTgate=4*1;
	int num_eachMUXgate_index=3*1;
	int num_eachMUXgate_value=3*m_nItems;
	int num_eachMUXgate_ID=3*IDBits;
	int num_gate_getserverinput=(num_eachGTgate+1+2*num_eachMUXgate_index+2*num_eachMUXgate_value)*m_nServers*m_nConsumers;
	int num_gate_getconsumerinput=(num_eachGTgate+1+2*(num_eachMUXgate_index+num_eachMUXgate_value+num_eachMUXgate_ID))*m_nServers*m_nConsumers;

	int num_addgates_hamm=(m_nItems-1)*m_nConsumers*m_nServers;	//# of ADD gates in the circuit
	int num_eachaddgate_hamm=6*m_nRep;	// # of element gates in one ADD gate roughly
	int num_xorgates_hamm=m_nServers*m_nItems*m_nConsumers*m_nRep;
	int num_gate_hammlayer=num_xorgates_hamm+num_addgates_hamm*num_eachaddgate_hamm;

	int num_addgates_add=(m_nServers-1)*m_nConsumers;
	int num_eachaddgate_add=6*OutputBits;
	int num_xorgates_add=OutputBits*m_nServers*m_nConsumers;
	int num_gate_addlayer=num_xorgates_add+num_addgates_add*num_eachaddgate_add;

	int num_eachGEgate=5*OutputBits;
	int num_eachMUXgate_compare=3*OutputBits;
	int num_eachMUXgate_original=3*m_nItems;
	int num_gate_topklayer=(num_eachGEgate+2*num_eachMUXgate_compare+2*num_eachMUXgate_original)*m_nTopK*(m_nConsumers-m_nTopK);

	int num_gatesoutput=m_nItems*m_nConsumers;
	int num_gatesother=num_gate_generateID+num_gate_getserverinput+num_gate_getconsumerinput+num_gate_hammlayer+num_gate_addlayer+num_gatesoutput+num_gate_topklayer;
	gates_noninput=num_gatesother+num_gatesoutput+3000;
  
	InitGates();
    //==============================================================
    // Construct circuit

	GenerateID();	//ID for each party from 0 to m_nNumParties-1
	
	GetIndex();	//First bit of each party

	GetServersInput();

	GetConsumersInput();

	m_vAddLayerOutputStart.resize(m_nConsumers,1);
	m_vAddLayerOutputEnd.resize(m_nConsumers,0);

	for(int t=0;t<m_nConsumers;t++)
	{
		PutHammLayer(t);
		PutAddLayer();
	
		m_vAddLayerOutputStart[t]=temp_id;
		m_vAddLayerOutputEnd[t]=m_nFrontier-1; 
	}

	PutTopKLayer();

	
    
    //================================================================
    //Gates for output
    PutOutputLayer();
    PatchParentFields();
    
    return TRUE;
}


// gets # of bits for a decimal number
// input: decimal number
// output: # of bits for the input
// note: input should not be a negative int
int CHammDist::GetNumBits(int decimal)
{
    int num_bits = 1;
    int tmp;
    
    if (decimal < 0){
        puts("CP2PCircuit::GetNumBits(int): negative input");
        exit(-1);
    }
    else if(decimal == 0){
        return 0;
    }
    else if(decimal == 1){
        return 1;
    }
    
    tmp = decimal;
    while(1==1){
        num_bits++;
        tmp = tmp/2;
        if (tmp <= 1){
            break;
        }
    }
    
    return num_bits;
}

void CHammDist::InitGates()
{
	m_othStartGate = m_nFrontier;
	m_nNumGates = m_nFrontier + gates_noninput;
	m_pGates = new GATE[m_nNumGates];
	m_nNumXORs = 0;

	GATE* gate;

	for(int i=0; i<m_othStartGate; i++)
	{
		gate = m_pGates + i;
		gate->p_ids = 0;
		gate->p_num = 0;
		gate->left = -1;
		gate->right = -1;
		gate->type = 0;
	}	
}

void CHammDist::GetServersInput()
{
	vector<int>	m_vServerIndexStart;
	vector<int>	m_vServerIndexEnd;	
	m_vServerInputStart.resize(m_nServers,1);
	m_vServerInputEnd.resize(m_nServers,0);
	m_vServerIndexStart.resize(m_nServers,1);
	m_vServerIndexEnd.resize(m_nServers,0);
	//m_vServerIDStart.resize(m_nServers,1);
	//m_vServerIDEnd.resize(m_nServers,0);
	for(int i=0;i<m_nServers;i++)
	{
		m_vServerInputStart[i]=m_vInputStart[i]+1;
		m_vServerInputEnd[i]=m_vInputEnd[i];
	}
	for(int i=0;i<m_nServers;i++)
	{
		m_vServerIndexStart[i]=m_vIndexInputStart[i];
		m_vServerIndexEnd[i]=m_vIndexInputStart[i];
	}
	/*for(int i=0;i<m_nServers;i++)
	{
		m_vServerIDStart[i]=m_vIDStart[i];
		m_vServerIDEnd[i]=m_vIDEnd[i];
	}*/
	

	for(int i=m_nServers;i<m_nNumParties;i++)
	{
		int index_a=m_vIndexInputStart[i];
		int value_a=m_vInputStart[i];
		//int id_a=m_vIDStart[i];
		for (int j=0;j<m_nServers;j++)
		{
			int index_b=m_vServerIndexStart[j];
			int value_b=m_vServerInputStart[j];
			//int id_b=m_vServerIDStart[j];
			int out_GT=PutGTGate(index_a,index_b,1);
			int out_GT_Opposite=PutXORGate(out_GT,1);
			int index_big=PutMUXGate(index_a,index_b,out_GT,1,0);
			int index_small=PutMUXGate(index_a,index_b,out_GT_Opposite,1,0);
			m_vServerIndexStart[j]=index_big;
			m_vServerIndexEnd[j]=index_big;
			index_a=index_small;
			int value_big=PutMUXGate(value_a,value_b,out_GT,m_nItems,0);
			int value_small=PutMUXGate(value_a,value_b,out_GT_Opposite,m_nItems,0);
			m_vServerInputStart[j]=value_big;
			m_vServerInputEnd[j]=value_big+m_nItems-1;
			value_a=value_small;
			/*int id_big=PutMUXGate(id_a,id_b,out_GT,IDBits,0);
			int id_small=PutMUXGate(id_a,id_b,out_GT_Opposite,IDBits,0);
			m_vServerInputStart[j]=id_big;
			m_vServerInputEnd[j]=id_big+IDBits-1;
			id_a=id_small;*/
		}
	}
}

void CHammDist::GetConsumersInput()
{
	vector<int> m_vConsumerIndexStart;
	vector<int> m_vConsumerIndexEnd;
	m_vConsumerInputStart.resize(m_nConsumers,1);
	m_vConsumerInputEnd.resize(m_nConsumers,0);
	m_vConsumerIndexStart.resize(m_nConsumers,1);
	m_vConsumerIndexEnd.resize(m_nConsumers,0);
	m_vConsumerIDStart.resize(m_nConsumers,1);
	m_vConsumerIDEnd.resize(m_nConsumers,0);
	for(int i=0;i<m_nConsumers;i++)
	{
		m_vConsumerInputStart[i]=m_vInputStart[i]+1;
		m_vConsumerInputEnd[i]=m_vInputEnd[i];
	}
	for(int i=0;i<m_nConsumers;i++)
	{
		m_vConsumerIndexStart[i]=m_vIndexInputStart[i];
		m_vConsumerIndexEnd[i]=m_vIndexInputStart[i];
	}
	for(int i=0;i<m_nConsumers;i++)
	{
		m_vConsumerIDStart[i]=m_vIDStart[i];
		m_vConsumerIDEnd[i]=m_vIDEnd[i];
	}

	for(int i=m_nConsumers;i<m_nNumParties;i++)
	{
		int index_a=m_vIndexInputStart[i];
		int value_a=m_vInputStart[i];
		int id_a=m_vIDStart[i];
		for (int j=0;j<m_nConsumers;j++)
		{
			int index_b=m_vConsumerIndexStart[j];
			int value_b=m_vConsumerInputStart[j];
			int id_b=m_vConsumerIDStart[j];
			int out_GT=PutGTGate(index_a,index_b,1);
			int out_GT_Opposite=PutXORGate(out_GT,1);
			int index_big=PutMUXGate(index_a,index_b,out_GT,1,0);
			int index_small=PutMUXGate(index_a,index_b,out_GT_Opposite,1,0);
			m_vConsumerIndexStart[j]=index_small;
			m_vConsumerIndexEnd[j]=index_small;
			index_a=index_big;
			int value_big=PutMUXGate(value_a,value_b,out_GT,m_nItems,0);
			int value_small=PutMUXGate(value_a,value_b,out_GT_Opposite,m_nItems,0);
			m_vConsumerInputStart[j]=value_small;
			m_vConsumerInputEnd[j]=value_small+m_nItems-1;
			value_a=value_big;
			int id_big=PutMUXGate(id_a,id_b,out_GT,IDBits,0);
			int id_small=PutMUXGate(id_a,id_b,out_GT_Opposite,IDBits,0);
			m_vConsumerIDStart[j]=id_small;
			m_vConsumerIDEnd[j]=id_small+IDBits-1;
			id_a=id_big;
		}
	}
}

void CHammDist::PutOutputLayer()
{
	m_vOutputStart.resize(m_nNumParties, 1);
	m_vOutputEnd.resize(m_nNumParties, 0);
	/*m_vOutputStart[0]=m_vTopKOriginalStart[0];
	m_vOutputEnd[0]=m_vTopKOriginalEnd[0];
	m_vOutputStart[1]=m_vTopKCompareStart[0];
	m_vOutputEnd[1]=m_vTopKCompareEnd[0];
	m_vOutputStart[0]=m_vTopKIDStart[0];
	m_vOutputEnd[0]=m_vTopKIDEnd[0];
	m_vOutputStart[1]=m_vTopKIDStart[1];
	m_vOutputEnd[1]=m_vTopKIDEnd[1];	
	for(int i=0;i<m_nConsumers;i++)
	{
		m_vOutputStart[i+m_nServers]=m_vAddLayerOutputStart[i];
		m_vOutputEnd[i+m_nServers]=m_vAddLayerOutputEnd[i];	
	}*/
	for (int i=0;i<m_nTopK;i++)
	{
		m_vOutputStart[m_nNumParties-1-i]=m_vTopKIDStart[i];
		m_vOutputEnd[m_nNumParties-1-i]=m_vTopKIDEnd[i];	
	}
	
	m_nNumGates = m_nFrontier;
}

void CHammDist::PutHammLayer(int consumer_id)
{
	vector<int> comp_id;	
	m_vHammOutputStart.resize(m_nServers,1);
	m_vHammOutputEnd.resize(m_nServers,0);
	for (int i=0; i<m_nServers; i++) {
		int k=m_vConsumerInputStart[consumer_id];          //consumer input
		comp_id.resize(m_nItems,0);
		int p=0;
        	for (int j=m_vServerInputStart[i]; j<=m_vServerInputEnd[i]; j++,k++,p++) {
            		comp_id[p]=PutXORGate(j,k);
            		for (int m=1; m<m_nRep; m++) {
                		PutXORGate(0,0);
            		}
        	}
		int gateid=comp_id[0];
		for (int m=1; m<m_nItems; m++){
			gateid=PutAddGate(gateid,comp_id[m],m_nRep);
		}
		m_vHammOutputStart[i]=gateid;
		m_vHammOutputEnd[i]=m_nFrontier-1;
	}
}

void CHammDist::PutAddLayer()
{
	OutputBits=GetNumBits(m_nItems*m_nServers);
	m_vAddInputStart.resize(m_nServers,1);
	m_vAddInputEnd.resize(m_nServers,0);
	for (int i=0;i<m_nServers;i++)
	{
		m_vAddInputStart[i]=m_nFrontier;
		for (int j=m_vHammOutputStart[i];j<=m_vHammOutputEnd[i];j++)
		{
			PutXORGate(j,0);
		}
		for (int k=m_nRep;k<OutputBits;k++)
		{
			PutXORGate(0,0);
		}
		m_vAddInputEnd[i]=m_nFrontier-1;
	} 
	temp_id=m_vAddInputStart[0];
	for (int i=1;i<m_nServers;i++)
	{
		temp_id=PutAddGate(temp_id,m_vAddInputStart[i],OutputBits);
	}
}

void CHammDist::GenerateID()
{
	m_vIDStart.resize(m_nNumParties,1);
	m_vIDEnd.resize(m_nNumParties,0);
	int ID_Zero=m_nFrontier;
	for(int i=0;i<IDBits;i++)
	{
		PutXORGate(0,0);
	}
	int ID_One=m_nFrontier;
	PutXORGate(0,1);
	for(int i=0;i<IDBits-1;i++)
	{
		PutXORGate(0,0);	
	}
	m_vIDStart[0]=ID_Zero;
	m_vIDEnd[0]=ID_Zero+IDBits-1;
	for (int i=1;i<m_nNumParties;i++)
	{
		m_vIDStart[i]=PutAddGate(m_vIDStart[i-1],ID_One,IDBits);
		m_vIDEnd[i]=m_vIDStart[i]+IDBits-1;		
	}
}

void CHammDist::GetIndex()
{
	m_vIndexInputStart.resize(m_nNumParties,1);
	m_vIndexInputEnd.resize(m_nNumParties,0);
	for(int i=0;i<m_nNumParties;i++)
	{
		m_vIndexInputStart[i]=m_vInputStart[i];
		m_vIndexInputEnd[i]=m_vInputStart[i];
	}
}

void CHammDist::PutTopKLayer()
{
	m_vTopKCompareStart.resize(m_nTopK,1);
	m_vTopKCompareEnd.resize(m_nTopK,0);
	m_vTopKOriginalStart.resize(m_nTopK,1);
	m_vTopKOriginalEnd.resize(m_nTopK,0);
	m_vTopKIDStart.resize(m_nTopK,1);
	m_vTopKIDEnd.resize(m_nTopK,0);    
	for(int i=0;i<m_nTopK;i++)
	{
		m_vTopKCompareStart[i]=m_vAddLayerOutputStart[i];
		m_vTopKCompareEnd[i]=m_vAddLayerOutputEnd[i];
		m_vTopKOriginalStart[i]=m_vConsumerInputStart[i];
		m_vTopKOriginalEnd[i]=m_vConsumerInputEnd[i];
		m_vTopKIDStart[i]=m_vConsumerIDStart[i];
		m_vTopKIDEnd[i]=m_vConsumerIDEnd[i];
	}	
	for (int i=m_nTopK; i<m_nConsumers; i++) 
	{
        	int compare_InputA=m_vAddLayerOutputStart[i];
		int original_InputA=m_vConsumerInputStart[i];
		int ID_InputA=m_vConsumerIDStart[i];
        	for (int j=0; j<m_nTopK; j++) 
		{
			int compare_InputB=m_vTopKCompareStart[j];
	    		int original_InputB=m_vTopKOriginalStart[j];
			int ID_InputB=m_vTopKIDStart[j];
            		int out_GE=PutGEGate(compare_InputA,compare_InputB,OutputBits);
			int out_GE_Opposite=PutXORGate(out_GE,1);
            		int compare_Big=PutMUXGate(compare_InputA,compare_InputB,out_GE,OutputBits,0);
			int compare_Small=PutMUXGate(compare_InputA,compare_InputB,out_GE_Opposite,OutputBits,0);
			m_vTopKCompareStart[j]=compare_Big;
			m_vTopKCompareEnd[j]=compare_Big+OutputBits-1;
			compare_InputA=compare_Small;
			int original_Big=PutMUXGate(original_InputA,original_InputB,out_GE,m_nItems,0);	    				
			int original_Small=PutMUXGate(original_InputA,original_InputB,out_GE_Opposite,m_nItems,0);
			m_vTopKOriginalStart[j]=original_Big;
			m_vTopKOriginalEnd[j]=original_Big+m_nItems-1;
			original_InputA=original_Small;	
			int ID_Big=PutMUXGate(ID_InputA,ID_InputB,out_GE,IDBits,0);
			int ID_Small=PutMUXGate(ID_InputA,ID_InputB,out_GE_Opposite,IDBits,0);
			m_vTopKIDStart[j]=ID_Big;
			m_vTopKIDEnd[j]=ID_Big+IDBits-1;
			ID_InputA=ID_Small;
        	}
	}
}













