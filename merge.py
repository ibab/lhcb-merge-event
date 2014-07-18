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

unpack = UnpackTrack("OtherUnpack")
unpack.InputName = '/Event/NewEvent/pRec'
unpack.OutputName = 'NewEvent/Rec'
unpack.OutputLevel = DEBUG

# Selection for this loop
_kaons = DataOnDemand(Location='Phys/StdMCKaons/Particles')
_pions = DataOnDemand(Location='Phys/StdMCPions/Particles')

#matchD2KKPi = "(mcMatch('[D_s+  ==> K- K+ pi+]CC'))"
matchD2KKPi = "(mcMatch('[D_s+  ==> K- K+ pi+]CC'))"
_d2kkpi = CombineParticles("d2kkpi")
_d2kkpi.DecayDescriptor = "D_s+ -> K- K+ pi+"
_d2kkpi.MotherCut = matchD2KKPi
_d2kkpi.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]

selD2KKPi = Selection( "selD2KKPiNew",
                        Algorithm = _d2kkpi,
                        RequiredSelections=[_kaons,_pions])  

tuple = DecayTreeTuple("Ds2KKPiTuple")
tuple.Decay = "[D_s+ -> K- K+ pi+]CC"
#tuple.Inputs = [seqD2KKPi.outputLocation()]
tuple.Inputs = ['Phys/SelD2KKPiOther/Particles']
#mcTruth = tuple.addTupleTool("TupleToolMCTruth")
tuple.addTupleTool("TupleToolPropertime")

# Selection for other loop
_otherKaons = DataOnDemand(Location='Phys/OtherAllKaons/Particles')
_otherPions = DataOnDemand(Location='Phys/OtherAllPions/Particles')
_otherd2kkpi = CombineParticles("otherd2kkpi")
_otherd2kkpi.DecayDescriptor = "D_s- -> K- K+ pi-"
_otherd2kkpi.MotherCut = "ALL"

selD2KKPiOther = Selection("SelD2KKPiOther",
                           Algorithm = _otherd2kkpi,
                           RequiredSelections=[_otherKaons,_otherPions])  

selD2KKPiOther.OutputLevel = 1

seqD2KKPiOther = SelectionSequence('MCFilterOther', TopSelection = selD2KKPiOther)

# Sequence that copies the other event and reruns the selection
evtAlgs = GaudiSequencer("EventAlgs",
                         Members=[RegisterAddr(AddressesFile='eventaddr.txt', OutputLevel=DEBUG),
                                  unpack,
                                  #MergeEvent(),
                                  makeparts,
                                  seqD2KKPiOther.sequence(),
                                  tuple
                                  ])

seqD2KKPi = SelectionSequence('MCFilterNew',TopSelection = selD2KKPi, PostSelectionAlgs=[evtAlgs])

dstWriter = SelDSTWriter("MyDSTWriter",
                         SelectionSequences = [seqD2KKPi],
                         OutputFileSuffix = 'AfterUnpacking'
                         )


from Configurables import DaVinci
DaVinci().EvtMax = 100
DaVinci().PrintFreq = 1
DaVinci().SkipEvents = 0
DaVinci().DataType = "2012"
DaVinci().HistogramFile = "meta.root"
#DaVinci().ProductionType = "Stripping"
DaVinci().Simulation = True
#DaVinci().appendToMainSequence([seqD2KKPi.sequence()])
DaVinci().TupleFile = "D_s_plus.root"
#DaVinci().UserAlgorithms = [seqD2KKPi.sequence()]
DaVinci().UserAlgorithms = [dstWriter.sequence()]


# Change the column size of Timing table
from Configurables import TimingAuditor, SequencerTimerTool
TimingAuditor().addTool(SequencerTimerTool,name="TIMER")
TimingAuditor().TIMER.NameSize = 60


