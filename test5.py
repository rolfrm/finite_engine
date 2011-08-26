import core
import random
from utils import *

def SpeedRun(length):
	lv = []
	nextpos = 0
	lasty = 0
	color = [random.random(),random.random(),random.random()]
	color = [0.5,0.8,0.4]
	for i in range(0,length):
		ground = MakeCompleteObject([-100-i,0, 100+i,0, 100+i,-100,-100-i,-100],color[:],[0,-1, 1,-1, 1,0, 0,0])
		ground.SetPos(nextpos,lasty)
		lv.append(ground)
		nextpos = nextpos + 200+i*i
		lasty = lasty + (random.random()-0.5)*100
		color[0] = color[0]*0.5 + random.random()*0.5
		color[1] = color[1]*0.5 + random.random()*0.5
		color[2] = color[2]*0.5 + random.random()*0.5
	return lv

class SpeedRunLevel(core.LevelBase):
	
	def __init__(self):
		import Image
		self.player = core.Player()
		
		fontImage = Image.open('graphics_framework/font3.png')
		tex = gfw.Texture(fontImage.tostring(),fontImage.size[0],fontImage.size[1],4,1)
		Text = gfw.Text(tex,0,1,0,1,10,10,32)

		Text.SetText("SCORE!")
		self.textrendering = core.GameObject(Text)
		self.textrendering.SetPos(-10,10)
		self.textrendering.IgnoresCamera = True
		
	def LoadLevel(self,gamecore):
		self.GameObjects = SpeedRun(5)
		self.player.SetPos(0,100)
		self.player.Body.setMass(1)
		gamecore.LoadLevel(self.GameObjects)
		gamecore.LoadObject(self.player)
		gamecore.LoadObject(self.textrendering)
		gamecore.Level = self	
		self.GC = gamecore
	def Unload(self):
		self.GC.UnloadAll()
		for i in self.GameObjects:
			i.Delete()
			self.GC.Level = 0
	def Update(self,dt):
		if self.player.y < -1000:
			self.Unload()
			self.LoadLevel(self.GC)

gameCore = core.Core()


level = SpeedRunLevel()
level.LoadLevel(gameCore)
level = 0
gameCore.doMainLoop()
gameCore = 0
#from guppy import hpy
#h = hpy()
#print h.heap()


