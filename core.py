import physics
import gfw
import time
import sound_system.soundplayer as snd
import shaders
class GameObject:
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
		self.ID = "obj"
		
	def SetPos(self,x,y):
		if self.Body is 0:
			self.x = x
			self.y = y
			
		else:
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
class Core:
	def __init__(self):
		gfw.Init(600,600,False)
		self.s1 = gfw.Shader(shaders.ObjectLightning[0],shaders.ObjectLightning[1])
		gfw.SetActiveShader(self.s1);
		gfw.Zoom(50,50)
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
		self.followIndex = 0
	def doMainLoop(self):
		i = 0
		while True:
			i += 1
			try:
				self.s1.SetUniform2f(self.GameObjects[self.followIndex].x,self.GameObjects[self.followIndex].y,"CameraPosition")		
				for item in self.GameObjects:
					if item.Visual is not 0:
						item.Draw()
					if item.Body is not 0:
						item.UpdatePos()
						
				self.PhysicsCore.Run()
				gfw.Refresh()
				#print "Run"
				while self.PhysicsCore.CollisionsReady():
					col = self.PhysicsCore.GetNextCollision()
					o1 = col.GetFrom()
					o2 = col.GetTo()
					col1 = self.PhysicsObjectRef[o1.GetID()]
					col2 = self.PhysicsObjectRef[o2.GetID()]
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
		try:
			self.GameObjects.remove(gameObject)
		except:
			pass		
		self.PhysicsCore.UnloadObject(gameObject.Body)
		try:
			self.PhysicsObjectRef.pop(gameObject.Body.GetID())
		except:
			pass

		try:
			self.MouseEventHandlers.remove(gameObject)
		except:
			pass
		try:		
			self.KeyEventHandlers.remove(gameObject)
		except:
			pass
	def PlaySound(self,node):
		self.nodeplayer.PlayNode(node,1)
	def LoadLevel(self,LevelList):
		for i in self.GameObjects:
			self.UnloadObject(i)
		for i in LevelList:
			self.LoadObject(i)
