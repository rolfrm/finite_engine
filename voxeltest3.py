import gfw
import numpy
import utils
shader =["""
attribute vec2 pos; 
attribute vec2 uv;
uniform vec2 Zoom;
varying vec2 vuv;
void main(){
 vuv = uv;
 vec2 npos = vec2(pos.x, pos.y)/Zoom;
 gl_Position=vec4(npos,0.0,1.0);
 }
""", """

// outwards flow calculating shader
uniform sampler2D tex0;
varying vec2 vuv;
void main(){
	vec4 col = texture2D(tex0,vuv);
	gl_FragColor = vec4(col)*3	;
}
"""]

gfw.Init(1000,1000,False)
s1 = gfw.Shader(shader[0],shader[1])
poly = utils.MakePolygon([-1,-1,-1,1,1,1,1,-1],[],[],[0,0, 0,1, 1,1, 1,0])
gfw.SetActiveShader(s1);
gfw.Zoom(1,1)
gfw.SetBGColor(1,0,0)	
vox1 = gfw.NTree()

import math
for i in range(0,500):
	for j in range(0,500):
		x = float(i)/500
		y = float(j)/500
		#print x,y
		vox1.InsertPoint(x,0.01 ,y,6 ,int(0x99FF99))
		if x > 0.3 and x < 0.7 and y >0.3 and y < 0.7:
			vox1.InsertPoint(x,math.sin(x*20)*math.cos(y*20)*0.04 +0.1 + math.sin(x*80)*0.02 + math.cos(y*160)*0.01 - 0.005 ,y,8 ,int(0xAAAAAA + int(math.sin(x+y)*20)))
			vox1.InsertPoint(x,math.sin(x*20)*math.cos(y*20)*0.04 +0.1 + math.sin(x*80)*0.02 + math.cos(y*160)*0.01 - 0.015 ,y,8 ,int(0xAAAAAA + int(math.sin(x+y)*20)))
		#vox1.InsertPoint(x,math.sin(x*20)*math.cos(y*20)*0.04 +0.1 + math.sin(x*80)*0.02 + math.cos(y*160)*0.01 - 0.005 ,y,7 	,int(0xAAAAAA + int(math.sin(x+y)*20)))

imgdata = numpy.zeros( (500*4,500),dtype=numpy.uint8)
flowbuffer1 = gfw.Texture(imgdata.tostring(),200,200,4,1)
poly.AddTexture(flowbuffer1,0)
import random
import time
import math
i = 0
scene = gfw.Scene(vox1)
x = 0.6501
y = 0.00
z = 0.6661
movz = 0
movr = 0
r = 0
#scene.SetTree(vox1.GetChild(0,0,0))
scene.SetTree(vox1)
vox1.UpdateTree()

x = 0.50001
y = 0.00
z = 0.50001

for k in range(0,5000):
	
	if k % 50 == 0:
		i+=1
	t = time.time()
	scene.SetRayLight(gfw.MakeRayLight(0.5+ math.sin(k*0.01 +1)*0.3,0.4,0.5 + 0.1 ,1,1,0.5,0.01,1),0)
	scene.SetRayLight(gfw.MakeRayLight(0.5+ math.sin(k*0.01+2)*0.3,0.2,0.5 ,0,1,0,0.01,1),1)
	scene.SetRayLight(gfw.MakeRayLight(0.5+ math.sin(k*0.01)*0.3,0.2,0.5  - 0.1,0,0,1,0.01,1),2)
	#scene.SetRayLight(gfw.MakeRayLight(0.5+ math.sin(k*0.01)*0.3+ 0.05,0.3+ 0.05,0.5 ,1,1,1,0.01,1),3)
	#scene.SetRayLight(gfw.MakeRayLight(0.8,math.sin(k*0.01),0.9,1,1,1,0.01,1),1)
	#scene.SetRayLight(gfw.MakeRayLight(0.7,math.sin(k*0.01),0.9,1,1,1,0.01,1),1)
	#for i in range(120,130):
	#	for j in range(1,256):
	#		x = float(i)/256
	#		y = float(j)/256
	#		#print x,y
	#		vox1.InsertPoint(x,math.sin(x*20)*math.cos(y*20)*0.04 + 		0.2,y,8,k*10)	
	 
	#x = 0.50001 #+ 0.3*math.sin(float(k)/160)
	#z = 0.5 + math.sin(k*0.001)*0.2 #+ math.cos(float(k)/160)*0.3
	y =0.02+ math.sin(x*20)*math.cos(z*20)*0.04 +0.1 + math.sin(x*80)*0.02 + math.cos(z*160)*0.01 - 0.005 #0.2001 #math.sin(x*20)*math.cos(z*20)*0.04 +0.1 + math.sin(x*80)*0.02 +0.05 #0.2#*math.sin(float(k)/40)+0.5
	#r =gfw.MakeRay(x,y,z,0,-1,0)
	#r = vox1.CastRay2(r)
	#print r.x,r.y,r.z
	#x = r.x 
	#y = r.y+0.02 
	#z = r.z
	scene.SetCamera(gfw.MakeCamera(x,y,z,0,r,0,1))
	scene.SetAmbient(0.1,0.1,0.05)
	scene.RenderToTexture(flowbuffer1)
	#vox1.RenderToTextureTest(flowbuffer1,x,y,z,3.16/2,0,0)
	poly.Draw(0,0,0)
	gfw.Refresh()
	keyEv = gfw.GetKeyEvents()
	#print keyEv.size()
	
	
	
	for i in range(keyEv.size()):
		print keyEv[i].key
		if keyEv[i].key == 283:
			if keyEv[i].action == 1:
				movz +=1
			else:
				movz -=1
		elif keyEv[i].key == 284:
			if keyEv[i].action == 1:
				movz -=1
			else:
				movz +=1
		elif keyEv[i].key == 285:
			if keyEv[i].action == 1:
				movr -=1
			else:
				movr +=1
		elif keyEv[i].key == 286:
			if keyEv[i].action == 1:
				movr +=1
			else:
				movr -=1
		elif keyEv[i].key == 257:
			quit()
		elif keyEv[i].key == 76:
			if keyEv[i].action ==1:
				scene.DoLightning = not scene.DoLightning
	z += math.cos(r-0.1)*movz*0.001
	x += math.sin(r-0.1)*movz*0.001
	r += movr*0.1
	#for i in keyEv:
	#	print i
	time.sleep(0.03)
	print "Rendering took:" + str(1/(time.time()-t)),x,y,z	
