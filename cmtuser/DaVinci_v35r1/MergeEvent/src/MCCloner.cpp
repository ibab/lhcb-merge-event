#include "MCCloner.h"

LHCb::MCParticle* MCCloner::getStoredMCP(const LHCb::MCParticle* mcp) {
    auto result = m_mcps.find(mcp);
    if (result == m_mcps.end()) {
        return nullptr;
    } else {
        return result->second;
    }
}

LHCb::MCVertex* MCCloner::getStoredMCV(const LHCb::MCVertex* mcv) {
    auto result = m_mcvs.find(mcv);
    if (result == m_mcvs.end()) {
        return nullptr;
    } else {
        return result->second;
    }
}

LHCb::MCParticle* MCCloner::cloneKeyedMCP(const LHCb::MCParticle* mcp) {
    auto clone = getStoredMCP(mcp);
    if (!clone) {
        clone = mcp->clone();
        m_mcps.insert(std::pair<const LHCb::MCParticle*, LHCb::MCParticle*>(mcp, clone));
    }
    return clone;
}

LHCb::MCVertex* MCCloner::cloneKeyedMCV(const LHCb::MCVertex* mcv) {
    auto clone = getStoredMCV(mcv);
    if (!clone) {
        clone = mcv->clone();
        m_mcvs.insert(std::pair<const LHCb::MCVertex*, LHCb::MCVertex*>(mcv, clone));
    }
    return clone;
}

LHCb::MCParticle* MCCloner::cloneMCP ( const LHCb::MCParticle* mcp )
{
  if ( !mcp ) return NULL;
  LHCb::MCParticle * clone = getStoredMCP(mcp);
  return ( clone ? clone : this->doCloneMCP(mcp) );
}

LHCb::MCParticle* MCCloner::doCloneMCP( const LHCb::MCParticle* mcp )
{
  if ( !mcp ) return NULL;

  // Clone the MCParticle
  LHCb::MCParticle * clone = cloneKeyedMCP(mcp);
  
  // Original origin vertex
  const LHCb::MCVertex * originVertex = mcp->originVertex();

  // Should we clone the origin vertex ?
  if ( cloneOriginVertex(originVertex) )
  {
   
    // Has it already been cloned
    LHCb::MCVertex* originVertexClone = getStoredMCV(originVertex);
    if ( !originVertexClone )
    {
      originVertexClone =
        cloneKeyedMCV(originVertex);
      originVertexClone->clearProducts();

      // Clone the origin vertex mother
      const LHCb::MCParticle* mother = mcp->mother();
      LHCb::MCParticle* motherClone = ( mother ? cloneMCP(mother) : NULL );
      originVertexClone->setMother(motherClone);
    }

    // Add the cloned origin vertex to the cloned MCP
    clone->setOriginVertex( originVertexClone );

    // Add the cloned MCP to the cloned origin vertex, if not already there
    bool found = false;
    for ( SmartRefVector<LHCb::MCParticle>::const_iterator i 
            = originVertexClone->products().begin();
          i != originVertexClone->products().end(); ++i )
    {
      const LHCb::MCParticle * c = *i;
      if ( c == clone ) { found = true; break; }
    }
    if ( !found ) { originVertexClone->addToProducts(clone); }

  }
  else
  {
    clone->setOriginVertex(NULL);
  }

  // Clone the end vertices
  clone->clearEndVertices();
  cloneDecayVertices( mcp->endVertices(), clone );

  return clone;
}


void
MCCloner::cloneDecayVertices( const SmartRefVector<LHCb::MCVertex>& endVertices,
                                      LHCb::MCParticle* clonedParticle )
{
  for ( SmartRefVector<LHCb::MCVertex>::const_iterator iEndVtx = endVertices.begin();
        iEndVtx!=endVertices.end(); ++iEndVtx )
  {
    if ( (*iEndVtx)->isDecay() && !((*iEndVtx)->products().empty()) )
    {
      LHCb::MCVertex* decayVertexClone = cloneMCV(*iEndVtx);
      clonedParticle->addToEndVertices(decayVertexClone);
    }
  }
}

LHCb::MCVertex* MCCloner::cloneMCV (const LHCb::MCVertex* vertex)
{
  if ( !vertex ) return NULL;
  LHCb::MCVertex* clone = getStoredMCV(vertex);

  const size_t nProd      = vertex->products().size();
  const size_t nCloneProd = ( clone ? clone->products().size() : 0 );

  return ( clone && (nProd == nCloneProd) ? clone : this->doCloneMCV(vertex) );
}

LHCb::MCVertex* MCCloner::doCloneMCV(const LHCb::MCVertex* vertex)
{
  LHCb::MCVertex* clone =
    cloneKeyedMCV(vertex);

  clone->setMother(cloneMCP(vertex->mother()));

  clone->clearProducts();

  cloneDecayProducts( vertex->products(), clone );

  return clone;
}

void 
MCCloner::cloneDecayProducts(const SmartRefVector<LHCb::MCParticle>& products,
                                   LHCb::MCVertex* clonedVertex)
{
  for ( SmartRefVector<LHCb::MCParticle>::const_iterator iProd = products.begin();
        iProd != products.end(); ++iProd )
  {
    LHCb::MCParticle* productClone = cloneMCP(*iProd);
    if ( productClone )
    {
      productClone->setOriginVertex( clonedVertex );
      clonedVertex->addToProducts( productClone );
    }
  }
}

MCCloner::MCCloner() 
 : m_mcps(),
   m_mcvs()
{

}


