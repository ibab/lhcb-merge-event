#include <typeinfo> 

#include "GaudiKernel/AlgFactory.h"

#include "MergeEvent.h"
#include "Event/Particle.h"
#include "Event/MCParticle.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/Property.h"
#include "Relations/Relation1D.h"

#include "GaudiKernel/LinkManager.h"

#include <stack>

DECLARE_ALGORITHM_FACTORY( MergeEvent )

MergeEvent::MergeEvent( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : DaVinciAlgorithm ( name , pSvcLocator )
{
  declareProperty("OutputPrefix", m_prefix="MicroDST");
}

MergeEvent::~MergeEvent() {} 

StatusCode MergeEvent::initialize() {

  StatusCode sc = DaVinciAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;

  if ( msgLevel(MSG::DEBUG) ) debug() << "Initialize " << endmsg;

  m_assoc = tool<IParticle2MCAssociator>("MCMatchObjP2MCRelator/MyRelator", this);
  m_mcCloner = tool<ICloneMCParticle>("MCParticleCloner", this);

  return StatusCode::SUCCESS;
}


StatusCode MergeEvent::execute() {

  debug() << "Execute MergeEvent" << endmsg;

  auto newProtos = get<LHCb::ProtoParticles>("/Event/Rec/ProtoP/Charged");
  auto newTracks = get<LHCb::Tracks>("/Event/Rec/Track/Best");
  auto newMCParts = get<LHCb::MCParticles>("/Event/MC/Particles");
  auto newMCVertices = get<LHCb::MCVertices>("/Event/MC/Vertices");
  auto newRelations = get<LHCb::RelationWeighted1D<LHCb::ProtoParticle,LHCb::MCParticle,double>>("/Event/Relations/Rec/ProtoP/Charged");

  LHCb::Particles *mainParts = get<LHCb::Particles>("/Event/Phys/DsPlusCandidates/Particles");
  LHCb::Particles *otherParts = get<LHCb::Particles>("/Event/NewEvent/Phys/DsMinusCandidates/Particles");
  auto otherRelations = get<LHCb::RelationWeighted1D<LHCb::ProtoParticle,LHCb::MCParticle,double>>("/Event/NewEvent/Relations/NewEvent/Rec/ProtoP/Charged");

  auto extraMCP = new LHCb::MCParticle::Vector;

  LHCb::Particle *mainP = nullptr;
  
  for (auto &p: *mainParts) {
      mainP = p;
      break;
  }
  
  const LHCb::VertexBase *mainPV = this->bestPV(mainP);

  info () << "Main PV is " << *mainPV << endmsg;

  for (auto p: *otherParts) {

      // Clone all MC info associated with candidate
      auto mcp = m_assoc->relatedMCP(p, "/Event/NewEvent/MC/Particles");

      std::stack<LHCb::MCParticle const *> remaining;
      remaining.push(mcp);

      // Clone MC info
      while (!remaining.empty()) {
          auto curr = remaining.top();
          remaining.pop();
          auto clone = (*m_mcCloner)(curr);
          if (std::find(extraMCP->begin(), extraMCP->end(), clone) == extraMCP->end()) {
              extraMCP->push_back(clone);
          }

          for (auto vtx: curr->endVertices()) {
              for (auto x: vtx->products()) {
                  remaining.push(x);
              }
          }
      }

      // Clone and translate all event info associated with candidate
      const LHCb::VertexBase *bestPV = this->bestPV(p);
      auto offset = mainPV->position() - bestPV->position();

      for (auto x: p->daughters()) {
          auto proto = x->proto()->clone();
          auto track = proto->track()->clone();
          proto->setTrack(track);

          for (auto s: track->states()) {
              auto oldPos = s->position();
              auto newPos = oldPos + offset;
              s->setX(newPos.x());
              s->setY(newPos.y());
              s->setZ(newPos.z());
          }

          auto xmcp = (*m_mcCloner)(m_assoc->relatedMCP(x, "/Event/NewEvent/MC/Particles"));
          info() << "XMCP is " << xmcp << endmsg;

          newProtos->insert(proto);
          newTracks->insert(track);

          for (auto entry: otherRelations->relations(x->proto())) {
              newRelations->i_push(proto, xmcp, entry.weight());
              newRelations->i_sort();
          }
      }
  }

  put(newProtos, "/Event/MergedEvent/Rec/ProtoP/Charged");
  put(newTracks, "/Event/MergedEvent/Rec/Track/Best");
  put(newRelations, "/Event/MergedEvent/Relations/MergedEvent/Rec/ProtoP/Charged");

  auto tmpParts = new LHCb::MCParticles;
  //for (auto m: *newMCParts) {
  //    info() << "A Inserting " << m << endmsg;
  //    tmpParts->insert(m);
  //}
  for (auto m: *extraMCP) {
      info() << "B Inserting " << m << endmsg;
      tmpParts->insert(m);
  }

  put(tmpParts, "/Event/MergedEvent/MC/Particles");

  auto mcps = get<LHCb::MCParticles>("/Event/MergedEvent/MC/Particles");
  mcps->linkMgr()->clearLinks();
  mcps->linkMgr()->addLink("/Event/MergedEvent/MC/Vertices", newMCVertices);

  return StatusCode::SUCCESS;
}

StatusCode MergeEvent::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return DaVinciAlgorithm::finalize();  // must be called after all other actions
}

