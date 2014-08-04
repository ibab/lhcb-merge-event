from Configurables import GaudiSequencer, NoPIDsParticleMaker, PrTrackAssociator, ChargedProtoParticleMaker, ChargedPP2MC, ChargedProtoParticleAddRichInfo, ChargedProtoCombineDLLsAlg

makeparts = GaudiSequencer('MakeOtherParticles', RootInTES='/Event/NewEvent/')

#assocTracks = PrTrackAssociator('OtherTrackAssociator', RootInTES='/Event/NewEvent/')
#assocTracks.RootOfContainers = 'Rec/Track'
#makeparts.Members.append(assocTracks)
#

#richInfo = ChargedProtoParticleAddRichInfo("AddRichToProtos")
#richInfo.InputRichPIDLocation = 'NewEvent/Rec/Rich/PIDs'
#richInfo.ProtoParticleLocation = 'NewEvent/Rec/ProtoP/OtherProtos'
#makeparts.Members.append(richInfo)
#
#makeDLLs = ChargedProtoCombineDLLsAlg("MakeDLLsForProtos")
#makeDLLs.ProtoParticleLocation = 'NewEvent/Rec/ProtoP/OtherProtos'
#makeparts.Members.append(makeDLLs)

#assoc = ChargedPP2MC('AssocProtos')
#assoc.TrackLocations = [ 'NewEvent/Rec/Track/Best' ]
#assoc.InputData = [ 'NewEvent/Rec/ProtoP/Charged' ]
#assoc.OutputTable = 'NewEvent/Relations/Rec/ProtoP/Charged'
#makeparts.Members.append(assoc)

ppMaker = ChargedProtoParticleMaker('OtherProtoParticles', Output="Phys/OtherProtos", RootInTES='/Event/NewEvent')
makeparts.Members.append(ppMaker)

allPions = NoPIDsParticleMaker('OtherAllPions'
                              , Particle = "pion"
                              , Input = "Rec/ProtoP/Charged"
                              , Output = "Phys/OtherAllPions/Particles"
                              , WriteP2PVRelations = False
                              , InputPrimaryVertices = "Rec/Vertex/Primary"
                              , RootInTES='/Event/NewEvent'
                              )

allKaons = NoPIDsParticleMaker('OtherAllKaons'
                              , Particle = "kaon"
                              , Input = "Rec/ProtoP/Charged"
                              , Output = "Phys/OtherAllKaons/Particles"
                              , WriteP2PVRelations = False
                              , InputPrimaryVertices = "Rec/Vertex/Primary"
                              , RootInTES = "/Event/NewEvent"
                              )

allPions.OutputLevel = 2
allKaons.OutputLevel = 2

makeparts.Members.append(allPions)
#makeparts.Members.append(allKaons)

