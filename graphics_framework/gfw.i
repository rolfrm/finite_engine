%module gfw
%{
#include "gfw.h"
%}

%include "gfw.h"
%include "std_vector.i"
%include "std_string.i"
%template(FloatVector) std::vector<float>;
