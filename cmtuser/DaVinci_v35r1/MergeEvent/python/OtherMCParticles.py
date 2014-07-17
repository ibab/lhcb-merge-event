from Configurables import GaudiSequencer, NoPIDsParticleMaker

makeparts = GaudiSequencer('makeparts')

allPions = NoPIDsParticleMaker('OtherAllPions'
                              , Particle = "pion"
                              , Input = "NewEvent/Rec/ProtoP/Charged"
                              , Output = "Phys/OtherAllPions/Particles"
                              , WriteP2PVRelations = False
                              )

allKaons = NoPIDsParticleMaker('OtherAllPions'
                              , Particle = "kaon"
                              , Input = "NewEvent/Rec/ProtoP/Charged"
                              , Output = "Phys/OtherAllKaons/Particles"
                              , WriteP2PVRelations = False
                              )

makeparts.Members.append(allPions)
makeparts.Members.append(allKaons)

