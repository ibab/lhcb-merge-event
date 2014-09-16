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
  : DaVinciAlgorithm ( name , pSvcLocator )
{

}

MergeEvent::~MergeEvent() {} 

StatusCode MergeEvent::initialize() {

  StatusCode sc = DaVinciAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;

  if ( msgLevel(MSG::DEBUG) ) debug() << "Initialize " << endmsg;

  return StatusCode::SUCCESS;
}

StatusCode MergeEvent::execute() {

  debug() << "Execute MergeEvent" << endmsg;

  auto newProtos = get<LHCb::ProtoParticles>("/Event/Rec/ProtoP/Charged");
  auto newTracks = get<LHCb::Tracks>("/Event/Rec/Track/Best");

  LHCb::Particles *mainParts = get<LHCb::Particles>("/Event/Phys/DsPlusCandidates/Particles");
  LHCb::Particles *otherParts = get<LHCb::Particles>("/Event/NewEvent/Phys/DsMinusCandidates/Particles");

  auto mainMCParts = get<LHCb::MCParticles>("/Event/MC/Particles");

  LHCb::Particle *mainP = nullptr;
  
  for (auto &p: *mainParts) {
      mainP = p;
      break;
  }
  
  const LHCb::VertexBase *mainPV = this->bestPV(mainP);

  info () << "Main PV is " << *mainPV << endmsg;

  for (auto p: *otherParts) {
      const LHCb::VertexBase *bestPV = this->bestPV(p);

      info () << "Old PV is " << *bestPV << endmsg;
      auto offset = mainPV->position() - bestPV->position();

      std::stack<LHCb::Particle const *> next;
      LHCb::Particle::Vector leaves;

      next.push(p);

      // Traverse the decay tree and take note of all
      // decay products with proto particles
      while (!next.empty()) {
          auto curr = next.top();
          next.pop();

          for (auto daugh: curr->daughters()) {
              next.push(daugh);

              if (daugh->proto()) {
                  leaves.push_back(daugh);
              }
          }

          info() << "Current endvertex is " << curr->endVertex() << endmsg;
      }

      for (auto x: leaves) {
          auto proto = x->proto()->clone();
          auto track = proto->track()->clone();

          proto->setTrack(track);

          for (auto s: track->states()) {
              info() << "State was " << *s << endmsg;
              auto oldPos = s->position();
              auto newPos = oldPos + offset;
              s->setX(newPos.x());
              s->setY(newPos.y());
              s->setZ(newPos.z());
              info() << "State is " << *s << endmsg;
          }

          newProtos->insert(proto);
          newTracks->insert(track);
      }
  }

  put(newProtos, "/Event/MergedEvent/Rec/ProtoP/Charged");
  put(newTracks, "/Event/MergedEvent/Rec/Track/Best");

  return StatusCode::SUCCESS;
}

StatusCode MergeEvent::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return DaVinciAlgorithm::finalize();  // must be called after all other actions
}

