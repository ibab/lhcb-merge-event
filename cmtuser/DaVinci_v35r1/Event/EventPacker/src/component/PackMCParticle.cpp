
// from Gaudi
#include "GaudiKernel/AlgFactory.h"

#include "Event/MCParticle.h"
#include "Event/StandardPacker.h"
#include "Event/PackedMCParticle.h"
// local
#include "PackMCParticle.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PackMCParticle
//
// 2005-03-18 : Olivier Callot
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( PackMCParticle )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  PackMCParticle::PackMCParticle( const std::string& name,
                                  ISvcLocator* pSvcLocator)
    : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = LHCb::MCParticleLocation::Default );
  declareProperty( "OutputName", m_outputName = LHCb::PackedMCParticleLocation::Default );
  declareProperty( "AlwaysCreateOutput",         m_alwaysOutput = false     );
  declareProperty( "DeleteInput",                m_deleteInput  = false     );
}

//=============================================================================
// Destructor
//=============================================================================
PackMCParticle::~PackMCParticle() {}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PackMCParticle::execute() {

  // Check to see if the output data already exists.
  if( exist<LHCb::PackedMCParticles>(m_outputName) ) {
    if (msgLevel(MSG::DEBUG) )
      debug() << "Output already exists at '" << m_outputName << "'" << endmsg;
    return StatusCode::SUCCESS;
  }

  // If input does not exist, and we aren't making the output regardless, just return
  if ( !m_alwaysOutput && !exist<LHCb::MCParticles>(m_inputName) ) {
    if (msgLevel(MSG::DEBUG) )
      debug() << "Input does not exist at '" << m_inputName << "'" << endmsg;
    return StatusCode::SUCCESS;
  }

  LHCb::MCParticles* parts = getOrCreate<LHCb::MCParticles,LHCb::MCParticles>( m_inputName );

  if ( msgLevel(MSG::DEBUG) )
    debug() << m_inputName << " contains " << parts->size()
            << " MCParticles to convert." << endmsg;

  StandardPacker pack;

  LHCb::PackedMCParticles* out = new LHCb::PackedMCParticles();
  put( out, m_outputName );

  out->reserve( parts->size() );
  for ( LHCb::MCParticles::const_iterator itP = parts->begin();
        parts->end() != itP; ++itP )
  {
    LHCb::MCParticle* part = (*itP);
    LHCb::PackedMCParticle newPart;

    newPart.key  = part->key();
    newPart.px   = pack.energy( part->momentum().px() );
    newPart.py   = pack.energy( part->momentum().py() );
    newPart.pz   = pack.energy( part->momentum().pz() );
    newPart.mass = (float)part->virtualMass();
    newPart.PID  = part->particleID().pid();
    newPart.originVertex = pack.reference( out,
                                           part->originVertex()->parent(),
                                           part->originVertex()->key() );
    for ( SmartRefVector<LHCb::MCVertex>::const_iterator itV = part->endVertices().begin();
          part->endVertices().end() != itV; ++itV ) {
      int ref = pack.reference( out, (*itV)->parent(), (*itV)->key() );
      newPart.endVertices.push_back( ref );
      if ( MSG::VERBOSE >= msgLevel() ) {
        verbose() << "Reference to endVertex       "
                  << format( "%8x",  ref ) << endmsg;
      }
    }
    out->addEntry( newPart );
  }

  // If requested, remove the input data from the TES and delete
  if ( UNLIKELY(m_deleteInput) )
  {
    const StatusCode sc = evtSvc()->unregisterObject( parts );
    if ( sc.isSuccess() )
    {
      delete parts;
      parts = NULL;
    }
    else
    {
      return Error("Failed to delete input data as requested", sc );
    }
  }
  else
  {
    // Clear the registry address of the unpacked container, to prevent reloading
    parts->registry()->setAddress( 0 );
  }

  return StatusCode::SUCCESS;
}

//=============================================================================