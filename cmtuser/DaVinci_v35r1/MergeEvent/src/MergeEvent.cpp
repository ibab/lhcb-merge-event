#include <typeinfo> 

#include "GaudiKernel/AlgFactory.h"

#include "MergeEvent.h"
#include "Event/Particle.h"
#include "Event/MCParticle.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "Relations/Relation1D.h"

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

  m_assoc = tool<IParticle2MCAssociator>("MCMatchObjP2MCRelator/MyRelator", this);

  return StatusCode::SUCCESS;
}

template <class T>
T* getOrAdd(T const * elem, std::vector<T const *> &reference, std::vector<T*> &cache) {

  auto it = std::find(reference.begin(), reference.end(), elem);

  // If the object is not know, we ignore it
  if (it != reference.end()) {
      int n = std::distance(reference.begin(), it);

      if (!cache[n]) {
          auto clone = elem->clone();
          cache[n] = clone;
          return clone;
      } else {
          return cache[n];
      }
  }
}

StatusCode MergeEvent::execute() {

  debug() << "Execute MergeEvent" << endmsg;

  auto newProtos = get<LHCb::ProtoParticles>("/Event/Rec/ProtoP/Charged");
  auto newTracks = get<LHCb::Tracks>("/Event/Rec/Track/Best");
  auto newMCParts = get<LHCb::MCParticles>("/Event/MC/Particles");
  auto newMCVertices = get<LHCb::MCVertices>("/Event/MC/Vertices");
  info() << "Length of MCVertices is " << newMCVertices->size() << endmsg;
  auto newRelations = get<LHCb::RelationWeighted1D<LHCb::ProtoParticle,LHCb::MCParticle,double>>("/Event/Relations/Rec/ProtoP/Charged");

  LHCb::Particles *mainParts = get<LHCb::Particles>("/Event/Phys/DsPlusCandidates/Particles");
  LHCb::Particles *otherParts = get<LHCb::Particles>("/Event/NewEvent/Phys/DsMinusCandidates/Particles");
  auto otherRelations = get<LHCb::RelationWeighted1D<LHCb::ProtoParticle,LHCb::MCParticle,double>>("/Event/NewEvent/Relations/NewEvent/Rec/ProtoP/Charged");

  LHCb::Particle *mainP = nullptr;
  
  for (auto &p: *mainParts) {
      mainP = p;
      break;
  }
  
  const LHCb::VertexBase *mainPV = this->bestPV(mainP);

  info () << "Main PV is " << *mainPV << endmsg;

  for (auto p: *otherParts) {

      // Clone all MC info associated with candidate
      std::vector<LHCb::MCParticle const *> traversed;
      std::vector<LHCb::MCVertex const *> vertices;
      auto mcp = m_assoc->relatedMCP(p, "/Event/NewEvent/MC/Particles");

      std::stack<LHCb::MCParticle const *> remaining;
      remaining.push(mcp);

      // Traverse current MC info first
      while (!remaining.empty()) {
          auto curr = remaining.top();
          remaining.pop();
          traversed.push_back(curr);
          if (std::find(vertices.begin(), vertices.end(), curr->originVertex()) == vertices.end()) {
              vertices.push_back(curr->originVertex());
          }
          info() << "Origin: " << curr->originVertex() << endmsg;

          for (auto vtx: curr->endVertices()) {
              info() << "End: " << vtx << endmsg;
              for (auto x: vtx->products()) {
                  remaining.push(x);
              }
          }
      }

      // Now copy the MC tree
      info() << "Number of mcps to copy: " << traversed.size() << endmsg;
      info() << "Number of vertices to copy: " << vertices.size() << endmsg;

      std::vector<LHCb::MCParticle *> traversedC(nullptr, traversed.size());
      std::vector<LHCb::MCVertex *> verticesC(nullptr, vertices.size());

      remaining.push(mcp);
      while (!remaining.empty()) {
          auto curr = remaining.top();
          remaining.pop();

          auto currClone = getOrAdd(curr, traversed, traversedC);
          auto ovtxClone = getOrAdd(curr->originVertex(), vertices, verticesC);

          currClone->setOriginVertex(ovtxClone);
          
          if (find::

      }

      //newMCParts->insert(mcp);
      //newMCVertices->insert(vtx);

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

          auto xmcp = m_assoc->relatedMCP(x, "/Event/NewEvent/MC/Particles")->clone();

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
  put(newMCParts, "/Event/MergedEvent/MC/Particles");
  put(newRelations, "/Event/MergedEvent/Relations/MergedEvent/Rec/ProtoP/Charged");

  return StatusCode::SUCCESS;
}

StatusCode MergeEvent::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return DaVinciAlgorithm::finalize();  // must be called after all other actions
}

