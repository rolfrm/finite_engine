%module physics
%{
#include "PhysicsCore.h"
#include "Objects/PhysicsObject.h"
#include "Shapes/Polygon.h"
#include "Math/Math.hpp"
#include "Objects/Constraint.hpp"
#include "Shapes/PolygonGenerator.hpp"
%}


%include "std_list.i"
%include "std_vector.i"
%include "Objects/PhysicsObject.h"
%include "PhysicsCore.h"
%include "Shapes/Polygon.h"
%include "Math/Math.hpp"
%include "Objects/Constraint.hpp"
%include "Shapes/PolygonGenerator.hpp"

