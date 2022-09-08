%include global.i

%module TextViewer

%import(module="kodi") "AddonBase.h"

%{
#include "gui/dialogs/TextViewer.h"
%}
%include "std_string.i"
%include "gui/dialogs/TextViewer.h"
