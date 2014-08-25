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

ds_mass = 1968.47

combo_cut = [ 'ASUM(PT) > 1800*MeV' # ASUMPT
            , "AMAXDOCA('LoKi::DistanceCalculator') < 0.5*mm" # AMAXDOCA
            ]
mom_cut = [ 'VFASPF(VCHI2/VDOF) < 10' # VCHI2DOF
          , 'in_range(%s, MM, %s)' % (str(ds_mass-100)+'*MeV', str(ds_mass+100)+'*MeV')
          ]

daug_cut = [ 'TRCHI2DOF < 3'
           , 'PT > 100*MeV'
           , 'P > 100*MeV'
           , 'MIPCHI2DV(PRIMARY) > 4' # MIPCHI2DV
           , 'TRGHP < 0.4'
           ]

combo_cut = ' & '.join(map(lambda x: '(' + x + ')', combo_cut))
mom_cut = ' & '.join(map(lambda x: '(' + x + ')', mom_cut))
daug_cut = ' & '.join(map(lambda x: '(' + x + ')', daug_cut))

print("CombinationCut = " + combo_cut)
print("MotherCut = " + mom_cut)
print("DaughtersCut = " + mom_cut)

# Selection for other loop
_otherKaons = DataOnDemand(Location='/Event/NewEvent/Phys/OtherAllKaons/Particles')
_otherPions = DataOnDemand(Location='/Event/NewEvent/Phys/OtherAllPions/Particles')
_otherd2kkpi = CombineParticles("otherd2kkpi")
_otherd2kkpi.DecayDescriptor = "D_s- -> K- K+ pi-"
#_otherd2kkpi.MotherCut = "(mcMatch('[D_s+  ==> K- K+ pi+]CC'))"
_otherd2kkpi.CombinationCut = combo_cut
_otherd2kkpi.MotherCut = mom_cut
_otherd2kkpi.DaughtersCuts = { "K-": daug_cut,
                               "K+": daug_cut,
                               "pi-": daug_cut,
                               "pi+": daug_cut,
                             }
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

tuple = DecayTreeTuple("Ds2KKPiTuple")
tuple.Decay = "[D_s+ -> K- K+ pi+]CC"
#tuple.Inputs = [seqD2KKPi.outputLocation()]
tuple.Inputs = ['Phys/SelD2KKPiOther/Particles']
#mcTruth = tuple.addTupleTool("TupleToolMCTruth")
tuple.addTupleTool("TupleToolPropertime")

evtAlgs = GaudiSequencer("EventAlgs",
                         Members=[RegisterAddr(AddressesFile='eventaddr.txt', OutputLevel=DEBUG),
                                  #MergeEvent(),
                                  makeparts,
                                  seqD2KKPiOther.sequence(),
                                  expl,
                                  tuple
                                  ])

from Configurables import DaVinci
DaVinci().EvtMax = 100
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

