#ifndef GAUDIEXAMPLES_MULTIINPUT_REGISTERADDR_H
#define GAUDIEXAMPLES_MULTIINPUT_REGISTERADDR_H

// Framework include files
#include "GaudiKernel/Algorithm.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "RootCnv/RootAddress.h"
#include "MIHelpers.h"

#include <vector>

/** Simple algorithm used to read data from two files. */
class RegisterAddr : public GaudiAlgorithm {
public:
    RegisterAddr(const std::string& name, ISvcLocator* pSvcLoc);
    virtual ~RegisterAddr();
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode execute();
private:
    std::string m_addressfile;
    /// Address details for the data to be added to the main event.
    std::vector<RootAddressArgs> m_addresses;
    size_t m_count;
    std::string m_prefix;
    std::vector<std::string> m_locations;

    StatusCode relinkAll(const DataObject* pObj);
    StatusCode resetLinks(const DataObject* pMCObj);
};

#endif
