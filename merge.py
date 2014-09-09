#!/usr/bin/env gaudirun.py

from Gaudi.Configuration import *
from Configurables import DaVinci
from Configurables import GaudiSequencer, CombineParticles, FilterDesktop, DecayTreeTuple, CheckPV, UnpackTrack, MergeEvent, SelDSTWriter
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand
from CommonMCParticles import StandardMCKaons, StandardMCPions
from DecayTreeTuple.Configuration import *
from Configurables import RegisterAddr
from OtherMCParticles import *
from Configurables import StoreExplorerAlg

import os
sys.path.append(os.getcwd())
from MainSelection import *
from OtherSelection import *

GaudiPersistency()
importOptions("data_local.py")


_dsplus = DataOnDemand(Location=seqD2KKPiMain.outputLocation())
_dsminus = DataOnDemand(Location=seqD2KKPiOther.outputLocation())
combine = CombineParticles('CombineToBs')
combine.DecayDescriptor = "B_s0 -> D_s+ D_s-"
combine.MotherCut = "ALL"
combine.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]

bsSelection = Selection("FakeBsCandidates",
                           Algorithm = combine,
                           RequiredSelections=[_dsplus, _dsminus])

bsSelectionSequence = SelectionSequence('FilterCombined', TopSelection = bsSelection)

evtAlgs = GaudiSequencer("EventAlgs",
                         Members=[seqD2KKPiMain.sequence(),
                                  RegisterAddr(AddressesFile='eventaddr.txt'),
                                  makeparts,
                                  seqD2KKPiOther.sequence(),
                                  bsSelectionSequence.sequence(),
                                  StoreExplorerAlg('Explorer'),
                                  MergeEvent(),
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
#unpackMC.OutputLevel = 1
DataOnDemandSvc().AlgMap['/Event/NewEvent/MC/Particles'] = unpackMC

from Configurables import UnpackMCVertex
unpackMCV = UnpackMCVertex('OtherUnpackMCVertex', InputName='/Event/NewEvent/pSim/MCVertices', OutputName='/Event/NewEvent/MC/Vertices')
#unpackMCV.OutputLevel = 1
DataOnDemandSvc().AlgMap['/Event/NewEvent/MC/Vertices'] = unpackMCV

from Configurables import UnpackRecVertex
unpackPV = UnpackRecVertex('OtherUnpackRecVertex', InputName='/Event/NewEvent/pRec/Vertex/Primary', OutputName='/Event/NewEvent/Rec/Vertex/Primary', WeightsVector='/Event/NewEvent/Rec/Vertex/Weights')
#unpackPV.OutputLevel = 1
DataOnDemandSvc().AlgMap['/Event/NewEvent/Rec/Vertex/Primary'] = unpackPV

# Change the column size of Timing table
from Configurables import TimingAuditor, SequencerTimerTool
TimingAuditor().addTool(SequencerTimerTool,name="TIMER")
TimingAuditor().TIMER.NameSize = 60

from Configurables import AuditorSvc 
AuditorSvc().Auditors.append("TES::TraceAuditor")

