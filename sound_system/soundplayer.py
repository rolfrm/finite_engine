import pyaudio
import wave
import sys
import scipy
import numpy
import ctypes
import math
import time
import scipy.signal as sig
chunk = 1024
samplespeed = 44100
nodeScaling = 2**(1/12.0) 

def PAInit():
	global p
	p = pyaudio.PyAudio()

def PAKill():
	p.terminate()

def NodeToFreq(node):
	return (nodeScaling**node)*220

class SoundNode(object):
	def __init__(self,noInlets,noOutlets,func):
		self.Children = []
		self.noOutlets = noOutlets
		self.noInlets = noInlets
		self.outlets = []
		self.inlets = []
		self.Parent = []
		self.func = func
		self.checked = False
		self.updateId = 0
		for i in range(0,noOutlets):
			self.outlets.append(self.Outlet())
		for i in range(0,noInlets):
			self.inlets.append(self.Inlet())
			
	def Update(self):
		
		input = []
		for i in self.inlets:
			data = i.GetData()
			if len(data) != chunk:
				data = numpy.zeros(shape=(chunk,),dtype=numpy.float32)
			input.append(data)
		out = self.func(*input)
		if not isinstance(out,list) or not isinstance(out,tuple):
			out = [out]
		for i in range(0,len(self.outlets)):
				self.outlets[i].Signal(out[i])
				
	def RecursiveUpdate(self,id):
			
			if self.updateId == id:
					return
			for i in self.Children:
				i.RecursiveUpdate(id)
			self.Update()
			self.updateId = id
			
	def GetChildren(self):
		children = self.getChildren()
		for i in children:
			i.checked = False
		return children
	def getChildren(self):
		if self.checked: #If children allready returned
			return []
		children = [self]
		self.checked = True
		
		for i in self.Children:
			children.extend(i.getChildren())
		return children
	
	def Connect(self,sink, noOutlet, noInlet):
		sink.Children.append(self)
		self.Parent.append(sink)
		self.outlets[noOutlet].Connect(sink.GetInlet(noInlet))
		
	def Disconnect(self,sink,noOutlet,noInlet):
		sink.Children.remove(self)
		self.GetOutlet(noOutlet).Disconnect(sink.GetInlet(noInlet))
		
	def GetInlet(self,i):
			return self.inlets[i]
		
	def GetOutlet(self,i):
			return self.outlets[i]
	
	class Outlet:
		def __init__(self):
			self.Connections = []
		def Connect(self, inlet):
			self.Connections.append(inlet)
		def Disconnect(self, inlet):
			self.Connections.remove(inlet)
		def Signal(self,sig):
			for i in self.Connections:
				i.ReadData(sig)
	class Inlet:
		def __init__(self):
			self.storage = ''
		def ReadData(self, signal):
			if self.storage is not '':
				self.storage = self.storage + signal
			self.storage = signal
		def GetData(self):
			buf = self.storage
			self.storage = ''
			return buf

class Source(SoundNode):
	def __init__(self, nofOutlets,sourceFunction):
		super(Source,self).__init__(0,nofOutlets,sourceFunction)

	
class Sink(SoundNode):
	def __init__(self, nofInlets,sinkFunc):
		super(Sink,self).__init__(nofInlets,0,sinkFunc)
		

class Sound(Source):
	SINE=0
	SAW=1
	SQUARE=2
	def __init__(self,soundType, frequency):
		super(Sound,self).__init__(1,lambda :self.Generate())		
		self.frequency = frequency
		self.t = 0
		self.x = 0
		if soundType == self.SINE:
			self.Generate = self.sine_gen
		elif soundType == self.SAW:
			self.Generate = self.saw_gen
		else:
			self.Generate = self.square_gen

	def sine_gen(self):
		x1 = self.x*chunk
		x2 = (self.x+1)*chunk	
		sample = numpy.arange(x1, x2, dtype=numpy.float32)
		self.x = int(self.x + 1)
	
		sample *= numpy.pi * 2 / samplespeed
		sample *= self.frequency
		sample = numpy.sin(sample)
		return sample
	def saw_gen(self):
		x1 = self.x*chunk
		x2 = (self.x+1)*chunk	
		sample = numpy.arange(x1, x2, dtype=numpy.float32)
		self.x = int(self.x + 1)
		sample *= numpy.pi * 2 / samplespeed
		sample *= self.frequency
		sample = sig.sawtooth(sample).astype(numpy.float32)
		return sample
		
	def square_gen(self):
		x1 = self.x*chunk
		x2 = (self.x+1)*chunk	
		sample = numpy.arange(x1, x2, dtype=numpy.float32)
		self.x = int(self.x + 1)
		sample *= numpy.pi * 2 / samplespeed
		sample *= self.frequency
		sample = sig.square(sample).astype(numpy.float32)
		return sample
	

class PASink(Sink):
	def __init__(self,channels):
		super(PASink,self).__init__(channels,lambda data:self.play(data))
		self.channels = channels
		self.stream = p.open(format = pyaudio.paFloat32,
                	channels = self.channels,
                	rate = 44100,
					frames_per_buffer = chunk,
                	output = True)
	def play(self, data):
		shape = (len(data),data[0].size)
		connected = numpy.array(data)
		connected = connected.reshape((connected.size),order='F')

		self.stream.write(connected.tostring(),num_frames=chunk	)
		
		connected = ''

	def Kill(self):
		self.stream.close()


class SoundEnv(SoundNode):
	def __init__(self,sound,stages):
		super(SoundEnv,self).__init__(1,1,lambda i1: self.env(i1))
		self.t = 0
		self.stages = stages
		sound.Connect(self,0,0)
		self.sound = sound
		self.vol = 0
		self.ok = True
	def env(self,i1):
		dt = float(chunk)/samplespeed
		envdata, ok = self.getFilterChunk()
		self.ok = ok
		if self.ok == False:
			return i1*0
		return i1*envdata
	def getFilterChunk(self):
		keynode1 = self.stages[0]
		dt = float(chunk)/samplespeed
		target = self.stages[0][1]
		
		totaltime = float(self.stages[0][0])
		dv = target - self.vol
		
		if totaltime > 0 :
			step = (self.t+dt)/totaltime
			if self.t+dt > totaltime:
				step = 1
			endvol = self.vol + dv*step
		else:
			endvol = target
		#print dv,target,self.vol,self.t
		self.t = self.t + dt
		data = numpy.linspace(self.vol,endvol,chunk).astype(numpy.float32)
		self.vol = endvol
		ok = True
		if self.t > totaltime:
			self.t -= self.stages[0][0]
			if len(self.stages) > 1:
				self.stages = self.stages[1:]
			else:
				ok = False
		return data, ok
	


class Mixer(SoundNode):
	def __init__(self, inputs):
		super(Mixer,self).__init__(inputs,1,lambda *data: self.Mix(data))
	def Mix(self,inputs):
		nInputs = len(inputs)
		out =inputs[0] 
		for i in range(1,nInputs):
			 out = out.__add__( inputs[i])

		out= out/nInputs
		return out

class NodePlayer(SoundNode):
	
	def __init__(self,maxInputs):
		super(NodePlayer,self).__init__(maxInputs,1,lambda *data: self.Mix(data))
		self.players = []
		for i in range(0,maxInputs):
			self.players.append(())
		
	def PlayNode(self,node,duration):
		freeslot = -1
		for i in range(0,len(self.players)):
				if self.players[i] == ():
					freeslot = i
					break
		if freeslot == -1:
			return
		
		snd = Sound(Sound.SINE,NodeToFreq(node))
		env = SoundEnv(snd,[(0,0),(0.1,2.0),(duration-0.1,0.0)])
		env.Connect(self,0,freeslot)
		self.players[freeslot] = (snd,env)
		
	def Mix(self,inputs):
		for i in range(0,len(self.players)):
			item = self.players[i]
			if len(item) == 2 and item[1].ok == False:
				
				item[0].Disconnect(item[1],0,0)
				item[1].Disconnect(self,0,i)
				self.players[i] = ()
		nInputs = len(inputs)
		out =inputs[0] 
		for i in range(1,nInputs):
			 out = out.__add__( inputs[i])

		out= out/2.5
		return out