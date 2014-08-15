// Framework include files
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataManagerSvc.h"

#include <Event/Track.h>

// Example related include files
#include "RegisterAddr.h"
#include "MIHelpers.h"

#include "GaudiKernel/System.h"
#include "GaudiKernel/LinkManager.h"

#include <fstream>

DECLARE_COMPONENT(RegisterAddr)

RegisterAddr::RegisterAddr(const std::string& name, ISvcLocator* pSvcLoc):
  GaudiAlgorithm(name, pSvcLoc), m_count(0) {
  declareProperty("AddressesFile", m_addressfile,
      "File containing the address details of the extra data.");
  declareProperty("Prefix", m_prefix);
}
RegisterAddr::~RegisterAddr(){}

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

StatusCode RegisterAddr::finalize() {
  return Algorithm::finalize();
}

StatusCode RegisterAddr::execute() {
  MsgStream log(msgSvc(), name());

  // Pull one address from the list of addresses and register the event
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

  // Test if it works
  std::vector<std::string> paths({"NewEvent/pRec",
                                  "NewEvent/MC"});
  for (auto &path: paths) {
      DataObject *event = get<DataObject>(path);
      relinkAll(event);
  }

  return StatusCode::SUCCESS;
}

StatusCode RegisterAddr::resetLinks( const DataObject* pMCObj )
{

  debug() << "Resetting links for " << pMCObj->name() <<  endmsg;
  
  LinkManager::LinkVector oldlinks = pMCObj->linkMgr()->m_linkVector;
  debug() << "Link length: " << oldlinks.size() << endmsg;
  if (oldlinks.empty()) return StatusCode::SUCCESS;

  LinkManager::LinkIterator ilink;
  std::vector<std::pair<std::string, long>> refs;
  for (auto link: oldlinks) {
    debug() << "Old path: " << link->path() << endmsg;
    refs.push_back(std::make_pair(link->path(), link->ID()));
  }
  pMCObj->linkMgr()->clearLinks();
  for (auto ref: refs) {
    const std::string newPath = "/Event/NewEvent" + ref.first.erase(0, 6);
    debug() << "New path: " << newPath << endmsg;
    const long itemp = ref.second;
    const long lid = pMCObj->linkMgr()->addLink(newPath, NULL);
    if (lid != itemp) {
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode RegisterAddr::relinkAll( const DataObject* pObj) {

  // Find and load all the leafs out of the object
  std::vector<IRegistry*> leaves;
  SmartIF<IDataManagerSvc> dataMgr(eventSvc());
  StatusCode sc = dataMgr->objectLeaves(pObj, leaves);
  if( !sc.isSuccess() ) return sc;

  DataObject *pMCObj = 0;
  std::string objPath;

  for( std::vector<IRegistry*>::iterator ileaf=leaves.begin();
       ileaf!=leaves.end(); ++ileaf) {
    sc = eventSvc()->retrieveObject(*ileaf, objPath, pMCObj );
    if( sc.isFailure() ) {
      return sc;
    }
    sc = resetLinks( pMCObj );
    if( !sc.isSuccess() ) return sc;
    sc = relinkAll( pMCObj );
    if( !sc.isSuccess() ) return sc;
  }

  return StatusCode::SUCCESS;
}

