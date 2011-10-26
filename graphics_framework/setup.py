# setup.py

from distutils.core import setup, Extension

setup(name="gfw",
      py_modules=['gfw'], 
      ext_modules=[Extension("_gfw",
                     ["gfw.i","gfw.cpp", "voxels.cpp"],
                     swig_opts=['-c++','-threads'],libraries = ['GL','glfw'],extra_compile_args=["-funsafe-math-optimizations","-O3","-mtune=core2","-fopenmp"],
      extra_link_args=['-lgomp']
                  )]
      
)
