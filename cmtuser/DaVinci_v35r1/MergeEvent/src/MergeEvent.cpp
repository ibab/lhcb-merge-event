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
#include <map>

DECLARE_ALGORITHM_FACTORY( MergeEvent )

MergeEvent::MergeEvent( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : DaVinciAlgorithm ( name , pSvcLocator )
{
  declareProperty("OutputPrefix", m_prefix="MergedEvent");
}

MergeEvent::~MergeEvent() {} 

StatusCode MergeEvent::initialize() {

  StatusCode sc = DaVinciAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;

  if ( msgLevel(MSG::DEBUG) ) debug() << "Initialize " << endmsg;

  m_assoc = tool<IParticle2MCAssociator>("MCMatchObjP2MCRelator/MyRelator", this);
  m_mcCloner = tool<ICloneMCParticle>("MCParticleCloner", this);
  m_mcVCloner = tool<ICloneMCVertex>("MCVertexCloner", this);

  return StatusCode::SUCCESS;
}


StatusCode MergeEvent::execute() {

  MCCloner cloner;

  auto otherProtos = get<LHCb::ProtoParticles>("/Event/NewEvent/Rec/ProtoP/Charged");
  auto otherTracks = get<LHCb::Tracks>("/Event/NewEvent/Rec/Track/Best");
  auto otherPVs = get<LHCb::RecVertices>("/Event/NewEvent/Rec/Vertex/Primary");
  auto otherMCParticles = get<LHCb::MCParticles>("/Event/NewEvent/MC/Particles");
  auto otherMCVertices = get<LHCb::MCVertices>("/Event/NewEvent/MC/Vertices");
  auto otherRelations = get<LHCb::RelationWeighted1D<LHCb::ProtoParticle,LHCb::MCParticle,double>>("/Event/NewEvent/Relations/NewEvent/Rec/ProtoP/Charged");

  auto selected = get<LHCb::Particles>("/Event/NewEvent/Phys/DsMinusCandidates/Particles");

  auto mainMCParticles = get<LHCb::MCParticles>("/Event/MC/Particles");
  auto mainMCVertices = get<LHCb::MCVertices>("/Event/MC/Vertices");

  auto newProtos = get<LHCb::ProtoParticles>("/Event/Rec/ProtoP/Charged");
  auto newTracks = get<LHCb::Tracks>("/Event/Rec/Track/Best");
  auto newRelations = get<LHCb::RelationWeighted1D<LHCb::ProtoParticle,LHCb::MCParticle,double>>("/Event/Relations/Rec/ProtoP/Charged");
  auto newMCParticles = new LHCb::MCParticles;
  auto newMCVertices = new LHCb::MCVertices;

  std::map<const LHCb::MCParticle*, LHCb::MCParticle*> toClone;

  for (auto mcp: *mainMCParticles) {
      newMCParticles->insert(mcp);
  }
  for (auto mcv: *mainMCVertices) {
      newMCVertices->insert(mcv);
  }

  for (auto mcp: *otherMCParticles) {
      auto clone = cloner.cloneMCP(mcp);
      toClone.insert(std::pair<const LHCb::MCParticle*,LHCb::MCParticle*>(mcp, clone));
      newMCParticles->insert(clone);
  }

  for (auto mcv: *otherMCVertices) {
      auto clone = cloner.cloneMCV(mcv);
      newMCVertices->insert(clone);
  }

  for (auto p: *selected) {
      for (auto d: p->daughters()) {
          auto proto = d->proto()->clone();
          newProtos->insert(proto);
          auto track = proto->track()->clone();
          newTracks->insert(track);

          auto mcp = m_assoc->relatedMCP(d, "/Event/NewEvent/MC/Particles");

          for (auto entry: otherRelations->relations(d->proto())) {
              //info() << "Found relation!" << endmsg;
              newRelations->relate(proto, toClone.at(mcp), entry.weight());
          }
      }
      // Only include a single candidate for now
      break;
  }

  //info() << "len(newProtos) == " << newProtos->size() << endmsg;

  newMCParticles->linkMgr()->clearLinks();
  newMCParticles->linkMgr()->addLink("/Event/MergedEvent/MC/Vertices", newMCVertices);

  put(newProtos, "/Event/MergedEvent/Rec/ProtoP/Charged");
  put(newTracks, "/Event/MergedEvent/Rec/Track/Best");
  put(newRelations, "/Event/MergedEvent/Relations/MergedEvent/Rec/ProtoP/Charged");
  put(newMCParticles, "/Event/MergedEvent/MC/Particles");
  put(newMCVertices, "/Event/MergedEvent/MC/Vertices");
  put(otherPVs, "/Event/MergedEvent/Rec/Vertex/Primary");

  return StatusCode::SUCCESS;
}

StatusCode MergeEvent::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return DaVinciAlgorithm::finalize();  // must be called after all other actions
}

