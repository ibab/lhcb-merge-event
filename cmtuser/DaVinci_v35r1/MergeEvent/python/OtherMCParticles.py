from Configurables import GaudiSequencer, NoPIDsParticleMaker, PrTrackAssociator, ChargedProtoParticleMaker, ChargedPP2MC, ChargedProtoParticleAddRichInfo, ChargedProtoCombineDLLsAlg

makeparts = GaudiSequencer('MakeOtherParticles')

#assocTracks = PrTrackAssociator('OtherTrackAssociator')
#assocTracks.RootOfContainers = 'NewEvent/Rec/Track'
#makeparts.Members.append(assocTracks)
#
#ppMaker = ChargedProtoParticleMaker('OtherProtoParticles', Inputs=['NewEvent/Rec/Track/Best'], Output='NewEvent/Rec/ProtoP/OtherProtos')
#makeparts.Members.append(ppMaker)

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

#allPions = NoPIDsParticleMaker('OtherAllPions'
#                              , Particle = "pion"
#                              , Input = "NewEvent/Rec/ProtoP/OtherProtos"
#                              , Output = "Phys/OtherAllPions/Particles"
#                              , WriteP2PVRelations = False
#                              )
#
#allKaons = NoPIDsParticleMaker('OtherAllKaons'
#                              , Particle = "kaon"
#                              , Input = "NewEvent/Rec/ProtoP/OtherProtos"
#                              , Output = "Phys/OtherAllKaons/Particles"
#                              , WriteP2PVRelations = False
#                              )

allPions = NoPIDsParticleMaker('OtherAllPions'
                              , Particle = "pion"
                              , Input = "NewEvent/Rec/ProtoP/Charged"
                              , Output = "Phys/OtherAllPions/Particles"
                              , WriteP2PVRelations = False
                              , InputPrimaryVertices = "NewEvent/Rec/Vertex/Primary"
                              )

allKaons = NoPIDsParticleMaker('OtherAllKaons'
                              , Particle = "kaon"
                              , Input = "NewEvent/Rec/ProtoP/Charged"
                              , Output = "Phys/OtherAllKaons/Particles"
                              , WriteP2PVRelations = False
                              , InputPrimaryVertices = "NewEvent/Rec/Vertex/Primary"
                              )

allPions.OutputLevel = 1
allKaons.OutputLevel = 1

makeparts.Members.append(allPions)
makeparts.Members.append(allKaons)

