from Gaudi.Configuration import *

#importOptions("$APPCONFIGOPTS/Brunel/DataType-2010.py")
#importOptions("$APPCONFIGOPTS/Brunel/DataType-2011.py")
importOptions("$APPCONFIGOPTS/Brunel/DataType-2012.py")

from GaudiConf import IOHelper;
##IOHelper('MDF').inputFiles(['/afs/cern.ch/user/r/rlambert/public/forMarkus/run_69669_large_2ev.mdf']);
##IOHelper('MDF').inputFiles(['castor://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/data/2011/RAW/FULL/LHCb/COLLISION11/102907/102907_0000000002.raw']);
##IOHelper('MDF').inputFiles(['castor://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/data/2011/RAW/FULL/LHCb/COLLISION11/97114/097114_0000000004.raw?svcClass=lhcbtape'])
##IOHelper('MDF').inputFiles(['/castorfs/cern.ch/grid/lhcb/data/2011/RAW/FULL/LHCb/COLLISION11/97114/097114_0000000004.raw'])

IOHelper('MDF').inputFiles(['castor://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/data/2012/RAW/FULL/LHCb/COLLISION12/133586/133586_0000000309.raw'])

from Configurables import Brunel, AuditorSvc, OutputStream;
ApplicationMgr().ExtSvc.insert(0,AuditorSvc())
Brunel().EvtMax=10;
Brunel().OutputType='DST';
Brunel().Persistency='ROOT';
Brunel().OutputLevel = 4;
MessageSvc().OutputLevel = 4;
AuditorSvc().Auditors.append("TES::TraceAuditor")
OutputStream('DstWriter').Output = "DATAFILE='PFN:someFile.dst' TYP='ROOT' OPT='REC'"

def fixstuff():
    for stream in IOHelper().activeStreams():
        if IOHelper().detectStreamType(stream)=="FSR":
            stream.Output=stream.Output.replace("SVC='RootCnvSvc'","SVC='FileRecordCnvSvc'")

appendPostConfigAction(fixstuff)
