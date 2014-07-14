########################################################################
from Gaudi.Configuration import *
from Configurables import DaVinci 
from Configurables import GaudiSequencer, CombineParticles, FilterDesktop, DecayTreeTuple, CheckPV, UnpackTrack, MergeEvent, SelDSTWriter
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand
from CommonMCParticles import StandardMCKaons, StandardMCPions
from DecayTreeTuple.Configuration import *
from Configurables import RegisterAddr

GaudiPersistency()
importOptions("data_local.py")

# BE CAREFUL: RUNTIME NAME HAS to be different, otherwise this is picked up
# by default to unpack prec/Track/Best
unpack = UnpackTrack("BCUnpack")
unpack.InputName = '/Event/NewEvent/pRec/Track'
unpack.OutputName = 'NewEvent/Rec/Track'
unpack.OutputLevel = DEBUG

evtAlgs = GaudiSequencer("EventAlgs",
                         Members=[ReadAlg(AddressesFile='trackaddr.txt', OutputLevel=DEBUG),
                                  unpack,
                                  #MergeEvent()
                                  ])

#Truth matched commonparticles: 
_kaons = DataOnDemand(Location='Phys/StdMCKaons/Particles')
_pions = DataOnDemand(Location='Phys/StdMCPions/Particles')

#
# MC matching
#

matchD2KKPi = "(mcMatch('[D_s+  ==> K- K+ pi+]CC'))"
#matchKaons = "(mcMatch('[K+]cc'))"
#matchPions = "(mcMatch('[pi+]cc'))"
_d2kkpi = CombineParticles("d2kkpi")
_d2kkpi.DecayDescriptor = "D_s+ -> K- K+ pi+"
#_d2kkpi.DaughtersCuts = { "pi+" : matchPions, "K+" : matchKaons}
_d2kkpi.MotherCut = matchD2KKPi
_d2kkpi.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]

SelD2KKPi = Selection( "SelD2KKPiNew",
                        Algorithm = _d2kkpi,
                        RequiredSelections=[_kaons,_pions])  

SeqD2KKPi = SelectionSequence('MCFilterNew',TopSelection = SelD2KKPi, PostSelectionAlgs=[evtAlgs])

dstWriter = SelDSTWriter("MyDSTWriter",
                         SelectionSequences = [SeqD2KKPi],
                         OutputFileSuffix = 'AfterUnpacking'
                         )

tuple = DecayTreeTuple("out")
tuple.Decay = "[D_s+ -> K- K+ pi+]CC"
#tuple.Inputs = [SeqD2KKPi.outputLocation()]
tuple.Inputs = ['NewEvent/Phys/SelD2KKPi/Particles']
mcTruth = tuple.addTupleTool("TupleToolMCTruth")
tuple.addTupleTool("TupleToolPropertime")

checkPV = CheckPV()

from Configurables import DaVinci
DaVinci().EvtMax = 100
DaVinci().PrintFreq = 1
DaVinci().SkipEvents = 0
DaVinci().DataType = "2012"
DaVinci().HistogramFile = "meta.root"
#DaVinci().ProductionType = "Stripping"
DaVinci().Simulation = True
DaVinci().appendToMainSequence([dstWriter.sequence(), tuple])
DaVinci().TupleFile = "D_s_plus.root"
#DaVinci().UserAlgorithms = [SeqD2KKPi.sequence(), dstWriter.sequence() ]

# Change the column size of Timing table
from Configurables import TimingAuditor, SequencerTimerTool
TimingAuditor().addTool(SequencerTimerTool,name="TIMER")
TimingAuditor().TIMER.NameSize = 60


