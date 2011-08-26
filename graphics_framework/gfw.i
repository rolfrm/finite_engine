%module gfw
%{
#include "gfw.h"
%}


%include "defines.h"
%include "std_vector.i"
%include "std_string.i"
%apply const std::string& {std::string* foo};
%include "std_list.i"
%include "gfw.h"
%template(FloatVector) std::vector<float>;
%template(IntVector) std::vector<int>;
%template(UIntVector) std::vector<unsigned int>;
%template(KeyEventList) std::list<KeyEvent>;
%template(MouseEventList) std::list<MouseEvent>;


