#ifndef MERGEEVENT_H 
#define MERGEEVENT_H

#include "GaudiAlg/GaudiAlgorithm.h"
#include "Kernel/DaVinciAlgorithm.h"
#include "Kernel/IParticle2MCAssociator.h"

class MergeEvent : public DaVinciAlgorithm {
public: 
  MergeEvent(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~MergeEvent();

  virtual StatusCode initialize();
  virtual StatusCode execute   ();
  virtual StatusCode finalize  ();

protected:

private:

  IParticle2MCAssociator* m_assoc;

};


#endif // CHECKTRACKS_H
