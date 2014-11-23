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

#_dsplus = DataOnDemand(Location=seqD2KKPiMain.outputLocation())
#_dsminus = DataOnDemand(Location='/Event/NewEvent/Phys/DsMinusCandidates/Particles')
#combine = CombineParticles('CombineToBs')
#combine.DecayDescriptor = "B_s0 -> D_s+ D_s-"
#combine.MotherCut = "ALL"
#combine.Preambulo = [
#    "from LoKiPhysMC.decorators import *",
#    "from PartProp.Nodes import CC" ]
#
#bsSelection = Selection("FakeBsCandidates",
#                           Algorithm = combine,
#                           RequiredSelections=[_dsplus, _dsminus])
#
#bsSelectionSequence = SelectionSequence('FilterCombined', TopSelection = bsSelection)

createEvent = GaudiSequencer('CreateFakeEvent')

mergedPions = NoPIDsParticleMaker('MergedPions'
                                 , Particle = "pion"
                                 , Input = "MergedEvent/Rec/ProtoP/Charged"
                                 , Output = "Phys/MergedPions/Particles"
                                 , WriteP2PVRelations = False
                                 , InputPrimaryVertices = "Rec/Vertex/Primary"
                                 )

mergedKaons = NoPIDsParticleMaker('MergedKaons'
                                 , Particle = "kaon"
                                 , Input = "MergedEvent/Rec/ProtoP/Charged"
                                 , Output = "Phys/MergedKaons/Particles"
                                 , WriteP2PVRelations = False
                                 , InputPrimaryVertices = "Rec/Vertex/Primary"
                                 )

createEvent.Members.append(mergedPions)
createEvent.Members.append(mergedKaons)

_kaons = DataOnDemand(Location='/Event/Phys/MergedPions/Particles')
_pions = DataOnDemand(Location='/Event/Phys/MergedKaons/Particles')

combineA = CombineParticles('CombineA')
combineA.DecayDescriptor = "D_s+ -> K+ K- pi+"
combineA.MotherCut = "(mcMatch('D_s+  ==> K+ K- pi+', ['/Event/MergedEvent/Relations/MergedEvent/Rec/ProtoP/Charged'], '/Event/MergedEvent/MC/Particles'))"
combineA.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]

combineB = CombineParticles('CombineB')
combineB.DecayDescriptor = "D_s- -> K- K+ pi-"
combineB.MotherCut = "(mcMatch('D_s-  ==> K- K+ pi-', ['/Event/MergedEvent/Relations/MergedEvent/Rec/ProtoP/Charged'], '/Event/MergedEvent/MC/Particles'))"
combineB.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]
#combineB.OutputLevel = 1

combineAB = CombineParticles('CombineAB')
combineAB.DecayDescriptor = "B_s0 -> D_s- D_s+"
combineAB.MotherCut = "ALL"
combineAB.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]

selectionA = Selection("FakeDsPlusSel",
                        Algorithm = combineA,
                        RequiredSelections=[_kaons, _pions])

selectionB = Selection("FakeDsMinusSel",
                        Algorithm = combineB,
                        RequiredSelections=[_kaons, _pions])

selASelectionSequence = SelectionSequence('FakeDsPlus', TopSelection = selectionA)
selBSelectionSequence = SelectionSequence('FakeDsMinus', TopSelection = selectionB)

selectionAB = Selection("FakeBsSel",
                        Algorithm = combineAB,
                        RequiredSelections=[DataOnDemand(Location=selASelectionSequence.outputLocation()),
                                            DataOnDemand(Location=selBSelectionSequence.outputLocation())])
                                            
selABSelectionSequence = SelectionSequence('FakeBs', TopSelection = selectionAB)


from Configurables import FitDecayTrees
fitD2KKP = FitDecayTrees ( 
    "fitD2KKP" , 
    Code = "DECTREE('B_s0 -> (D_s+ -> K+ K- pi+) (D_s- -> K- K+ pi-)')",
    MassConstraints = [ 'D_s+', 'D_s-' ], 
    )
fitD2KKP.Inputs = [selABSelectionSequence.outputLocation()]

from Configurables import P2MCPFromProtoP, BackgroundCategory
fakebstuple = DecayTreeTuple("out")
fakebstuple.Decay = "'[B_s0 -> ^D_s- ^D_s+]CC'"
fakebstuple.Inputs = ['Phys/fitD2KKP']
#fakebstuple.addTupleTool("TupleToolPropertime")
#bkgcat = fakebstuple.addTupleTool("TupleToolMCBackgroundInfo")
#bkgcat.IBackgroundCategoryTypes = ['BackgroundCategory/MyBC']
#bkgcat.addTool(BackgroundCategory, name='MyBC')
#bkgcat.MyBC.addTool(P2MCPFromProtoP, name='P2MCPFromProtoP')
#bkgcat.MyBC.P2MCPFromProtoP.Locations = ['MergedEvent/Relations/MergedEvent/Rec/ProtoP/Charged', 'Relations/Rec/ProtoP/Charged']
#bkgcat.MyBC.P2MCPFromProtoP.MCParticleDefaultLocation = 'MergedEvent/MC/Particles'
#fakebstuple.OutputLevel = 2

from Configurables import MCMatchObjP2MCRelator
merge = MergeEvent()
merge.addTool(MCMatchObjP2MCRelator, name='MyRelator')
merge.MyRelator.RelTableLocations = ['/Event/NewEvent/Relations/NewEvent/Rec/ProtoP/Charged']
#merge.OutputLevel = 1


evtAlgs = GaudiSequencer("EventAlgs",
                         Members=[seqD2KKPiMain.sequence(),
                                  RegisterAddr(AddressesFile='eventaddr.txt'),
                                  makeparts,
                                  seqD2KKPiOther.sequence(),
                                  merge,
                                  createEvent,
                                  StoreExplorerAlg('Explorer'),
                                  selASelectionSequence.sequence(),
                                  selBSelectionSequence.sequence(),
                                  selABSelectionSequence.sequence(),
                                  fitD2KKP,
                                  fakebstuple
                                  ])


from Configurables import DaVinci
DaVinci().EvtMax = 20000
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

#from Configurables import AuditorSvc 
#AuditorSvc().Auditors.append("TES::TraceAuditor")

