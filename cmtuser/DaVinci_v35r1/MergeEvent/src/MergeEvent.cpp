#include <typeinfo> 

#include "GaudiKernel/AlgFactory.h"

#include "MergeEvent.h"
#include "Event/Particle.h"
 
DECLARE_ALGORITHM_FACTORY( MergeEvent )

MergeEvent::MergeEvent( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{

}

MergeEvent::~MergeEvent() {} 

StatusCode MergeEvent::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;

  if ( msgLevel(MSG::DEBUG) ) debug() << "Initialize " << endmsg;

  return StatusCode::SUCCESS;
}

StatusCode MergeEvent::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "Execute MergeEvent" << endmsg;

  LHCb::Tracks *tracks = get<LHCb::Tracks>("/Event/Rec/Track/Best");
  LHCb::Tracks *extraTracks = get<LHCb::Tracks>("/Event/NewEvent/Rec/Track/Best");
  info() << "********* There are " << tracks->size() << " tracks in main event" << endmsg;
  info() << "********* There are " << extraTracks->size() << " extra tracks in extra event" << endmsg;

  return StatusCode::SUCCESS;
}

StatusCode MergeEvent::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

