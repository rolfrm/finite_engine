import core
import random
from utils import *
class Player(core.GameObject):
	def __init__(self):
		
		self.Body = [makeGfxBox(10,10,[1,1,1]),[0,0],None]
		self.LArm = [makeGfxBox(5,5,[1,1,1]),[-10,0],[0]]
		self.RArm = [makeGfxBox(5,5,[1,1,1]),[10,0],[0]]
		self.LArmBegin = [0,0]
		self.RArmBegin = [0,0]
		self.LFoot =[makeGfxBox(5,5,[1,1,1]),[0,-10],[0]]
		self.LFootBegin = [0,-10]
		self.RFoot = [makeGfxBox(5,5,[1,1,1]),[0,-10],[0]]
		self.RFootBegin = [0,-10]
		self.Head = [makeGfxBox(7,7,[1,1,1]),[0,10],[0]]
		self.HeadBegin = [0,10]
		self.LEye = [makeGfxBox(2.0,3.0,[0,0,0]),[2,0],[5]]
		self.REye = [makeGfxBox(2.0,3.0,[0,0,0]),[-2,0],[5]]
		obj = MakeCompleteObject([-5.0,-28, -5.0,10.0, 5.0,10.0,5.0,-28.0])
		super(Player,self).__init__([self.Body,self.LArm,self.RArm,self.LFoot,self.RFoot,self.Head,self.LEye,self.REye],obj.Body,(0,0))
		self.t = 0
		
		self.LoadNextLevel = False
		self.KeyEventHandler = lambda x,y,z: self.keyEventHandler(y,z)
		self.CollisionResponse = lambda x,y,z: self.Col(y)
		self.walkLeft = 0
		self.walkRight = 0
		self.AllowJump = False
	def Col(self,other):
		self.AllowJump = True
	def keyEventHandler(self,a,n):
		if a == 286:
			self.walkLeft = n
		if a == 285:
			self.walkRight = n
		
		if n == 1:
			#if a == 284:
				#self.Body.AddForce(0,-1)
			if a == 283:
				if self.AllowJump is True:
					self.Body.AddForce(0,10)
			
			if a == gfw.KEY_LCTRL:
				self.LoadNextLevel = True
		if n == 0:
			if a == gfw.KEY_LCTRL:
				self.LoadNextLevel = False
	def Update(self,dt):
		self.t += dt
		#self.a = self.t
		self.Body.AddForce(float(self.walkLeft-self.walkRight)/5,0)
		self.Head[1][0] = self.HeadBegin[0] + math.sin(self.t)
		self.LFoot[1][0] = self.LFootBegin[0]+ math.sin(self.x/20)*10
		self.RFoot[1][0] = self.RFootBegin[0]+ math.sin(self.x/20 + math.pi)*10
		self.LFoot[1][1] = self.LFootBegin[1]+ math.cos(self.x/20)*10
		self.RFoot[1][1] = self.RFootBegin[1]+ math.cos(self.x/20 + math.pi)*10
		
		self.LArm[1][0] = self.LArmBegin[0]+ math.sin(self.x/20)*10
		self.RArm[1][0] = self.RArmBegin[0]+ math.sin(self.x/20 + math.pi)*10
		self.Head[1][0] = -self.Body.GetVelocity().GetNorm2()/10
		self.Body.setInertia(0)
		self.AllowJump = False
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
		self.player = Player()
		
		fontImage = Image.open('graphics_framework/font3.png')
		tex = gfw.Texture(fontImage.tostring(),fontImage.size[0],fontImage.size[1],4,1)
		Text = gfw.Text(tex,0,1,0,1,10,10,32)

		Text.SetText("SCORE!")
		self.textrendering = core.GameObject(Text)
		self.textrendering.SetPos(-10,15)
		self.textrendering.IgnoresCamera = True
		
	def LoadLevel(self,gamecore):
		self.GameObjects = SpeedRun(100)
		self.player.SetPos(0,100)
		self.player.Body.setMass(1)
		gamecore.LoadLevel(self.GameObjects)
		gamecore.LoadObject(self.player)
		gamecore.LoadObject(self.textrendering)
		gamecore.Level = self	
		self.GC = gamecore

	def Unload(self):
		self.GC.UnloadAll()

	def Update(self,dt):
		self.textrendering.Visual.SetText("Distance: " + str(self.player.x))
		gameCore.SetCamera(self.player.x,self.player.y)
		if self.player.y < -1000:
			self.Unload()
			self.LoadLevel(self.GC)

gameCore = core.Core()


level = SpeedRunLevel()
level.LoadLevel(gameCore)
level = 0
gameCore.doMainLoop()
gameCore.UnloadAll()


