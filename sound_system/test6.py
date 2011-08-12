import soundplayer
import time 
s = soundplayer.Sound(soundplayer.Sound,440)
t = time.clock()
print "start sine"
for i in range(0,1000):
	s.sine_gen()
print "stop", t - time.clock()

t = time.clock()
print "start saw"
for i in range(0,1000):
	s.saw_gen()
print "stop", t-time.clock()

t = time.clock()
print "start square"
for i in range(0,1000):
	s.square_gen()
print "stop",t-time.clock()