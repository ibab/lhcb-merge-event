########################################################################
from Gaudi.Configuration import *
from Configurables import DaVinci
from Configurables import GaudiSequencer, CombineParticles, FilterDesktop, DecayTreeTuple, CheckPV, UnpackTrack, MergeEvent, SelDSTWriter
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand
from CommonMCParticles import StandardMCKaons, StandardMCPions
from DecayTreeTuple.Configuration import *
from Configurables import RegisterAddr
from OtherMCParticles import *

GaudiPersistency()
importOptions("data_local.py")

# Selection for other loop
_otherKaons = DataOnDemand(Location='/Event/NewEvent/Phys/OtherAllKaons/Particles')
_otherPions = DataOnDemand(Location='/Event/NewEvent/Phys/OtherAllPions/Particles')
_otherd2kkpi = CombineParticles("otherd2kkpi")
_otherd2kkpi.DecayDescriptor = "D_s- -> K- K+ pi-"
_otherd2kkpi.MotherCut = "ALL"  #"(mcMatch('[D_s+  ==> K- K+ pi+]CC'))"
_otherd2kkpi.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]

selD2KKPiOther = Selection("SelD2KKPiOther",
                           Algorithm = _otherd2kkpi,
                           RequiredSelections=[_otherKaons,_otherPions])  

selD2KKPiOther.OutputLevel = 1

seqD2KKPiOther = SelectionSequence('MCFilterOther', TopSelection = selD2KKPiOther)

#tuple = DecayTreeTuple("Ds2KKPiTuple")
#tuple.Decay = "[D_s+ -> K- K+ pi+]CC"
#tuple.Inputs = [seqD2KKPiOther.outputLocation()]
##tuple.Inputs = ['Phys/SelD2KKPiOther/Particles']
##mcTruth = tuple.addTupleTool("TupleToolMCTruth")
#tuple.addTupleTool("TupleToolPropertime")

evtAlgs = GaudiSequencer("EventAlgs",
                         Members=[RegisterAddr(AddressesFile='eventaddr.txt', OutputLevel=DEBUG),
                                  makeparts,
                                  seqD2KKPiOther.sequence(),
                                  #tuple,
                                  #MergeEvent(),
                                  ])

from Configurables import DaVinci
DaVinci().EvtMax = 100
DaVinci().PrintFreq = 1
DaVinci().SkipEvents = 0
DaVinci().DataType = "2012"
DaVinci().HistogramFile = "meta.root"
DaVinci().Simulation = True
DaVinci().TupleFile = "out.root"
DaVinci().UserAlgorithms = [evtAlgs]

from Configurables import UnpackRecVertex
unpackT = UnpackTrack('UnpackTrackOther', RootInTES='/Event/NewEvent/')
unpackV = UnpackRecVertex('UnpackRecVertexOther', RootInTES='/Event/NewEvent/')
unpackV.OutputLevel = 1
#DataOnDemandSvc().AlgMap['/Event/NewEvent/Rec/Track/Best'] = unpackT
#DataOnDemandSvc().AlgMap['/Event/NewEvent/Rec/Vertex/Primary'] = unpackV

# Change the column size of Timing table
from Configurables import TimingAuditor, SequencerTimerTool
TimingAuditor().addTool(SequencerTimerTool,name="TIMER")
TimingAuditor().TIMER.NameSize = 60

