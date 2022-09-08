%define ATTR_DLL_LOCAL
%enddef
%define __INTRODUCED_IN_KODI(api_level)
%enddef
%define __DEPRECATED_IN_KODI(api_level)
%enddef
%define __REMOVED_IN_KODI(api_level)
%enddef

%ignore KODI_ADDON_INSTANCE_INFO;

%include <typemaps.i>
%apply bool & INOUT { bool & bar };
