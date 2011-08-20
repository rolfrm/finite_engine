import core
import gfw
import numpy
import physics
import random
import Image
from utils import *
gameCore = core.Core()
fontpng = Image.open("font1.png")

#gos = MakeSurfaceFromHeights([100,2,3,4,0,2,4,-10,0,0,0,0,10,10,10,0,0,0],3,100)
gos = MakeSurfaceFromHeights([10,0,1,0,2,0,1,0,3,-1,0,1,0],3,10)
for i in gos:
	gameCore.LoadObject(i)
fonttex = gfw.Texture(fontpng.tostring(),800,600,4,1)

textobj = gfw.Text(fonttex, 0,1,0,1,1,80,32);
textobj.SetText("Oh hai!")
textobj.Draw()

g1 = core.GameObject(makeGfxBox(3,3),MakePhysicsBox(2,2,1,(0,30)))
g2 = core.GameObject(makeGfxBox(100,10),MakePhysicsBox(100,10,0,(0,0)))
g2.Visual.AddTexture(fonttex,0)
g3 = core.GameObject(makeGfxBox(3,3),MakePhysicsBox(2,2,1,(5,30)))
def OhHai(self,other,force):
	if force > 0.01:
		#self.Body.AddForce(0,random.random()/10.0 + 0.2)
		#gameCore.ReloadObject(self)
		gameCore.PlaySound(random.random()*20 - 10)
g1.CollisionResponse = OhHai
g3.CollisionResponse = OhHai
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
g1.KeyEventHandler = printkev

#for i in range(-2,10):
#	g = core.GameObject(makeGfxBox(3,3),MakePhysicsBox(3,3,1,(0+ i*3,15)))
#	g.CollisionResponse = OhHai
#	gameCore.LoadObject(g)

gameCore.LoadObject(g1)
#gameCore.LoadObject(g2)
gameCore.LoadObject(g3)
gameCore.ReloadObject(g3)
gameCore.doMainLoop()
