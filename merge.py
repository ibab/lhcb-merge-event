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

evtAlgs = GaudiSequencer("EventAlgs",
                         Members=[RegisterAddr(AddressesFile='eventaddr.txt', OutputLevel=DEBUG),
                                  MergeEvent(),
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

# Change the column size of Timing table
from Configurables import TimingAuditor, SequencerTimerTool
TimingAuditor().addTool(SequencerTimerTool,name="TIMER")
TimingAuditor().TIMER.NameSize = 60

