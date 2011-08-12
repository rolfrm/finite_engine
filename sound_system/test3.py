import soundplayer as sp
import numpy.random as rand
sp.PAInit()
so1 = sp.Sound(sp.Sound.SINE,sp.NodeToFreq(0))
so2 = sp.Sound(sp.Sound.SINE,sp.NodeToFreq(4))
so3 = sp.Sound(sp.Sound.SQUARE,sp.NodeToFreq(9))
e1 = sp.SoundEnv(so1,[(0,0),(0.1,2.0),(1,0.0),(0,0)])
mix = sp.Mixer(3)
out = sp.PASink(1)
np = sp.NodePlayer(10)

np.Connect(mix,0,0)
def makeScale(scale):
	return (lambda x: 12*(x/len(scale)) +scale[x%len(scale)])

def makeTune(scale,notes):
	pass

Amajor = makeScale([0,2,4,5,7,9,11])
Aminor = makeScale([0,2,3,5,7,8,11])
class tune:
	### Plays a tune of nodes given a scale(look up)
	### Nodes comes in different variants:
	### (note,duration,volume) Normal note
	### (note,duration) Uses last volume
	### (pause) Wait before next node
	def __init__(self,scale,nodes):
		self.nodes = nodes
		self.scale = scale
		self.i = 0
		self.t = 0
		self.node = ()
		
	def getDur(self,val):
		return 2*0.5**val
	def GetNext(self):
		if self.i >= len(self.nodes):
			return ()
		
		node = self.nodes[self.i]
		self.i +=1
		if isinstance(node,int):
			node = [node]
		return node
	def Update(self,nodeplayer,dt):
		
		while len(self.node) > 1:
			nodeplayer.PlayNode(self.scale(self.node[0]),self.getDur(self.node[1]))
			self.node = self.GetNext()
		if len(self.node) == 1:
			if self.t > self.getDur(self.node[0]):
					self.t = 0
					self.node = self.GetNext()
		elif len(self.node) == 0:
			self.node = self.GetNext()
		self.t += dt
t1 = tune(Amajor,[[0,1],[1],(2,1),[1],(4,1),[1],(4,2),[2],(4,1),[1],(2,2),[2],(4,1),[1]])

rtunelen = 200
tones = rand.randint(-6,8,rtunelen)
tones2 = rand.randint(0,4,rtunelen)
timings = rand.randint(1,4,rtunelen)
nstr = []
for i in range(0,rtunelen):
	new = [ [tones[i], timings[i]],[tones2[i],timings[i]],[timings[i]] ]
	nstr.extend(new)
print nstr
t2 = tune(Aminor,nstr)

#so2.Connect(mix,0,1)
#so3.Connect(mix,0,0)
mix.Connect(out,0,0)
#so2.Connect(out,0,0)	
print out.GetChildren()
print out.Children
j = 0
for i in range(0,2000):
	t1.Update(np,0.05)
	out.RecursiveUpdate(i)
out.Kill()
sp.PAKill()	