import physics
import gfw
import time
import sound_system.soundplayer as snd
import shaders
from copy import copy
class Level:
	def __init__(self,portals,ground,others):
		self.Portals = portals
		self.Ground = ground
		self.others = others
	def Load(self,core,player,exitPortal):
		collected = copy(self.Portals)
		collected.extend(self.Ground)
		collected.extend(self.others)
		print "Exit portal:" ,exitPortal.x,exitPortal.y
		player.SetPos(exitPortal.x + 5,exitPortal.y)
		#collected.extend([player])
		core.LoadLevel(collected)
		print "collen:", len(collected)
		core.LoadObject(player)
		core.followIndex = core.GameObjects.index(player)

class GameObject(object):
	def __init__(self,graphicsObject = 0, physicsObject = 0, offset = (0,0)):
		self.Visual = graphicsObject
		self.Body = physicsObject
		self.offset = offset
		self.x = 0
		self.y = 0
		self.a = 0
		self.UpdatePos()
		self.CollisionResponse = 0
		self.KeyEventHandler = 0
		self.MouseEventHandler = 0
	def SetPos(self,x,y):
		
		self.x = x
		self.y = y
		if self.Body is not 0:
			self.Body.SetPosition(physics.Vec2(x,y))
	def UpdatePos(self):
		if self.Body is not 0:
			pos = self.Body.GetPosition()
			self.x = pos.x
			self.y = pos.y
			self.a = self.Body.GetAngle()
	def Draw(self):
		self.Visual.ActivateTextures()
		gfw.Draw(self.x+self.offset[0],self.y+self.offset[1],self.a,self.Visual)
	def OnCollision(self,other,force):
		if self.CollisionResponse is not 0:
			self.CollisionResponse(self,other,force)

class Portal(GameObject):
	def __init__(self,Level, exitPortal,visual,body,core):
		super(Portal,self).__init__(visual,body,(0,0))
		self.Level = Level 
		self.ExitPortal = exitPortal
		self.core = core
		
		self.CollisionResponse = lambda x,y,z: self.ActivateLevel(y)
	def ActivateLevel(self,other):
		if isinstance(other,Player):
			if other.LoadNextLevel == True:
					self.ExitPortal.Level.Load(self.core,other,self.ExitPortal)
		
class Player(GameObject):
	def __init__(self,visual,body):
		super(Player,self).__init__(visual,body,(0,0))
		self.LoadNextLevel = False
		self.KeyEventHandler = lambda x,y,z: self.keyEventHandler(y,z)
	def keyEventHandler(self,a,n):
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
				self.LoadNextLevel = True
		if n == 0:
			if a == gfw.KEY_LCTRL:
				self.LoadNextLevel = False
class Core:
	def __init__(self):
		gfw.Init(600,600,False)
		self.s1 = gfw.Shader(shaders.ObjectLightning[0],shaders.ObjectLightning[1])
		gfw.SetActiveShader(self.s1);
		gfw.Zoom(20,20)
		self.PhysicsCore = physics.Core(100)
		self.PhysicsCore.setGravity(0.0,-0.004)
		self.GameObjects = []
		self.KeyEventHandlers =[]
		self.MouseEventHandlers = []
		self.PhysicsObjectRef = {}
		snd.PAInit()
		self.PA = snd.PASink(1)
		self.nodeplayer = snd.NodePlayer(10)
		self.nodeplayer.Connect(self.PA,0,0)
	def doMainLoop(self):
		i = 0
		while True:
			i += 1
			try:
					
				for item in self.GameObjects:
					if isinstance(item,Player):
							self.s1.SetUniform2f(item.x,item.y,"CameraPosition")	
							print item.x,item.y
					if item.Visual is not 0:
						item.Draw()
					if item.Body is not 0:
						item.UpdatePos()
						
				self.PhysicsCore.Run()
				gfw.Refresh()
				while self.PhysicsCore.CollisionsReady():
					col = self.PhysicsCore.GetNextCollision()
					o1 = col.GetFrom()
					o2 = col.GetTo()
					try:
						col1 = self.PhysicsObjectRef[o1.GetID()]
						col2 = self.PhysicsObjectRef[o2.GetID()]
					except KeyError:
						continue
					col1.OnCollision(col2,col.GetI())					
					col2.OnCollision(col1,col.GetI())
				self.PA.RecursiveUpdate(i)
				mev = gfw.GetMouseEvents()
				kev = gfw.GetKeyEvents()	
				for item in kev:
					for j in self.KeyEventHandlers:
						j.KeyEventHandler(j,item.key,item.action)
			
				for item in mev:
					for j in self.MouseEventHandlers:
						j.MouseEventHandler(j,item.button,item.action)
	
				time.sleep(0.01)
			except KeyboardInterrupt as ke:
				break
	def LoadObject(self,gameObject):
		self.GameObjects.append(gameObject)
		if gameObject.Body is not 0:
			self.PhysicsCore.LoadObject(gameObject.Body)
			self.PhysicsObjectRef[gameObject.Body.GetID()] = gameObject
		if gameObject.MouseEventHandler is not 0:
			self.MouseEventHandlers.append(gameObject)
		if gameObject.KeyEventHandler is not 0:
			self.KeyEventHandlers.append(gameObject)
	def ReloadObject(self,gameObject):
		self.UnloadObject(gameObject)
		self.LoadObject(gameObject)
		
	def UnloadObject(self, gameObject):
		self.GameObjects.remove(gameObject)
		self.PhysicsCore.UnloadObject(gameObject.Body)
		try:
			self.PhysicsObjectRef.pop(gameObject.Body.GetID())
		except KeyError:
			pass
		
		try:
			self.MouseEventHandlers.remove(gameObject)
		except ValueError:
			pass	
		try:
			self.KeyEventHandlers.remove(gameObject)
		except ValueError:
			pass
	def PlaySound(self,node):
		self.nodeplayer.PlayNode(node,1)
	def LoadLevel(self,LevelList):
		for i in self.GameObjects[:]:
			self.UnloadObject(i)
		for i in LevelList:
			self.LoadObject(i)
		print len(self.GameObjects)
