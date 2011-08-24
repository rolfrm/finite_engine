# setup.py

from distutils.core import setup, Extension

setup(name="physics",
      py_modules=['physics'], 
      ext_modules=[Extension("_physics",
                     ["physics.i","PhysicsCore.cpp","Objects/PhysicsObject.cpp","Objects/Constraint.cpp","CollisionDetection/SeperatingAxis.cpp","Shapes/Polygon.cpp","Shapes/PolygonGenerator.cpp","Math/Math.cpp"],
                     swig_opts=['-c++']
                  )]
      
)

