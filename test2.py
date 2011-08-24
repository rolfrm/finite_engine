import core
import gfw
import numpy
import physics
import random
import Image
from utils import *
gameCore = core.Core()
fontpng = Image.open("font1.png")
nodes = [ [[-1000.0,0.0,0.0],'cave'],[[1000.0,0.0,0.0],'forest']]
connections = [[0,1]]
for i in range(0,4):
	GenLevel(nodes,connections)	
lvselect = int(random.random()*len(connections))
n1 = nodes[connections[lvselect][0]]
n2 = nodes[connections[lvselect][1]]
hmap = GenHeightMap(n1[0][1],n2[0][1],8)
gos = MakeSurfaceFromHeights(hmap,3.0*8,10)
for i in gos:
	i.Body.AdjustPosition(-48,0)
	
mainLevel = core.Level([],gos,[])

g1 = core.GameObject(makeGfxBox(3.0,3.0),MakePhysicsBox(3.5,3.5,1,(0,hmap[0]+30)))

g2 = core.GameObject(makeGfxBox(100,10),MakePhysicsBox(100,190,0,(0,0)))
portal = core.GameObject(makeGfxBox(5.0,5.0),MakePhysicsBox(5.0,1.0,1,(55,hmap[0]+40)),(0,2))
g3 = core.GameObject(makeGfxBox(3.0,3.0),MakePhysicsBox(3.5,3.5,1,(0,hmap[0]+30)))
def OhHai(self,other,force):
	print force
	if force > 0.001:
		gameCore.PlaySound(random.random()*20 - 10)
def PortalFunc(self,other,force):
	if other == g1:
		if g1.newLv:
			print "Next level"
portal.CollisionResponse = PortalFunc
#g1.CollisionResponse = OhHai
#g3.CollisionResponse = OhHai
def printkev(self,a,n):
	if n == 1:
		if a == 284:
			self.Body.AddForce(0,-0.1)
		if a == 283:
			self.Body.AddForce(0,0.1)
		if a == 286:
			self.Body.AddForce(0.1,0)
		if a == 285:
			self.Body.AddForce(-0.1,0)
		if a == gfw.KEY_LCTRL:
			self.newLv = True
	if n == 0:
		if a == gfw.KEY_LCTRL:
			self.newLv = False
g1.KeyEventHandler = printkev
gameCore.LoadLevel(gos)
gameCore.LoadObject(g1)
gameCore.LoadObject(portal)
gameCore.ReloadObject(g3)
gameCore.doMainLoop()
