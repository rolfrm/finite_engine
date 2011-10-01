from copy import copy
import physics
import gfw
import utils
import Image
import copy
import math
class LevelBase(object):
	
	def __init__(self):
		self.ItemList = []
	
	def LoadLevel(self):
		pass
	def Update(self,dt):
		pass
	def GetItems(self):
		return self.ItemList
	def AddItems(self,items):
		self.ItemList.extend(items)
	def RemoveItems(self,items):
		for i in items:
			self.ItemList.remove(i)
	def ClearItems(self):
		self.ItemList = list()
noGameObjects = 0
class GameObject(object):

	def __init__(self,graphicsObject = 0, physicsObject = 0,x =0, y = 0, a = 0, offset = (0,0)):
		self.Visual = graphicsObject
		self.Body = physicsObject
		self.offset = offset
		self.SetPos(x,y)
		self.a = a
		self.z = 0
		self.parralax = 1
		self.UpdatePos()
		self.CollisionResponse = 0
		self.KeyEventHandler = 0
		self.MouseEventHandler = 0
		self.IgnoresCamera = False
		self.UpdateFunc = 0

	def SetPos(self,x,y):
		
		self.x = x
		self.y = y
		if self.Body is not 0:
			print self
			self.Body.SetPosition(physics.Vec2(x,y))
	def UpdatePos(self):
		if self.Body is not 0:
			pos = self.Body.GetPosition()
			self.x = pos.x
			self.y = pos.y
			self.a = self.Body.GetAngle()
	def Draw(self,campos = (0,0)):
		x = self.x
		y = self.y
		if not self.IgnoresCamera:
			#x -=campos[0]*self.parralax
			#y -= campos[1]*self.parralax
			x = (x - campos[0])*self.parralax
			y = (y - campos[1])*self.parralax
		if isinstance(self.Visual,list):
			#[obj,[x,y],[depencies],resolved]
			vlist = self.Visual[:]
			for i in range(0,len(vlist)):
				vlist[i] = self.Visual[i][:]
				vlist[i].append(False)
				vlist[i][1] = self.Visual[i][1][:]
				
			resolved = False
			while resolved is False:
				resolved = True
			
			for i in vlist:
				if i[3] == True:
					continue
				dep = i[2]
				
				if dep == None:
					vec = i[1]
					vec[0] += x
					vec[1] += y
					i[3] = True
				else:					
					ctd = False
					
					for j in dep:
						
						if vlist[j][3] is False:
							ctd = True
							
					if ctd:
						resolved = False
						continue
					i[3] = True
					vec = i[1]
					ldep = len(dep)
					if ldep > 1:
						vec = [0,0]
						for j in dep:
							deppos = vlist[j][1]
							
							vec[0] += deppos[0]
							vec[1] += deppos[1]
						vec[0] = vec[0]/ldep
						vec[1] = vec[1]/ldep
					else:
						deppos = vlist[j][1]
						vec[0] += deppos[0]
						vec[1] += deppos[1]
				obj = i[0]
				
				gfw.Draw(vec[0],vec[1],self.a,i[0])
		else:
			gfw.Draw(x+self.offset[0],y+self.offset[1],self.a,self.Visual)
	def OnCollision(self,other,force):
		if self.CollisionResponse is not 0:
			self.CollisionResponse(self,other,force)
	def Update(self,dt):
		if self.UpdateFunc is not 0:
			self.UpdateFunc(self,dt)

