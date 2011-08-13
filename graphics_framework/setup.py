# setup.py

from distutils.core import setup, Extension

setup(name="gfw",
      py_modules=['gfw'], 
      ext_modules=[Extension("_gfw",
                     ["gfw.i","gfw.cpp"],
                     swig_opts=['-c++'],libraries = ['GL','glfw','GLEW']
                  )]
      
)
