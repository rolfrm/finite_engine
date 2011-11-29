import gfwi
import copy

class Entity(gfwi.Drawable,gfwi.Scripted,gfwi.KeyEventHandler):
	quad = gfwi.Polygon([-0.5,-0.5,0.5,-0.5,0.5,0.5,-0.5,0.5],[],[],[0,0,1,0,1,1,0,1])
	
	def __init__(self,pos,rotation,size):
		gfwi.Drawable.__init__(self,pos,rotation,size,self.quad)
	
	def KeyEvent(self,evt):
		print evt
	
def LoadTest():
	import copy
	import pickle
	gfwi.ActiveDrawRutine = []
	guy1 = Entity((0,0),0,(1,1))
	guy1.Texture = "lolner.png"
	guy2 = copy.copy(guy1)
	guy2.Texture = "meeh.png"
	gfwi.LoadObject(guy1)
	return [guy1,guy2]
	
def Save(savefile,level):
	import pickle
	pickle.dump(level,savefile)

#pickle.dump(level,savefile)

def Load(savefile,level):
	import pickle
	
	for i in range(len(level)):
		level.pop()
	level.extend(pickle.load(savefile))

def mainloop():
	pass
