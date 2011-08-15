%module gfw
%{
#include "gfw.h"
%}

%include "gfw.h"
%include "defines.h"
%include "std_vector.i"
%include "std_string.i"
%include "std_list.i"
%template(FloatVector) std::vector<float>;
%template(KeyEventList) std::list<KeyEvent>;
%template(MouseEventList) std::list<MouseEvent>;