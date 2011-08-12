import pyaudio
import wave
import sys
import scipy
import numpy
import ctypes
chunk = 1024
p = pyaudio.PyAudio()

class SoundNode(object):
	def __init__(self):
		self.level = 0
		self.Children = []
		self.outlets = []
		self.inlets = []
	def Update(self):
		pass

	def Connect(self,sink, noOutlet, noInlet):
		sink.Children.append(self)
		if isinstance(sink,Sink):
			self.children.append(sink)
			inlet = sink.GetInlet(noInlet)
			if isinstance(inlet,Sink.Inlet):
				self.outlets[noOutlet].Connect(inlet)

	class Outlet:
		def __init__(self):
			self.Connections = []
		def Connect(self, sink):
			
			self.Connections.append(sink)
		def Signal(self,sig):
			for i in self.Connections:
				i.ReadData(sig)

	class Inlet:
		def __init__(self):
			self.storage = ''
		def ReadData(self, signal):
			self.storage = signal
		def GetData(self):
			buf = self.storage
			self.storage = ''
			return buf


class Source(SoundNode):
	def __init__(self, nofOutlets,sourceFunction):
		super(Source,self).__init__()
		self.outlets = []
		self.children = []
		self.srcFun = sourceFunction
		for i in range(0,nofOutlets):
			self.outlets.append(self.Outlet())
	class Outlet:
		def __init__(self):
			self.Connections = []
		def Connect(self, sink):
			
			self.Connections.append(sink)
		def Signal(self,sig):
			for i in self.Connections:
				i.ReadData(sig)
	def Connect(self,sink, noOutlet, noInlet):
		sink.Children.append(self)
		if isinstance(sink,Sink):
			self.children.append(sink)
			inlet = sink.GetInlet(noInlet)
			if isinstance(inlet,Sink.Inlet):
				self.outlets[noOutlet].Connect(inlet)
	def SendData(self):
		out = self.srcFun()
		for i in range(0,len(out)-1):
			self.outlets[i].Signal(out[i])
	def Update(self):
		self.SendData()		
	
class Sink(SoundNode):
	def __init__(self, nofInlets,sinkFunc):
		super(Sink,self).__init__()
		self.inlets = []
		self.sinkFun = sinkFunc
		for i in range(0,nofInlets):
			self.inlets.append(self.Inlet())	
	class Inlet:
		def __init__(self):
			self.storage = ''
		def ReadData(self, signal):
			self.storage = signal
		def GetData(self):
			buf = self.storage
			self.storage = ''
			return buf
	def GetInlet(self,inletNr):
		return self.inlets[inletNr]
	def Update(self):
		dat = []
		for i in self.inlets:
			dat.append(i.GetData())
		self.sinkFun(*dat)
		

class Sound(Source):
	SINE=0
	SAW=1
	SQUARE=2
	def __init__(self,soundType, duration, frequency,samplespeed, chunksize):
		super(Sound,self).__init__(1,lambda :self.Generate())		
		self.duration = duration
		self.frequency = frequency
		self.t = 0
		self.x = 0
		self.sampleSpeed = samplespeed
		self.chunkSize = chunksize
		if soundType == self.SINE:
			self.Generate = self.sine_gen
		elif soundType == self.SAW:
			self.Generate = self.saw_gen
		else:
			self.Generate = self.square_gen
	def sine_gen(self):
		x1 = self.x*self.chunkSize
		x2 = (self.x+1)*self.chunkSize	
		sample = numpy.arange(x1, x2, dtype=numpy.float32)
		self.x = int(self.x + 1)
	
		self.t = float(self.x)*self.chunkSize/self.sampleSpeed
		sample *= numpy.pi * 2 / self.sampleSpeed
		sample *= self.frequency
		return numpy.sin(sample), self.t < self.duration
	def saw_gen(self):
		pass
	def square_gen(self):
		pass

class PASource(Sink):
	def __init__(self,channels):
		super(PASource,self).__init__(channels,lambda *data:self.play(data))
		self.channels = channels
		self.stream = p.open(format = pyaudio.paFloat32,
                	channels = self.channels,
                	rate = 44100,
                	output = True)
	def play(self, data):
		shape = (len(data),data[0].size)
		connected = numpy.array(data)
		connected = connected.reshape((connected.size),order='F')
		self.stream.write(connected.tostring())
		connected = ''

so1 = Sound(Sound.SINE,1,100,44100,1024)
so2 = Sound(Sound.SINE,1,300,44100,1024)
si1 = PASource(2)
so1.Connect(si1,0,0)
so2.Connect(si1,0,1)
allNodes = [si1,so1,so2]
def nodeCmp(a,b):
	if a.level > b.level:
		return -1
	elif a.level < b.level:
		return 1
	else:
		return 0
def UpdateChildren(sink,level):
	for i in sink.Children:
		i.level = level+1
		UpdateChildren(i,i.level)
UpdateChildren(si1,0)
allNodes.sort(cmp = nodeCmp)
class ADSR:
	def __init__(self,attack,decay,sustain,release, soundGenerator):
		self.attack = attack
		self.decay = decay
		self.sustain = sustain
		self.release = release
		self.soundGenerator = soundGenerator
	

class Filter:
	def __init__(self, inputs, outputs):
		pass

data = "omg"
while data != '':
    for i in allNodes:
	i.Update()
    
stream.close()
p.terminate()

