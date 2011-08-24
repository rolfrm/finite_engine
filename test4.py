import core
import gfw
import numpy
import physics
import random
from utils import *
random.seed(40)
gameCore = core.Core()
world = GenTestWorld(gameCore)

g1 = core.Player(makeGfxBox(50,50),MakePhysicsBox(50,50,1.0,(0.0,0)))

world[0].Load(gameCore,g1,world[0].Portals[0])
for i in gameCore.GameObjects:
	i.UpdatePos()
gameCore.doMainLoop()


