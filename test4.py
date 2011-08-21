import core
import gfw
import numpy
import physics
import random
import Image
from utils import *
random.seed(40)
gameCore = core.Core()
fontpng = Image.open("font1.png")
world = GenTestWorld(gameCore)
g1 = core.Player(makeGfxBox(3.5,3.5),MakePhysicsBox(3.5,3.5,1,(0.0,0)))

world[0].Load(gameCore,g1,world[0].Portals[0])
for i in gameCore.GameObjects:
	i.UpdatePos()
gameCore.doMainLoop()


