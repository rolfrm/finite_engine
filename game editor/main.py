import gfw
import wx
import pickle
import livecoding.reloader as Reloader
import time
import code
import threading
import Image
gfw.Init(400,400,False)

shader = gfw.Shader("""
attribute vec2 pos;
attribute vec2 uv;
uniform vec2 offset;
uniform vec2 scale;
uniform float rotation;
varying vec2 vuv;
void main(){
vuv = uv;
if(0 == scale){
scale = 1;
}
gl_Position = vec4(pos*scale + offset,0,1);
}

""","""
uniform sampler2D tex;
varying vec2 vuv;
void main(){
gl_FragColor = vec4(texture2D(tex,vuv).rgba);
}

""")

gfw.SetActiveShader(shader)
class Drawable:
    def __init__(self,verts,indexes,uvs,color):
        self.verts = verts
        self.indexes = indexes
        self.uvs = uvs
        self.color = color
        self.changed = True
        self.instanced = False
    def _Draw():
        if self.changed or not self.instanced:
            self.instanced = True
            self.changed = False
            self.ply = gfw.Polygon(gfw.FloatVector(self.verts),gfw.UIntVector(self.indexes),gfw.FloatVector(self.uvs),gfw.FloatVector(self.color))
        self.ply.Draw()    

cr = Reloader.CodeReloader()
cr.AddDirectory("gamefunc","gamefunc")
import gamefunc

###Console stuff
class glob:
    pass

glob.__dict__ = globals()

def startthread():
    code.interact("Welcome to the grid, user",None,glob.__dict__)

consolethread = threading.Thread(None,startthread)
consolethread.start()


if "LoadGame" in gamefunc.__dict__:
    gamefunc.LoadGame()
import gfwi
while consolethread.isAlive():
    time.sleep(0.1)
    for i in globals().keys():
        glob.__dict__[i] = globals()[i]
    if "mainloop" in gamefunc.__dict__:
        mainloop = gamefunc.__dict__["mainloop"]
        try:
            
            mainloop()
            gfwi.UpdateUI()
        except Exception as e:
            print "exc"
            print e
