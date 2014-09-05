#!/usr/bin/env gaudirun.py

from Gaudi.Configuration import *
from Configurables import DaVinci
from Configurables import GaudiSequencer, CombineParticles, FilterDesktop, DecayTreeTuple, CheckPV, UnpackTrack, MergeEvent, SelDSTWriter
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand
from CommonMCParticles import StandardMCKaons, StandardMCPions
from DecayTreeTuple.Configuration import *
from Configurables import RegisterAddr
from OtherMCParticles import *

locationRoot = '/Event/NewEvent'

GaudiPersistency()
importOptions("data_local.py")

# Selection for other loop
_otherKaons = DataOnDemand(Location=locationRoot + '/Phys/OtherAllKaons/Particles')
_otherPions = DataOnDemand(Location=locationRoot + '/Phys/OtherAllPions/Particles')
_otherd2kkpi = CombineParticles("otherd2kkpi", InputPrimaryVertices=locationRoot + "/Rec/Vertex/Primary")
_otherd2kkpi.DecayDescriptor = "D_s- -> K- K+ pi-"
_otherd2kkpi.MotherCut = "(mcMatch('D_s-  ==> K- K+ pi-', ['/Event/NewEvent/Relations/NewEvent/Rec/ProtoP/Charged'], '/Event/NewEvent/MC/Particles'))"
_otherd2kkpi.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]

selD2KKPiOther = Selection("SelD2KKPiOther",
                           Algorithm = _otherd2kkpi,
                           RequiredSelections=[_otherKaons, _otherPions],
                           OutputBranch="NewEvent/Phys")

selD2KKPiOther.OutputLevel = 1

seqD2KKPiOther = SelectionSequence('MCFilterOther', TopSelection = selD2KKPiOther)

from Configurables import StoreExplorerAlg

expl = StoreExplorerAlg('Explorer')

tuple = DecayTreeTuple("Ds2KKPiTuple", RootInTES='/Event/NewEvent')
tuple.Decay = "[D_s+ -> K- K+ pi+]CC"
#tuple.Inputs = [seqD2KKPi.outputLocation()]
tuple.Inputs = ['Phys/SelD2KKPiOther/Particles']
tuple.ToolList = []
from Configurables import MCMatchObjP2MCRelator

tuple.addTupleTool('TupleToolKinematic')
tuple.addTupleTool('TupleToolPropertime')

mcTruth = tuple.addTupleTool("TupleToolMCTruth")
mcTruth.IP2MCPAssociatorTypes = ['MCMatchObjP2MCRelator/MyMCMatcher']
mcTruth.addTool(MCMatchObjP2MCRelator, name='MyMCMatcher')
mcTruth.MyMCMatcher.RelTableLocations = ['/Event/NewEvent/Relations/NewEvent/Rec/ProtoP/Charged']

#tuple.addTupleTool("TupleToolPropertime")

evtAlgs = GaudiSequencer("EventAlgs",
                         Members=[RegisterAddr(AddressesFile='eventaddr.txt', OutputLevel=DEBUG),
                                  #MergeEvent(),
                                  makeparts,
                                  expl,
                                  seqD2KKPiOther.sequence(),
                                  tuple,
                                  ])

from Configurables import DaVinci
DaVinci().EvtMax = 1
DaVinci().PrintFreq = 1
DaVinci().SkipEvents = 0
DaVinci().DataType = "2011"
DaVinci().DDDBtag = "MC11-20111102"
DaVinci().CondDBtag = "sim-20121025-vc-mu100"
DaVinci().HistogramFile = "meta.root"
DaVinci().Simulation = True
DaVinci().TupleFile = "out.root"
DaVinci().UserAlgorithms = [evtAlgs]

from Configurables import UnpackMCParticle
unpackMC = UnpackMCParticle('OtherUnpackMCParticle', InputName='/Event/NewEvent/pSim/MCParticles', OutputName='/Event/NewEvent/MC/Particles')
unpackMC.OutputLevel = 1
DataOnDemandSvc().AlgMap['/Event/NewEvent/MC/Particles'] = unpackMC

from Configurables import UnpackMCVertex
unpackMCV = UnpackMCVertex('OtherUnpackMCVertex', InputName='/Event/NewEvent/pSim/MCVertices', OutputName='/Event/NewEvent/MC/Vertices')
unpackMCV.OutputLevel = 1
DataOnDemandSvc().AlgMap['/Event/NewEvent/MC/Vertices'] = unpackMCV

from Configurables import UnpackRecVertex
unpackPV = UnpackRecVertex('OtherUnpackRecVertex', InputName='/Event/NewEvent/pRec/Vertex/Primary', OutputName='/Event/NewEvent/Rec/Vertex/Primary', WeightsVector='/Event/NewEvent/Rec/Vertex/Weights')
unpackPV.OutputLevel = 1
DataOnDemandSvc().AlgMap['/Event/NewEvent/Rec/Vertex/Primary'] = unpackPV

# Change the column size of Timing table
from Configurables import TimingAuditor, SequencerTimerTool
TimingAuditor().addTool(SequencerTimerTool,name="TIMER")
TimingAuditor().TIMER.NameSize = 60

from Configurables import AuditorSvc 
AuditorSvc().Auditors.append("TES::TraceAuditor")

