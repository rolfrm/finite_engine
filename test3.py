import core
import gfw
import numpy
import physics
import random
import Image
from utils import *
random.seed(10)
gameCore = core.Core()
fontpng = Image.open("font1.png")

def MakeTestLevel():
	hmap = GenHeightMap(-100,100,8)
	gos = MakeSurfaceFromHeights(hmap,3.0*3,10)
	return core.Level([],gos,[])
nodes = [ [[-1000.0,0.0,0.0],'cave'],[[1000.0,0.0,0.0],'forest']]
connections = [[0,1]]
for i in range(0,4):
	GenLevel(nodes,connections)	
lvselect = int(random.random()*len(connections))
n1 = nodes[connections[lvselect][0]]
n2 = nodes[connections[lvselect][1]]
hmap = GenHeightMap(n1[0][1],n2[0][1],8)
gos = MakeSurfaceFromHeights(hmap,3.0*8,10)


g1 = core.Player(makeGfxBox(3.5,3.5),MakePhysicsBox(3.5,3.5,1,(0.0,0)))

mainLevel = MakeTestLevel()#core.Level([],gos,[])
level2 = MakeTestLevel()


portal = core.Portal(mainLevel,0,makeGfxBox(4.0,4.0),MakePhysicsBox(4,4,1,(0.0,0)),gameCore)
portal2 = core.Portal(mainLevel,0,makeGfxBox(4.0,4.0),MakePhysicsBox(4,4,1,(0.0,0)),gameCore)
portal3= core.Portal(level2,0,makeGfxBox(4.0,4.0),MakePhysicsBox(4,4,1,(0.0,0)),gameCore)
portal.SetPos(10,-80)
portal2.SetPos(100,-40)
portal3.SetPos(20,-80)
portal.ExitPortal = portal2
portal2.ExitPortal = portal3
portal3.ExitPortal = portal2
mainLevel.Portals.append(portal)
mainLevel.Portals.append(portal2)
level2.Portals.append(portal3)
#gameCore.LoadObject(g1)
#gameCore.followIndex = gameCore.GameObjects.index(g1)
print "Before load:"
print g1.x,g1.y
print portal.x,portal.y

mainLevel.Load(gameCore,g1,portal)
for i in gameCore.GameObjects:
	i.UpdatePos()
	#print i.x, i.y
print "After load:"
print g1.x,g1.y
print portal.x,portal.y
print hmap[0]
gameCore.doMainLoop()

