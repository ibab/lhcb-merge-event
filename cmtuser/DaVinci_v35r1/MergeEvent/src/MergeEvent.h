#ifndef MERGEEVENT_H 
#define MERGEEVENT_H

#include "GaudiAlg/GaudiAlgorithm.h"
#include "Kernel/DaVinciAlgorithm.h"
#include "Kernel/IRelatedPVFinder.h"

class MergeEvent : public DaVinciAlgorithm {
public: 
  MergeEvent(const std::string& name, ISvcLocator* pSvcLocator);

  virtual ~MergeEvent();

  virtual StatusCode initialize();
  virtual StatusCode execute   ();
  virtual StatusCode finalize  ();

protected:

private:

};

#endif // CHECKTRACKS_H
