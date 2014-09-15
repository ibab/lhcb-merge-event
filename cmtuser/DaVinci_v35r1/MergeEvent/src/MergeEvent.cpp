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

  //auto mainprotos = get<LHCb::ProtoParticles>("/Event/Rec/ProtoP/Charged");
  //auto maintracks = get<LHCb::ProtoParticles>("/Event/Rec/Track/Best");

  LHCb::Particles *mainParts = get<LHCb::Particles>("/Event/Phys/DsPlusCandidates/Particles");
  LHCb::Particles *otherParts = get<LHCb::Particles>("/Event/NewEvent/Phys/DsMinusCandidates/Particles");

  /*
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
  */

  LHCb::Particle *mainP = nullptr;
  
  for (auto &p: *mainParts) {
      mainP = p;
      break;
  }
  
  const LHCb::VertexBase *mainPV = this->bestPV(mainP);

  info () << "Main PV is " << *mainPV << endmsg;

  auto results = new LHCb::Particles;

  for (auto p: *otherParts) {
      const LHCb::VertexBase *bestPV = this->bestPV(p);

      info () << "Old PV is " << *bestPV << endmsg;
      auto offset = mainPV->position() - bestPV->position();

      //const LHCb::Track *track = p->proto()->track();

      //for (auto s: track->states()) {
      //    auto curPos = s->position();
      //    auto newPos = s->position() - offset;
      //}
      
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

          // Translate vertex location
          if (curr->endVertex()) {
              auto pos = curr->endVertex()->position();
              LHCb::Particle* clone = curr->clone();
              clone->endVertex()->setPosition(pos + offset);
              results->add(clone);
              const LHCb::VertexBase *newPV = this->bestPV(clone);
              info() << "New PV is " << *newPV << endmsg;
          }
          info() << "Current endvertex is " << curr->endVertex() << endmsg;
      }

      for (auto x: leaves) {
         auto track = x->proto()->track();
          for (auto s: track->states()) {
              info() << "State was " << *s << endmsg;
              auto oldPos = s->position();
              auto newPos = oldPos + offset;
              s->setX(newPos.x());
              s->setY(newPos.y());
              s->setZ(newPos.z());
              info() << "State is " << *s << endmsg;
          }
      }

  }

  //auto decayVertices = get<LHCb::Vertices>("/Event/NewEvent/Phys/DsMinusCandidates/decayVertices");

  put(results, "/Event/Phys/DsMinusCandidates/Particles");
  //put(decayVertices, "/Event/Phys/DsMinusCandidates/decayVertices");

  return StatusCode::SUCCESS;
}

StatusCode MergeEvent::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return DaVinciAlgorithm::finalize();  // must be called after all other actions
}

