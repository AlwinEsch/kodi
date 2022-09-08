%module (docstring="Java interface to libkodidevkit") kodi

%feature("autodoc", "3");

%define __INTRODUCED_IN_KODI(api_level)
%enddef
%define __DEPRECATED_IN_KODI(api_level)
%enddef
%define __REMOVED_IN_KODI(api_level)
%enddef

%include ../kodi_addon.i

/* This tells SWIG to treat char ** as a special case when used as a parameter
   in a function call */
%typemap(in) char ** (jint size) {
  int i = 0;
  size = jenv->GetArrayLength($input);
  $1 = (char **) malloc((size+1)*sizeof(char *));
  /* make a copy of each string */
  for (i = 0; i<size; i++) {
    jstring j_string = (jstring)jenv->GetObjectArrayElement($input, i);
    const char * c_string = jenv->GetStringUTFChars(j_string, 0);
    $1[i] = static_cast<char*>(malloc((strlen(c_string)+1)*sizeof(char)));
    strcpy($1[i], c_string);
    jenv->ReleaseStringUTFChars(j_string, c_string);
    jenv->DeleteLocalRef(j_string);
  }
  $1[i] = 0;
}

/* This cleans up the memory we malloc'd before the function call */
%typemap(freearg) char ** {
  int i;
  for (i=0; i<size$argnum-1; i++)
    free($1[i]);
  free($1);
}

/* This allows a C function to return a char ** as a Java String array */
%typemap(out) char ** {
    int i;
  int len=0;
  jstring temp_string;
  const jclass clazz = jenv->FindClass("java/lang/String");

  while ($1[len]) len++;
  jresult = jenv->NewObjectArray(len, clazz, NULL);
  /* exception checking omitted */

  for (i=0; i<len; i++) {
    temp_string = jenv->NewStringUTF(*result++);
    jenv->SetObjectArrayElement(jresult, i, temp_string);
    jenv->DeleteLocalRef(temp_string);
  }
}

/* These 3 typemaps tell SWIG what JNI and Java types to use */
%typemap(jni) char ** "jobjectArray"
%typemap(jtype) char ** "String[]"
%typemap(jstype) char ** "String[]"

/* These 2 typemaps handle the conversion of the jtype to jstype typemap type
   and vice versa */
%typemap(javain) char ** "$javainput"
%typemap(javaout) char ** {
    return $jnicall;
  }

%inline %{

bool Init(unsigned int api, char **argv)
{
  int argc = 0;
  while (argv[argc])
  {
    argc++;
  }

  if (!kodi::dl::LoadDll(argc, argv))
    return false;

  KODI_IFC_HDL hdl = kodi::dl::api.kodi_init(api, argc, argv, nullptr, false, false);
  if (!hdl)
    return false;

  kodi::dl::hdl = hdl;

  return true;
}

void Log(const ADDON_LOG loglevel, const char* fmt, ...)
{
  va_list arg;
  va_start(arg, fmt);
  char *errorStr;
  if (vasprintf(&errorStr, fmt, arg) >= 0)
  {
    kodi::dl::api.kodi_log(loglevel, errorStr);
    free(errorStr);
  }
  va_end(arg);
}


%}


%constant int KODI_API = __KODI_API__;

%pragma(java) jniclasscode=%{
    static {
        System.loadLibrary("kodidevkit-java");
    }
%}
