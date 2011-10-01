import core
import utils
from utils import makeGfxBox
from utils import MakeCompleteObject
import game_object as GameObject
import gfw
import Image
import math
import random
import physics
fontImage = Image.open('graphics_framework/font3.png')
class Player(core.GameObject):
	def __init__(self,gameCore):
		self.gameCore = gameCore
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
		obj.Body.setMass(1)
		tex = gfw.Texture(fontImage.tostring(),fontImage.size[0],fontImage.size[1],4,1)
		self.Text = gfw.Text(tex,0,1,0,1,10,10,32,15)
		
		self.textrendering = core.GameObject(self.Text)
		self.textrendering.SetPos(-180,150)
		self.textrendering.IgnoresCamera = False
		self.TextBox =[self.Text,[-200,-200],[0]]
		self.Text.SetText("lolololol")
		super(Player,self).__init__([self.Body,self.LArm,self.RArm,self.LFoot,self.RFoot,self.Head,self.LEye,self.REye,self.TextBox],obj.Body,0,0)
		self.t = 0
		self.Closest = None
		self.LoadNextLevel = False
		self.KeyEventHandler = lambda x,y,z: self.keyEventHandler(y,z)
		self.CollisionResponse = lambda x,y,z: self.Col(y)
		self.walkLeft = 0
		self.walkRight = 0
		self.AllowJump = False
		self.Inventory = []
	def Col(self,other):
		self.AllowJump = True
	
	def keyEventHandler(self,a,n):
		if a == 286:
			self.walkLeft = n
		if a == 285:
			self.walkRight = n

		print a,n
		if n == 1:
			#if a == 284:
				#self.Body.AddForce(0,-1)
			if a == gfw.KEY_SPACE:
				if self.AllowJump is True:
					self.gameCore.PlaySound(random.random()*10)
					self.Body.AddForce(0,10)
			
			if a == gfw.KEY_LCTRL:
				self.LoadNextLevel = True
				if self.Closest is not None:
					self.Closest.Activate(self)
		if n == 0:
			if a == gfw.KEY_LCTRL:
				self.LoadNextLevel = False
	def Update(self,dt):
		self.gameCore.Camera = [self.x,self.y]
		#print self.Body.GetMass()
		self.t += dt
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
		
		self.Text.SetText("")


class MissileBox(core.GameObject):
	def __init__(self,x,y,target):
		self.target = target
		box = utils.MakeCompleteObject([-10,-10,10,-10,10,10,-10,10],[1,0,0],[])
		box.Body.setMass(1)
		super(MissileBox,self).__init__(box.Visual,box.Body,x,y)
		targetVector = physics.Vec2(self.target.x-self.x,self.target.y-self.y)
		targetVector.Print()
		targetVector = targetVector/ targetVector.GetNorm2()
		targetVector.Print()
		targetVector = targetVector * 20
		self.Body.AddForce(targetVector.x,targetVector.y)
	def Update(self,dt):
		pass
class Boss(core.GameObject):
	def __init__(self,gameCore,player):
		self.GC = gameCore
		self.player = player
		hand = utils.MakePolygon([0,0, 20,-20, 30,0, 40,-20,60,0],[0,1,2,2,3,4],[],[])
		hand.SetDrawType(4)
		body = utils.MakePolygon([-100,0,0,-200,100,0],[],[0,0,1,0,0,1,0,0,1],[])
		body.SetDrawType(4)
		head = utils.MakePolygon([-20,-60,0,0,20,-60],[],[0,0,1,0,0,1,0,0,1],[])
		head.SetDrawType(4)
		eye = utils.MakePolygon([-5,0,0,-5,5,0],[],[1,0,0,1,0,0,1,0,0],[])
		eye.SetDrawType(4)
		self.lhand = [hand,[-100,0],[0]]
		self.body = [body,[0,0],None]
		self.head = [head,[0,70],[0]]
		self.leye = [eye,[-10,-50],[1]]
		self.reye = [eye,[10,-50],[1]]
		self.vis = [self.body,self.head,self.leye,self.reye,self.lhand,[hand,[100,0],[0]]]
		super(Boss,self).__init__(self.vis,0,0,0)
		self.t = 0
		self.z = 5
		self.attack = None
		self.doAttack = True
		self.LastSpawn = 0
	def Update(self,dt):
		self.t += dt
		print self.t % 100
		if self.t-self.LastSpawn > 10:
			self.LastSpawn=self.t
			newbox = MissileBox(math.sin(self.t*2)*20 - 200 + self.body[1][0]+self.x, self.body[1][1]+self.y,self.player)
			newbox.Body.setFriction(0.4)
			self.GC.LoadObject(newbox)
		if self.t % 20 < 10 and self.attack is None:
			self.attack = 0
			
			
		if self.attack is not None:
			self.attack += dt*20
			self.lhand[1] = [math.sin(self.t*2)*20 - 200,-self.attack]
			
			if self.attack > 250:
				self.lhand[1] = [math.sin(self.t*2)*20 - 200,0]
				self.attack = None
		self.body[1] = [math.sin(self.t)*5,math.cos(self.t)*5]
		
gameCore = core.Core()

player = Player(gameCore)
player.SetPos(0,100)
player.Body.setFriction(0.4)
platform = utils.MakeCompleteObject([0,0, 1000,0, 1000,-100, 0,-100],[1,1,1, 1,1,1, 1,1,1, 1,1,1])
platform.Visual.SetDrawType(2)
platform.SetPos(0,0)
platform.Body.setFriction(0.4)
boss = Boss(gameCore,player)
boss.z = 100
boss.SetPos(300,300)

gameCore.PlaySound(0)
gameCore.LoadObject(player)
gameCore.LoadObject(platform)
gameCore.LoadObject(boss)
gameCore.ZSort()
gameCore.doMainLoop()
