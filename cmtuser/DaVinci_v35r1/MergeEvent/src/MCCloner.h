
#include <map>
#include "Event/Particle.h"
#include "Event/MCParticle.h"


class MCCloner {

public:

    MCCloner();
    LHCb::MCParticle* cloneMCP (const LHCb::MCParticle* mcp);
    LHCb::MCVertex* cloneMCV (const LHCb::MCVertex* mcVertex);
    LHCb::MCParticle* getStoredMCP(const LHCb::MCParticle* mcp);
    LHCb::MCVertex* getStoredMCV(const LHCb::MCVertex* mcv);

private:

    LHCb::MCParticle* cloneKeyedMCP(const LHCb::MCParticle* mcp);
    LHCb::MCVertex* cloneKeyedMCV(const LHCb::MCVertex* mcv);
    LHCb::MCParticle* doCloneMCP (const LHCb::MCParticle* mcp);
    LHCb::MCVertex* doCloneMCV (const LHCb::MCVertex* mcVertex);

    inline bool cloneOriginVertex(const LHCb::MCVertex* vertex)
    {
      return ( vertex != NULL );
      // return vertex && (vertex->isDecay() || vertex->isPrimary() );
    }

    void cloneDecayVertices(const SmartRefVector<LHCb::MCVertex>& endVertices, LHCb::MCParticle* clonedParticle);
    void cloneDecayProducts(const SmartRefVector<LHCb::MCParticle>& products, LHCb::MCVertex* clonedVertex);

    std::map<const LHCb::MCParticle*, LHCb::MCParticle*> m_mcps;
    std::map<const LHCb::MCVertex*, LHCb::MCVertex*> m_mcvs;

};
