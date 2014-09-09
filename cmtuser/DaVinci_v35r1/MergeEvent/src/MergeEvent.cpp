#include <typeinfo> 

#include "GaudiKernel/AlgFactory.h"

#include "MergeEvent.h"
#include "Event/Particle.h"
#include "Event/MCParticle.h"
#include "GaudiKernel/IDataManagerSvc.h"

#include <stack>

 
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

  debug() << "Execute MergeEvent" << endmsg;

  auto mainprotos = get<LHCb::ProtoParticles>("/Event/Rec/ProtoP/Charged");
  //auto maintracks = get<LHCb::ProtoParticles>("/Event/Rec/Track/Best");

  auto newprotos = new LHCb::ProtoParticles();
  auto newtracks = new LHCb::Tracks();

  // Copy old protos/tracks
  for (auto p: *mainprotos) {
      auto cl = p->clone();
      newprotos->add(cl);
      newtracks->add(cl->track()->clone());
  }

  LHCb::Particles *parts = get<LHCb::Particles>("/Event/NewEvent/Phys/SelD2KKPiOther/Particles");

  for (auto part: *parts) {
      std::stack<LHCb::Particle> next;
      LHCb::Particle::Vector leaves;

      // Traverse the decay tree and take note of all
      // decay products with proto particles
      next.push(*part);

      while (!next.empty()) {
          LHCb::Particle curr = next.top();
          next.pop();

          for (auto daugh: curr.daughters()) {
              next.push(*daugh);

              if (daugh->proto()) {
                  leaves.push_back(daugh);
              }
          }
      }

      for (auto x: leaves) {
          auto cl = x->proto()->clone();
          newprotos->add(cl);
          newtracks->add(cl->track()->clone());
      }
  }

  put(newprotos, "/Event/MergedEvent/Rec/ProtoP/Charged");
  put(newtracks, "/Event/MergedEvent/Rec/Track/Best");

  return StatusCode::SUCCESS;
}

StatusCode MergeEvent::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

