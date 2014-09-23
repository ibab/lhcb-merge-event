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

  auto results = new LHCb::Particles;

  const std::string finderType = "GenericParticle2PVRelator__p2PVWithIPChi2_OfflineDistanceCalculatorName_/OtherPVFinder";
  auto m_otherBestPV = tool<IRelatedPVFinder>(finderType, this);

  if (mainPV) {
      for (auto p: *otherParts) {
          const LHCb::VertexBase *bestPV = m_otherBestPV->relatedPV(p, "/Event/NewEvent/Rec/Vertex/Primary");

          info () << "Old PV is " << *bestPV << endmsg;
          auto offset = mainPV->position() - bestPV->position();

          auto clone = p->clone();

          auto pos = clone->referencePoint();
          clone->setReferencePoint(pos + offset);

          if (clone->endVertex()) {
              auto pos = clone->endVertex()->position();
              clone->endVertex()->setPosition(pos + offset);
          }

          results->insert(clone);
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

