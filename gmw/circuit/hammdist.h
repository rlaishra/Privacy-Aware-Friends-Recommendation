//  hammdist.h

#ifndef __HAMM_DIST__BY_KLOU_
#define __HAMM_DIST__BY_KLOU_

#include "circuit.h"
#include <cassert>

class CHammDist:public CCircuit
{
public:
    BOOL Create(int nParties, const vector<int>& vParams);
public:
    int	GetNumBits(int decimal);
    void InitGates();
    void PutOutputLayer();
    void PutAddLayer();
    void PutHammLayer(int consumer_id);
    void PutTopKLayer();
    void GenerateID();
    void GetIndex();
    void GetServersInput();
    void GetConsumersInput();
private:
    int	m_nRep;         // # of bits for hamming distance
    int	m_nItems;
    int gates_noninput;
    int m_nServers;
    int m_nConsumers;
    int temp_id;
    int OutputBits;
    int m_nTopK;
    int IDBits;
    	
    vector<int>	m_vIndexInputStart;
    vector<int>	m_vIndexInputEnd;
    vector<int>	m_vServerInputStart;
    vector<int>	m_vServerInputEnd;
    vector<int> m_vConsumerInputStart;
    vector<int> m_vConsumerInputEnd;
    vector<int> m_vConsumerIDStart;
    vector<int> m_vConsumerIDEnd;
    vector<int> m_vAddLayerOutputStart;
    vector<int> m_vAddLayerOutputEnd;
    vector<int> m_vHammOutputStart;
    vector<int> m_vHammOutputEnd;
    vector<int> m_vAddInputStart;
    vector<int> m_vAddInputEnd;
    vector<int> m_vTopKCompareStart;
    vector<int> m_vTopKCompareEnd;
    vector<int> m_vTopKOriginalStart;
    vector<int> m_vTopKOriginalEnd;
    vector<int> m_vIDStart;
    vector<int> m_vIDEnd;
    vector<int> m_vTopKIDStart;
    vector<int> m_vTopKIDEnd;
};



#endif //__HAMM_DIST__BY_KLOU_
