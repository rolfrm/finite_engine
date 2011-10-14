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

gfw.Init(500,500,False)
s1 = gfw.Shader(shader[0],shader[1])
poly = utils.MakePolygon([-1,-1,-1,1,1,1,1,-1],[],[],[0,0, 0,1, 1,1, 1,0])
gfw.SetActiveShader(s1);
gfw.Zoom(1,1)
gfw.SetBGColor(1,0,0)	
vox1 = gfw.NTree()

import math
for i in range(0,512):
	for j in range(0,512):
		x = float(i)/512
		y = float(j)/512
		#print x,y
		vox1.InsertPoint(x,0.01 ,y,5 ,int(0x33FF33))
		if x > 0.3 and x < 0.7 and y >0.3 and y < 0.7:
			vox1.InsertPoint(x,math.sin(x*20)*math.cos(y*20)*0.04 +0.1 + math.sin(x*80)*0.02 + math.cos(y*160)*0.01 - 0.005 ,y,6 ,int(0xAAAAAA + int(math.sin(x+y)*20)))
		#vox1.InsertPoint(x,math.sin(x*20)*math.cos(y*20)*0.04 +0.1 + math.sin(x*80)*0.02 + math.cos(y*160)*0.01 - 0.005 ,y,7 	,int(0xAAAAAA + int(math.sin(x+y)*20)))

imgdata = numpy.zeros( (500*4,500),dtype=numpy.uint8)
flowbuffer1 = gfw.Texture(imgdata.tostring(),200,200,4,1)
poly.AddTexture(flowbuffer1,0)
import random
import time
import math
i = 0
scene = gfw.Scene(vox1)

scene.SetTree(vox1)
for k in range(0,10000):
	if k % 50 == 0:
		i+=1
	t = time.time()
	scene.SetRayLight(gfw.MakeRayLight(0.9,math.sin(k*0.01),0.9,1,1,1,0.1,1),0)
	scene.SetRayLight(gfw.MakeRayLight(0.1,math.sin(k*0.01),0.1,1,1,1,0.1,1),1)
	print math.sin(k*0.01)
	#for i in range(120,130):
	#	for j in range(1,256):
	#		x = float(i)/256
	#		y = float(j)/256
	#		#print x,y
	#		vox1.InsertPoint(x,math.sin(x*20)*math.cos(y*20)*0.04 + 		0.2,y,8,k*10)	
	 
	x = 0.50001 #+ 0.3*math.sin(float(k)/160)
	z = 0.50001 #- k*0.01 #+ math.cos(float(k)/160)*0.3
	y = 0.70001 #math.sin(x*20)*math.cos(z*20)*0.04 +0.1 + math.sin(x*80)*0.02 +0.05 #0.2#*math.sin(float(k)/40)+0.5
	#r =gfw.MakeRay(x,y,z,0,-1,0)
	#r = vox1.CastRay2(r)
	#print r.x,r.y,r.z
	#x = r.x 
	#y = r.y+0.02 
	#z = r.z
	scene.SetCamera(gfw.MakeCamera(x,y,z,1.5,0.01*k,0,1))
	scene.RenderToTexture(flowbuffer1)
	#vox1.RenderToTextureTest(flowbuffer1,x,y,z,3.14/2,0,0)
	poly.Draw(0,0,0)
	gfw.Refresh()
	time.sleep(0.1)
	print "Rendering took:" + str(1/(time.time()-t)),x,y,z	
