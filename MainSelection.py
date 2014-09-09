
from Configurables import CombineParticles, DecayTreeTuple
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand

#Truth matched commonparticles: 
_mainkaons = DataOnDemand(Location='Phys/StdMCKaons/Particles')
_mainpions = DataOnDemand(Location='Phys/StdMCPions/Particles')

#
# MC matching
#

matchD2KKPi = "(mcMatch('D_s+  ==> K- K+ pi+'))"
#matchKaons = "(mcMatch('[K+]cc'))"
#matchPions = "(mcMatch('[pi+]cc'))"

_maind2kkpi = CombineParticles("d2kkpiMain")
_maind2kkpi.DecayDescriptor = "D_s+ -> K+ K- pi+"
#_d2kkpi.DaughtersCuts = { "pi+" : matchPions, "K+" : matchKaons}
_maind2kkpi.MotherCut = matchD2KKPi
_maind2kkpi.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]

selD2KKPiMain = Selection( "DsPlusCandidates",
                            Algorithm = _maind2kkpi,
                            RequiredSelections=[_mainkaons, _mainpions])  

seqD2KKPiMain = SelectionSequence('MCFilterMain',TopSelection = selD2KKPiMain)

maintuple = DecayTreeTuple("out")
maintuple.Decay = "[D_s+ -> K- K+ pi+]CC"
maintuple.Inputs = [seqD2KKPiMain.outputLocation()]
mcTruth = maintuple.addTupleTool("TupleToolMCTruth")
maintuple.addTupleTool("TupleToolPropertime")

