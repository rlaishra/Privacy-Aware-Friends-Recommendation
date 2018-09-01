// main.cpp
// Modified by Haoyi Shi
#include "../util/typedefs.h"
#include "../util/config.h"
#include "../circuit/circuit.h"

#include "party.h"
#include <ctime>
using namespace std;

void PrintOutput(const vector<int>& vOutput)
{
	cout << "output:" << endl;
	cout << "(binary)";
	for( UINT i=0; i<vOutput.size(); i++ )
	{
		cout << " " << (int) vOutput[i];
	}
	cout << endl;

	ZZ out;
	out.SetSize(vOutput.size());

	cout << "(numeric:big-endian) ";

	int size = vOutput.size();
	for( int i=0; i<size; i++ )
	{
		if( bit(out,i) != vOutput[i] )
			SwitchBit(out,i);
	}
	cout << out << endl;


	cout << "(numeric:little-endian) ";

	for( int i=0; i<size; i++ )
	{
		if( bit(out,i) != vOutput[size-i-1] )
			SwitchBit(out,i);
	}
	cout << out << endl;
}


void PrintOutput(CParty* pParty)
{
	const vector<int>& vOutput =pParty->GetOutput();


	cout << "output:" << endl;

	cout << "# of parties " << pParty->m_nNumParties << endl;
	cout << "# of ref parties " << pParty->m_pCircuit->p_nRefParties << endl;
	cout << "# of dist bits " << pParty->m_pCircuit->p_ndistBits << endl;
	cout << "# of parties bits " << pParty->m_pCircuit->p_nPartyLength << endl;
	cout << "# of sorted parties " << pParty->m_pCircuit->p_nSortedParties << endl;

	cout << "***********************************************" << endl;

	cout << "(binary)" << endl;
	cout << " Parties " << endl;

	int partyBitsCount = 0;
	int valueBitsCount = 0 ;
	int partyCount=pParty->m_pCircuit->p_nSortedParties * pParty->m_pCircuit->p_nPartyLength;
	int counter=0;
	int counter2=0;
	int count2 = 0;
	vector<int> tmp;
	for( UINT i=0; i<vOutput.size(); i++ )
	{
		if(i < partyCount){
			cout << " " << (int) vOutput[i];
			partyBitsCount++;
			tmp.push_back(vOutput[i]);
			if(partyBitsCount == pParty->m_pCircuit->p_nPartyLength){


				int ret = 0;
				int size = tmp.size();

				for(int i=0;i<size;i++){
					int bit =  tmp[size-i-1];

					if(bit != 0){
						ret += pow(2,i);
					}
				}

				cout << " -- ("<< ret << ")" ;

				cout << endl;
				partyBitsCount = 0;
				tmp.clear();
			}
			counter ++;
		}else if(i >= partyCount){
			if(counter2 == 0){
				cout << "\n Values "<< endl;
			}
			cout << " " << (int) vOutput[i];
			counter2 ++;
			tmp.push_back(vOutput[i]);
			valueBitsCount++;
			if(valueBitsCount == pParty->m_pCircuit->p_ndistBits){
				int ret = 0;
				int size = tmp.size();

				for(int i=0;i<size;i++){
					int bit =  tmp[size-i-1];

					if(bit != 0){
						ret += pow(2,i);
					}
				}

				cout << " -- ("<< ret << ")" ;

				cout << endl;
				valueBitsCount = 0;
				tmp.clear();
			}
		}

	}
	cout << endl;

	cout << "Total Gate: " << pParty->m_pCircuit->p_lastGateId << endl;
	cout << "Estimate Gates: " << pParty->m_pCircuit->p_totalGates << endl;

}


int main(int argc, char** argv)
{
	if( argc != 2 )
	{
		cout << "usage: mpc.exe config_file " << endl;
		return 0;
	}

	double tt = clock();

	CConfig* pConfig = new CConfig();
	if(!pConfig->Load(argv[1]))
	{
		cout << "failure in opening the config file: " << argv[1] << endl;
		return 0;
	}

	CParty* pParty = new CParty();
	pParty->Run();
	//PrintOutput( pParty->GetOutput() );
	PrintOutput(pParty);

	delete pParty;

	double tt1 = clock();
	cout << endl << "elapsed " <<  (tt1-tt)/CLOCKS_PER_SEC << " seconds." << endl;


	return 0;
}
