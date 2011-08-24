import core
import gfw
import numpy
import physics
import random
import time
from utils import *
random.seed(time.time())
def SpeedRun(length):
	lv = []
	nextpos = 0
	lasty = 0
	color = [random.random(),random.random(),random.random()]
	color = [1,1,1]	
	print color
	
	#font = LoadImageAsTexture("font1.png")
	#Text = gfw.Text(font,0,1,0,1,4,4,0)
	#print font
	for i in range(0,length):
		ground = MakeCompleteObject([-100,0, 100,0, 100,-100,-100,-100],color[:],[0,-1, 1,-1, 1,0, 0,0])
		#ground.Visual.AddTexture(font,0)
		ground.SetPos(nextpos,lasty)
		#ground.tex = font
		lv.append(ground)
		nextpos = nextpos + 200+i*i
		lasty = lasty + (random.random()-0.5)*200
		#color[0] = color[0]*0.5 + random.random()*0.5
		#color[1] = color[1]*0.5 + random.random()*0.5
		#color[2] = color[2]*0.5 + random.random()*0.5
		#print color
	return core.Level([],lv,[])
	
print "Welcome to extreme speed run:"
#time.sleep(1)
print "Speed up on the Right arrow"
#time.sleep(1)
print "jump on up!"
#time.sleep(1)
print "AAAANNNDD !"
#time.sleep(1)
print "GO!"

gameCore = core.Core()
player = core.Player()
player.SetPos(0,50)
player.Body.setMass(1)
MainScreen = SpeedRun(100)
MainScreen.Load(gameCore,player)

for i in gameCore.GameObjects:
	i.UpdatePos()

gameCore.doMainLoop()



