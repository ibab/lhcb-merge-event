#ifndef MERGEEVENT_H 
#define MERGEEVENT_H

#include "GaudiAlg/GaudiAlgorithm.h"
#include "Kernel/DaVinciAlgorithm.h"
#include "Kernel/IParticle2MCAssociator.h"
#include "MicroDST/ICloneMCParticle.h"

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
  ICloneMCParticle* m_mcCloner;
  std::string m_prefix;
};


#endif // CHECKTRACKS_H
