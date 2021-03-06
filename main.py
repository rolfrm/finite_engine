import core
import random
from utils import *
import levelgen
from physics import Vec2
fontImage = Image.open('graphics_framework/font3.png')
doorTex = Image.open('door.png')
class ActivateAble(core.GameObject):
	def __init__(self,visual,body,st = "ActivateAble"):
		self.ID = st
		super(ActivateAble,self).__init__(visual,body)
	def Activate(self,activator):
		print "not implemented"
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
		self.SetPos(0,0)
	def Col(self,other):
		if isinstance(other,Portal):
			pass
		#elif isinstance(other,PickUpable):
		#	self.gameCore.UnloadObject(other)
		#	self.Inventory.append(other)
		else:
			self.AllowJump = True
	def keyEventHandler(self,a,n):
		if a == 286:
			self.walkLeft = n
		if a == 285:
			self.walkRight = n
		
		if n == 1:
			#if a == 284:
				#self.Body.AddForce(0,-1)
			if a == gfw.KEY_SPACE:
				if self.AllowJump is True:
					self.Body.AddForce(0,10)
			
			if a == gfw.KEY_LCTRL:
				self.LoadNextLevel = True
				if self.Closest is not None:
					self.Closest.Activate(self)
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
		gos = self.gameCore.GameObjects
		dist = 50
		obj = None
		s = ""
		for i in gos:
			dx = self.x - i.x
			dy = self.y - i.y
			dxy = (dx**2 + dy**2)**0.5
						
			if dxy < dist and isinstance(i,ActivateAble):
				s += i.ID +"\n"
				obj = i
				dist = dxy
		self.Closest = obj
		if obj is not None:
			self.Text.SetText(obj.ID)
		else:
			self.Text.SetText("")

def SpeedRun(length):
	lv = []
	nextpos = 0
	lasty = 0
	color = [random.random(),random.random(),random.random()]
	color = [0.5,0.8,0.4]
	for i in range(0,length):
		ground = MakeCompleteObject([-100-i,0, 100+i,0, 100+i,-100,-100-i,-100],color[:],[0,-1, 1,-1, 1,0, 0,0])
		ground.SetPos(nextpos,lasty)
		#ground.Body.setMass(100000)
		def upf(obj,dt):
			obj.Body.AddForce(0,100000*0.4)
		ground.UpdateFunc = upf
		lv.append(ground)
		nextpos = nextpos + 200+i*i
		lasty = lasty + (random.random()-0.5)*100
		color[0] = color[0]*0.5 + random.random()*0.5
		color[1] = color[1]*0.5 + random.random()*0.5
		color[2] = color[2]*0.5 + random.random()*0.5
	return lv

class PickUpable(ActivateAble):
	def __init__(self,gameobject):
		super(PickUpable,self).__init__(gameobject.Visual,gameobject.Body,"Pickupable, take[ctrl]")
		self.CollisionResponse = (lambda x,other,y: self.Col(other))
		self.usage = 0
	def Use(self):
		other.gameCore.UnloadObject(self)
		other.Inventory.append(self)
	def Activate(self,other):
		other.gameCore.UnloadObject(self)
		other.Inventory.append(self)
	def SetUseFunction(self,func):
		self.usage = func
	def Col(self,other):
		pass
class SpeedRunLevel(core.LevelBase):
	
	def __init__(self,gameCore,player,mainscreen):
		super(SpeedRunLevel,self).__init__()
		import Image
		self.player = player
		self.GC = gameCore
		self.MainScreen = mainscreen
		fontImage = LoadImageAsTexture('graphics_framework/font3.png')
		Text = gfw.Text(fontImage,0,1,0,1,10,10,32,20)
		
		self.textrendering = core.GameObject(Text)
		self.textrendering.SetPos(-180,150)
		self.textrendering.IgnoresCamera = True
		
		RunText = gfw.Text(fontImage,0,1,0,1,10,10,32,100)
		self.runtext = core.GameObject(RunText)
		self.runtext.SetPos(0,100)
		self.runtext.z = 1
		RunText.SetText("RUN!")
		self.runtext.parralax = 0.5
		
	def LoadLevel(self):
		self.ClearItems()
		gfw.SetBGColor(0.3,0.4,0.5)
		self.GameObjects = SpeedRun(200)
		
		self.player.SetPos(0,100)
		self.player.Body.setMass(1)
		portal = Portal(self.MainScreen,gameCore)
		portal.SetPos(-50,100)
		self.AddItems(self.GameObjects+[self.player,self.textrendering,portal,self.runtext])
		self.GC.LoadLevel(self)
		
	def Update(self,dt):
		#print self.player.x,self.player.y
		print self.player.Body.GetVelocity().x
		self.textrendering.Visual.SetText("Distance: " + str(int(self.player.x)))
		self.GC.SetCamera(self.player.x,self.player.y)
		if self.player.y < -1000:
			self.LoadLevel()

class OutsideDemo(core.LevelBase):
	def __init__(self,gameCore,player,mainScreen):
		super(OutsideDemo,self).__init__()
		self.player = player
		self.mainScreen = mainScreen
		self.GC = gameCore
		self.SideRock = MakeCompleteObject([0,0, -100,100, -80,130, -110,150, -115,200,-100,250, 0,200,50,100,50,50,30,0],[0.1,0.1,0.1]) 
		self.SideRock.SetPos(-200,100)
		self.SideRock.Visual.SetDrawType(0x0009)
		self.Floor = MakeCompleteObject([0,0, 500,10, 700,-50, 900,50, 900,-1000,0 ,-1000],[0.4,0.6,0.4])
		self.Floor.SetPos(-50,-500)
		self.Floor.Visual.SetDrawType(0x0009)
		self.AddItems([self.SideRock,self.Floor,self.player])
	def LoadLevel(self):
		gfw.SetBGColor(0.4,0.6,0.9)
		self.player.SetPos(0,100)
		self.GC.LoadLevel(self)
	def Update(self,dt):
		self.GC.SetCamera(self.player.x,self.player.y)
class Portal(ActivateAble):
	def Activate(self,other):
		if isinstance(other,Player):
			self.exitLevel.LoadLevel()
	def __init__(self,exitLevel,gameCore,to ="..."):
		self.exitLevel = exitLevel
		self.GC = gameCore
		porttex = LoadImageAsTexture('door.png')
		go = MakeCompleteObject([0,0,64,0,64,96,0,96],[0.1,0.1,0.2],[0,1, 1,1, 1,0, 0,0])

		go.Body.setMass(0)
		super(Portal,self).__init__(go.Visual,0,"Door to " + to)
		
class TestTrack(core.LevelBase):
	def __init__(self,gameCore,player,mainScreen):
		super(TestTrack,self).__init__()
		self.GC = gameCore
		self.player = player
		self.mainScreen = mainScreen
		tex = LoadImageAsTexture("grasstex2.png")
		tex2 = LoadImageAsTexture("rock.png")
		
		def MakeTree(x,y):
			treetex = LoadImageAsTexture("tree.png")
			tree = core.GameObject(MakePolygon([-16,0,-16,64,16,64,16,0],[],[],[0,1, 0,0.01, 1,0.01,1,1]),0,x,y)
			tree.Visual.AddTexture(treetex,0)
			tree.parralax = 0.8
			tree.z = 1
			return tree
		
		def BlockHandler(blocks):
			for i in blocks:
				i.Visual.AddTexture(tex,0)
				
				
		def BGBlockHandler(blocks):
			for i in blocks:
				i.Visual.AddTexture(tex2,0)
				i.parralax = 0.8
				i.Body = 0
				i.z = 2
		def Extra(surfList):
			out = []
			for i in range(int(surfList[0].x),int(surfList[-1].x),10):
				if random.random() < 0.6:
					continue
				x = i
				x,y = levelgen.GetObjPosition(surfList,x)
				out.append(MakeTree(x,y))
			return out
		self.Floor = levelgen.MakeChunk([Vec2(-6200,500),Vec2(-4200,1500),Vec2(-4000,-100),Vec2(4090,-100),Vec2(4200,1500),Vec2(6200,500)],levelgen.InterPolataur,0,BlockHandler)
		self.BG = levelgen.MakeChunk([Vec2(-6200,500),Vec2(-4200,1500),Vec2(-4000,-100),Vec2(-200,-100),Vec2(0,-100),Vec2(200,-100),Vec2(4090,-100),Vec2(4200,1500),Vec2(6200,500)],levelgen.InterPolataur,Extra,BGBlockHandler)
		self.AddItems(self.Floor + [self.player] + self.BG)
	def LoadLevel(self):
		self.GC.LoadLevel(self)
	def Update(self,dt):
		self.GC.SetCamera(self.player.x,self.player.y)	
	
class MainScreen(core.LevelBase):
	def __init__(self,gameCore):
		super(MainScreen,self).__init__()
		self.GC = gameCore
		self.player = Player(self.GC)
		self.Portal = Portal(SpeedRunLevel(self.GC,self.player,self),gameCore, "The running grounds")
		self.Portal.SetPos(-120,50)
		self.Portal.parralax = 0.9
		self.OutsideDoor = Portal(TestTrack(self.GC,self.player,self),gameCore, "outside.")
		self.OutsideDoor.SetPos(-20,50)
		self.OutsideDoor.parralax = 0.8
		
		self.player.Body.setMass(1)
		self.Portal.z = 3
		self.floor = MakeCompleteObject([-200,0, 200,0, 200,-100,-200,-100],[0.2,0.2,0.4],[0,-1, 1,-1, 1,0, 0,0])
		#self.floor.SetPos(0,-100)
		self.floor.z = 4
		self.item = PickUpable(MakeCompleteObject([0,0,10,0,10,10,0,10],[0.5,0.5,1],[0,-1, 1,-1, 1,0, 0,0]))
		self.item.SetPos(0,60)
		self.item.Body.setMass(1)
		self.wall = core.GameObject(MakePolygon([-200,-100, -200,200, 200,200, 200,-100],[],[0.1,0.1,0.3],[]))
		self.wall.z = 5
		#self.wall.parralax = 0.5
		self.window = core.GameObject(makeGfxBox(50,50,[1,1,1],LoadImageAsTexture("window.png")))
		self.window.SetPos(80,80)
		self.window.z = 4
		self.window.parralax = 0.9
		tex = gfw.Texture(fontImage.tostring(),fontImage.size[0],fontImage.size[1],4,1)
		Text = gfw.Text(tex,0,1,0,1,10,10,32,20)
		Text.SetText("HOME")
		
		self.textrendering = core.GameObject(Text)
		self.textrendering.z = 1
		self.textrendering.SetPos(-200,200)
		
		self.AddItems([self.player,self.floor,self.Portal,self.item,self.textrendering,self.wall,self.window,self.OutsideDoor])
	def LoadLevel(self):
		gfw.SetBGColor(0,0,0)
		self.player.SetPos(100,100)
		self.player.z =-1
		self.GC.LoadLevel(self)
		self.GC.ZSort()
	def Update(self,dt):
		self.GC.SetCamera(self.player.x,self.player.y)

gameCore = core.Core()

level = MainScreen(gameCore)
level.LoadLevel()
gameCore.doMainLoop()
gameCore.UnloadAll()


