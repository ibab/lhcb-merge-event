// Framework include files
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataManagerSvc.h"

#include <Event/Track.h>

// Example related include files
#include "RegisterAddr.h"
#include "MIHelpers.h"

#include "GaudiKernel/System.h"

#include <fstream>

DECLARE_COMPONENT(RegisterAddr)

RegisterAddr::RegisterAddr(const std::string& name, ISvcLocator* pSvcLoc):
  GaudiAlgorithm(name, pSvcLoc), m_count(0) {
  declareProperty("AddressesFile", m_addressfile,
      "File containing the address details of the extra data.");
}
RegisterAddr::~RegisterAddr(){}

//--------------------------------------------------------------------
// Initialize
//--------------------------------------------------------------------
StatusCode RegisterAddr::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;

  MsgStream log(msgSvc(), name());
  if (outputLevel() <= MSG::DEBUG)
    log << MSG::DEBUG << "Reading " << m_addressfile << endmsg;
  m_addresses.clear();
  std::ifstream input(m_addressfile.c_str());
  while (input.good()) {
    RootAddressArgs addr;
    input >> addr;
    if (input.eof()) break;
    m_addresses.push_back(addr);
  }
  if (outputLevel() <= MSG::DEBUG)
    log << MSG::DEBUG << "Read " << m_addresses.size() << " addresses" << endmsg;

  m_count = 0;

  return StatusCode::SUCCESS;
}

//--------------------------------------------------------------------
// Finalize
//--------------------------------------------------------------------
StatusCode RegisterAddr::finalize() {
  return Algorithm::finalize();
}

//--------------------------------------------------------------------
// Execute
//--------------------------------------------------------------------
StatusCode RegisterAddr::execute() {
  // This just makes the code below a bit easier to read (and type)
  MsgStream log(msgSvc(), name());

  if (m_count < m_addresses.size()) {
    info() << "Registering " << make_address(m_addresses[m_count]) << endmsg;
    StatusCode sc = SmartIF<IDataManagerSvc>(eventSvc())
        ->registerAddress("NewEvent", make_address(m_addresses[m_count]));
    if (sc.isFailure()) {
      log << MSG::ERROR << "Failed to register the address to the extra data"
          << endmsg;
      return sc;
    }
    ++m_count;
  }

  DataObject *trks2 = get<DataObject>("NewEvent/pRec/Track");
  if (trks2) {
      log << MSG::INFO << "Extra event tracks is not null: " << trks2->name()  << "/" << trks2->clID() << endmsg;
  } else
      log << MSG::WARNING << "No tracks container in extra event" << endmsg;

  return StatusCode::SUCCESS;
}
