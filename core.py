import physics
import gfw
import time
import sound_system.soundplayer as snd
import shaders
from game_object import *
class Core:
	def __init__(self):
		gfw.Init(800,800,False,2)
		self.s1 = gfw.Shader(shaders.ObjectLightning[0],shaders.ObjectLightning[1])
		gfw.SetActiveShader(self.s1);
		gfw.Zoom(400,400)
		self.PhysicsCore = physics.Core(100)
		self.PhysicsCore.setGravity(0.0,-0.4)
		self.GameObjects = []
		self.KeyEventHandlers =[]
		self.MouseEventHandlers = []
		self.PhysicsObjectRef = {}
		snd.PAInit()
		self.PA = snd.PASink(1)
		self.nodeplayer = snd.NodePlayer(10)
		self.nodeplayer.Connect(self.PA,0,0)
		self.Level = 0
	def doMainLoop(self):
		i = 0
		self.Running = True
		while self.Running == True:
			if not self.Level is 0:
				self.Level.Update(0.01)
			i += 1
			try:
					
				for item in self.GameObjects:
					if isinstance(item,Player):
							self.s1.SetUniform2f(item.x,item.y,"CameraPosition")	
							#print item.x,item.y
							#item.Body.setInertia(0)
					if item.Visual is not 0:
						item.Draw()
					if item.Body is not 0:
						item.UpdatePos()
					item.Update(0.1)
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
					#print item.key
					if item.key == 257:
						print "ESCAPE!"
						self.Running = False
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
		if gameObject.Body is not 0:
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
