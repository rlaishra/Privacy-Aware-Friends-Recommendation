OT setup
---

```
SetupOTExtension()
  |
  +-->WakeupWorkerThreads(e_OTExt)
           |
           +--> PutJob()
                  ...
                   |
                   +---> 
                        if( AsOTSender(nPID, m_nPartnerID )){
                                //cout << nPID << " SEND TO " << m_nPartnerID << endl;
                                bSuccess = party->ThreadRunIKNPSndFirst(m_nPartnerID);
                                //cout << nPID << " SEND TO " << m_nPartnerID << " DONE!!!!" << endl;
                        }

                        else{
                                //cout << nPID << " RECEIVE FROM " << m_nPartnerID << endl;
                                bSuccess = party->ThreadRunIKNPRcvFirst(m_nPartnerID);
                                //cout << nPID << " RECEIVE FROM " << m_nPartnerID << " DONE!!!!" <<endl;
                        }


``` 


OT per gate (in evaluation
---

```
                case e_Mult:
                        //cout << "e_Mult " << endl;
                        if( AsOTSender(nPID, m_nPartnerID )) bSuccess = party->ThreadRunMultiplySender(m_nPartnerID);
                        else bSuccess = party->ThreadRunMultiplyReceiver(m_nPartnerID);

``` 
