#ifndef MERGEEVENT_H 
#define MERGEEVENT_H

// Include files 
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

/** @class MergeEvent MergeEvent.h
 *
 *  @author Igor Babuschkin
 *  @date   2014-07-09
 */
class MergeEvent : public GaudiAlgorithm {
public: 
  MergeEvent( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MergeEvent( );

  virtual StatusCode initialize();
  virtual StatusCode execute   ();
  virtual StatusCode finalize  ();

protected:

private:

};

#endif // CHECKTRACKS_H
