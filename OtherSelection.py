
from Configurables import CombineParticles, DecayTreeTuple
from PhysSelPython.Wrappers import Selection, SelectionSequence, DataOnDemand

# Selection for other loop
_otherKaons = DataOnDemand(Location='/Event/NewEvent/Phys/OtherAllKaons/Particles')
_otherPions = DataOnDemand(Location='/Event/NewEvent/Phys/OtherAllPions/Particles')
_otherd2kkpi = CombineParticles("otherd2kkpi", InputPrimaryVertices="/Event/NewEvent/Rec/Vertex/Primary")
_otherd2kkpi.DecayDescriptor = "D_s- -> K- K+ pi-"
_otherd2kkpi.MotherCut = "(mcMatch('D_s-  ==> K- K+ pi-', ['/Event/NewEvent/Relations/NewEvent/Rec/ProtoP/Charged'], '/Event/NewEvent/MC/Particles'))"
_otherd2kkpi.Preambulo = [
    "from LoKiPhysMC.decorators import *",
    "from PartProp.Nodes import CC" ]

selD2KKPiOther = Selection("DsMinusCandidates",
                           Algorithm = _otherd2kkpi,
                           RequiredSelections=[_otherKaons, _otherPions],
                           OutputBranch="NewEvent/Phys")

#selD2KKPiOther.OutputLevel = 1

seqD2KKPiOther = SelectionSequence('MCFilterOther', TopSelection = selD2KKPiOther)

othertuple = DecayTreeTuple("Ds2KKPiTuple", RootInTES='/Event/NewEvent')
othertuple.Decay = "[D_s+ -> K- K+ pi+]CC"
#othertuple.Inputs = [seqD2KKPi.outputLocation()]
othertuple.Inputs = ['Phys/SelD2KKPiOther/Particles']
othertuple.ToolList = []
from Configurables import MCMatchObjP2MCRelator

othertuple.addTupleTool('TupleToolKinematic')
othertuple.addTupleTool('TupleToolPropertime')

mcTruth = othertuple.addTupleTool("TupleToolMCTruth")
mcTruth.IP2MCPAssociatorTypes = ['MCMatchObjP2MCRelator/MyMCMatcher']
mcTruth.addTool(MCMatchObjP2MCRelator, name='MCMatchObjP2MCRelator')
mcTruth.MCMatchObjP2MCRelator.RelTableLocations = ['/Event/NewEvent/Relations/NewEvent/Rec/ProtoP/Charged']

#tuple.addTupleTool("TupleToolPropertime")

