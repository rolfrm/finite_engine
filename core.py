import physics
import gfw
import time
import sound_system.soundplayer as snd
vs = """
uniform vec2 Zoom;
attribute vec2 pos; 
attribute vec3 color;
attribute vec2 uv;
uniform float Xoff;
uniform float Yoff;
uniform float Rotation;
uniform vec3 Color;
uniform vec2 lightpos;
#define MAX_LIGHTS 5
uniform vec2 MultiLightPos[MAX_LIGHTS];
uniform float MultiLightIntensity[MAX_LIGHTS];
uniform vec2 CameraPosition;
uniform int NumLights;
varying vec3 vColor;
varying vec2 vuv;
varying vec2 lightDir;
varying vec2 normal;
varying vec2 vMultiLightDir[MAX_LIGHTS];

void main(){
vColor = color;
vuv = uv;//vec2(0.5,0.5);
 vec2 npos = vec2(pos.x*cos(Rotation) - pos.y*sin(Rotation), pos.y*cos(Rotation)+ pos.x*sin(Rotation))+vec2(Xoff,Yoff);
 lightDir = npos-lightpos;
 lightDir = npos-MultiLightPos[0];
 
 for(int i= 0; i < NumLights;i++){
	vMultiLightDir[i] = npos-MultiLightPos[i];
 }
 
 normal = uv - vec2(0.5,0.5);
 normal.x *=-1.0;
 normal.y *=-1.0;
 normal = vec2(normal.x*cos(Rotation) - normal.y*sin(Rotation), normal.y*cos(Rotation)+ normal.x*sin(Rotation));
 gl_Position=vec4((npos - CameraPosition)/Zoom,0.0,1.0);
 }
"""

fs = """
uniform sampler2D tex0;
uniform int tex0Active;
uniform sampler2D tex1;
varying vec3 vColor;
varying vec2 vuv;
varying vec2 lightDir;
#define MAX_LIGHTS 5

uniform float MultiLightIntensity[MAX_LIGHTS];
varying vec2 vMultiLightDir[MAX_LIGHTS];
uniform vec3 MultiLightColor[MAX_LIGHTS];
uniform int NumLights;
varying vec2 normal;
void main(){
	vec3 globalColor = vec3(1,1,1)/2.0;
	vec3 col = vColor;
	vec4 tex;
	if( tex0Active == 1){
		 tex= texture2D(tex0,vuv);	
		col +=  tex.xyz;
	}
	vec3 ncol = vec3(0.0,0.0,0.0);
	for(int i =0; i< NumLights;i++){
		 float namb = 1.0/(length(vMultiLightDir[i]));
		 float ndiff = max(dot(normalize(normal),normalize(vMultiLightDir[i])),0.0)*1.0/(length(vMultiLightDir[i])/5.0);
		 ncol += col*(min(1.0,namb) + ndiff*0.5)*MultiLightColor[i]*MultiLightIntensity[i]; 
	}
	ncol += col*globalColor;
	if(tex0Active != 0){
		gl_FragColor= vec4(ncol,tex.a);
	}else{
		gl_FragColor= vec4(ncol,1);

	}
}
"""
class GameObject:
	def __init__(self,graphicsObject = 0, physicsObject = 0):
		self.Visual = graphicsObject
		self.Body = physicsObject
		self.x = 0
		self.y = 0
		self.a = 0
		self.UpdatePos()
		self.CollisionResponse = 0
		self.KeyEventHandler = 0
		self.MouseEventHandler = 0
		
	def SetPos(self,x,y):
		if self.Body is 0:
			self.x = x
			self.y = y
			
		else:
			self.Body.SetPosition(physics.Vec2(x,y))
	def UpdatePos(self):
		if self.Body is not 0:
			pos = self.Body.GetPosition()
			self.x = pos.x
			self.y = pos.y
			self.a = self.Body.GetAngle()
	def Draw(self):
		self.Visual.ActivateTextures()
		gfw.Draw(self.x,self.y,self.a,self.Visual)
	def OnCollision(self,other,force):
		if self.CollisionResponse is not 0:
			self.CollisionResponse(self,other,force)
class Core:
	def __init__(self):
		gfw.Init(600,600,False)
		self.s1 = gfw.Shader(vs,fs)
		gfw.SetActiveShader(self.s1);
		gfw.Zoom(50,50)
		self.PhysicsCore = physics.Core(100)
		self.PhysicsCore.setGravity(0.0,-0.004)
		self.GameObjects = []
		self.KeyEventHandlers =[]
		self.MouseEventHandlers = []
		self.PhysicsObjectRef = {}
		snd.PAInit()
		self.PA = snd.PASink(1)
		self.nodeplayer = snd.NodePlayer(10)
		self.nodeplayer.Connect(self.PA,0,0)
	def doMainLoop(self):
		i = 0
		while True:
			i += 1
			try:
				for item in self.GameObjects:
					if item.Visual is not 0:
						item.Draw()
					if item.Body is not 0:
						item.UpdatePos()
				self.PhysicsCore.Run()
				gfw.Refresh()
				#print "Run"
				while self.PhysicsCore.CollisionsReady():
					col = self.PhysicsCore.GetNextCollision()
					o1 = col.GetFrom()
					o2 = col.GetTo()
					col1 = self.PhysicsObjectRef[o1.GetID()]
					col2 = self.PhysicsObjectRef[o2.GetID()]
					col1.OnCollision(col2,col.GetI())					
					col2.OnCollision(col1,col.GetI())
				self.PA.RecursiveUpdate(i)
				mev = gfw.GetMouseEvents()
				kev = gfw.GetKeyEvents()	
				for item in kev:
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
		try:
			self.GameObjects.remove(gameObject)
		except:
			pass		
		self.PhysicsCore.UnloadObject(gameObject.Body)
		try:
			self.PhysicsObjectRef.pop(gameObject.Body.GetID())
		except:
			pass

		try:
			self.MouseEventHandlers.remove(gameObject)
		except:
			pass
		try:		
			self.KeyeventHandlers.remove(gameObject)
		except:
			pass
	def PlaySound(self,node):
		self.nodeplayer.PlayNode(node,1)
		
