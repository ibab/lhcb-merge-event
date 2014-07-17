########################################################################
from Gaudi.Configuration import *
from Configurables import DaVinci 
from Configurables import GaudiSequencer, CombineParticles, FilterDesktop, DecayTreeTuple, CheckPV, SelDSTWriter
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand
from CommonMCParticles import StandardMCKaons, StandardMCPions
from DecayTreeTuple.Configuration import *
from Configurables import DumpAddr

GaudiPersistency()
importOptions("data_local.py")

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
_d2kkpi.DecayDescriptor = "D_s- -> K- K+ pi-"
#_d2kkpi.DaughtersCuts = { "pi+" : matchPions, "K+" : matchKaons}
_d2kkpi.MotherCut = matchD2KKPi
_d2kkpi.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]

SelD2KKPi = Selection( "SelD2KKPi",
                        Algorithm = _d2kkpi,
                        RequiredSelections=[_kaons,_pions])  

dumpAlg = DumpAddr(OutputFile='trackaddr.txt', ObjectPath='/Event')
SeqD2KKPi = SelectionSequence('MCFilter',TopSelection = SelD2KKPi, PostSelectionAlgs=[dumpAlg])

tuple = DecayTreeTuple("out")
tuple.Decay = "[D_s+ -> K- K+ pi+]CC"
tuple.Inputs = [SeqD2KKPi.outputLocation()]
mcTruth = tuple.addTupleTool("TupleToolMCTruth")
tuple.addTupleTool("TupleToolPropertime")

checkPV = CheckPV()

dstWriter = SelDSTWriter("MyDSTWriter",
                         SelectionSequences = [SeqD2KKPi],
                         OutputFileSuffix = 'EXTRA'
                         )

from Gaudi.Configuration import *

from Configurables import DaVinci
DaVinci().EvtMax = 1000
DaVinci().PrintFreq = 10
DaVinci().SkipEvents = 0
DaVinci().DataType = "2012"
DaVinci().HistogramFile = "meta.root"
DaVinci().Simulation = True
DaVinci().appendToMainSequence([dstWriter.sequence(), tuple])
DaVinci().TupleFile = "D_s_minus.root"

# Change the column size of Timing table
from Configurables import TimingAuditor, SequencerTimerTool
TimingAuditor().addTool(SequencerTimerTool,name="TIMER")
TimingAuditor().TIMER.NameSize = 60

