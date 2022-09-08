%include global.i

%module YesNo

%include <typemaps.i>
%apply bool & INOUT { bool & bar };

%import "kodi_addon.i"

%{
#include "gui/dialogs/YesNo.h"
%}
%include "std_string.i"
%include "gui/dialogs/YesNo.h"
