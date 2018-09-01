//  grouping.cpp

#include "grouping.h"
#include <cassert>
using namespace std;

BOOL CGrouping::Create(int nParties, const vector<int>& vParams)
{
    m_nNumParties = nParties;

	// parse params
	if( vParams.size() < (unsigned) 1 )
	{
		cout << "Error! This circuit needs " << 1  
			 << "parameters: # of Items of each party"
			 << endl;
		return FALSE;
	}

	m_nItems = vParams[0];
	m_vNumVBits.resize(m_nNumParties, 1);
	
	//================================================================	
	// gates for inputs
	m_vInputStart.resize(nParties);
	m_vInputEnd.resize(nParties);

	m_nFrontier=2;
	for(int i=0; i<nParties; i++)
	{		
		m_vInputStart[i] = m_nFrontier;
		for(int j=0; j<m_nItems;j++,m_nFrontier++){}
		m_vInputEnd[i] = m_nFrontier - 1;
	}
	//=========================================================================
	//computes gates roughly
	int gates_or=(m_nNumParties-1)*m_nItems;
	int gates_input = 2+m_nNumParties*m_nItems; 
	gates_noninput = 3*gates_or+m_nItems;
	 	
	InitGates();
	//==================================================================
	//Construct circuit
	m_vLayerOutput.resize(m_nItems,0);
	for(int i=0;i<m_nItems;i++)
	{	
		int temp_id=m_vInputStart[0]+i;
		for(int j=1;j<m_nNumParties;j++)
		{
			int k=m_vInputStart[j]+i;
			temp_id=PutORGate(temp_id,k);
		}
		m_vLayerOutput[i]=temp_id;
	}
	//==================================================================
	//Gates for outputs
	PutOuputLayer();	
	PatchParentFields();
	return TRUE;
}

// gets the OR output of two binary input
// input: the gate id of two inputs
// output: gate id of output
int CGrouping::PutORGate(int a, int b)
{
    int axb=PutXORGate(a,b);
    int anb=PutANDGate(a,b);
    int out=PutXORGate(axb,anb);
    return out;

}

void CGrouping::InitGates()
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
void CGrouping::PutOuputLayer()
{
	int o_start = m_nFrontier;
	for(int i=0;i<m_nItems;i++)
	{	
		PutXORGate(m_vLayerOutput[i],0);
	}
	int o_end = m_nFrontier -1;
	m_vOutputStart.resize(m_nNumParties, o_start);
	m_vOutputEnd.resize(m_nNumParties, o_end);
	m_nNumGates = m_nFrontier;
}
