#!/usr/bin/env gaudirun.py

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
_otherd2kkpi.MotherCut = "(mcMatch('[D_s+  ==> K- K+ pi+]CC'))"
_otherd2kkpi.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]

selD2KKPiOther = Selection("SelD2KKPiOther",
                           Algorithm = _otherd2kkpi,
                           RequiredSelections=[_otherKaons,_otherPions])  

selD2KKPiOther.OutputLevel = 2

seqD2KKPiOther = SelectionSequence('MCFilterOther', TopSelection = selD2KKPiOther)

from Configurables import StoreExplorerAlg

expl = StoreExplorerAlg('Explorer')

evtAlgs = GaudiSequencer("EventAlgs",
                         Members=[RegisterAddr(AddressesFile='eventaddr.txt', OutputLevel=DEBUG),
                                  #MergeEvent(),
                                  makeparts,
                                  seqD2KKPiOther.sequence(),
                                  expl
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

