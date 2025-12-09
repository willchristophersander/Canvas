#include <glad/glad.h>
#include <dlfcn.h>
#include <cstdlib>

// Define function pointers (they're declared as extern in header)
// These are in global namespace, not extern "C" (header declares them before extern "C" block)
PFNGLCLEARCOLORPROC glad_glClearColor;
PFNGLCLEARPROC glad_glClear;
PFNGLVIEWPORTPROC glad_glViewport;
PFNGLENABLEPROC glad_glEnable;
PFNGLDISABLEPROC glad_glDisable;
PFNGLBLENDFUNCPROC glad_glBlendFunc;
PFNGLGENBUFFERSPROC glad_glGenBuffers;
PFNGLBINDBUFFERPROC glad_glBindBuffer;
PFNGLBUFFERDATAPROC glad_glBufferData;
PFNGLDELETEBUFFERSPROC glad_glDeleteBuffers;
PFNGLCREATESHADERPROC glad_glCreateShader;
PFNGLSHADERSOURCEPROC glad_glShaderSource;
PFNGLCOMPILESHADERPROC glad_glCompileShader;
PFNGLGETSHADERIVPROC glad_glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC glad_glGetShaderInfoLog;
PFNGLDELETESHADERPROC glad_glDeleteShader;
PFNGLCREATEPROGRAMPROC glad_glCreateProgram;
PFNGLATTACHSHADERPROC glad_glAttachShader;
PFNGLLINKPROGRAMPROC glad_glLinkProgram;
PFNGLGETPROGRAMIVPROC glad_glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC glad_glGetProgramInfoLog;
PFNGLUSEPROGRAMPROC glad_glUseProgram;
PFNGLDELETEPROGRAMPROC glad_glDeleteProgram;
PFNGLGETUNIFORMLOCATIONPROC glad_glGetUniformLocation;
PFNGLUNIFORM1FPROC glad_glUniform1f;
PFNGLUNIFORM2FPROC glad_glUniform2f;
PFNGLUNIFORM4FPROC glad_glUniform4f;
PFNGLGENVERTEXARRAYSPROC glad_glGenVertexArrays;
PFNGLBINDVERTEXARRAYPROC glad_glBindVertexArray;
PFNGLDELETEVERTEXARRAYSPROC glad_glDeleteVertexArrays;
PFNGLENABLEVERTEXATTRIBARRAYPROC glad_glEnableVertexAttribArray;
PFNGLVERTEXATTRIBPOINTERPROC glad_glVertexAttribPointer;
PFNGLDRAWARRAYSPROC glad_glDrawArrays;
PFNGLACTIVETEXTUREPROC glad_glActiveTexture;
PFNGLGENTEXTURESPROC glad_glGenTextures;
PFNGLBINDTEXTUREPROC glad_glBindTexture;
PFNGLTEXIMAGE2DPROC glad_glTexImage2D;
PFNGLTEXPARAMETERIPROC glad_glTexParameteri;
PFNGLDELETETEXTURESPROC glad_glDeleteTextures;
PFNGLTEXSUBIMAGE2DPROC glad_glTexSubImage2D;

static void* (*s_proc)(const char*) = nullptr;

static void* get_proc(const char *namez) {
    void* result = nullptr;
    if (s_proc) {
        result = s_proc(namez);
    }
    if (!result) {
        result = dlsym(RTLD_DEFAULT, namez);
    }
    return result;
}

extern "C" {

int gladLoadGL(void) {
    glad_glClearColor = (PFNGLCLEARCOLORPROC) get_proc("glClearColor");
    glad_glClear = (PFNGLCLEARPROC) get_proc("glClear");
    glad_glViewport = (PFNGLVIEWPORTPROC) get_proc("glViewport");
    glad_glEnable = (PFNGLENABLEPROC) get_proc("glEnable");
    glad_glDisable = (PFNGLDISABLEPROC) get_proc("glDisable");
    glad_glBlendFunc = (PFNGLBLENDFUNCPROC) get_proc("glBlendFunc");
    glad_glGenBuffers = (PFNGLGENBUFFERSPROC) get_proc("glGenBuffers");
    glad_glBindBuffer = (PFNGLBINDBUFFERPROC) get_proc("glBindBuffer");
    glad_glBufferData = (PFNGLBUFFERDATAPROC) get_proc("glBufferData");
    glad_glDeleteBuffers = (PFNGLDELETEBUFFERSPROC) get_proc("glDeleteBuffers");
    glad_glCreateShader = (PFNGLCREATESHADERPROC) get_proc("glCreateShader");
    glad_glShaderSource = (PFNGLSHADERSOURCEPROC) get_proc("glShaderSource");
    glad_glCompileShader = (PFNGLCOMPILESHADERPROC) get_proc("glCompileShader");
    glad_glGetShaderiv = (PFNGLGETSHADERIVPROC) get_proc("glGetShaderiv");
    glad_glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC) get_proc("glGetShaderInfoLog");
    glad_glDeleteShader = (PFNGLDELETESHADERPROC) get_proc("glDeleteShader");
    glad_glCreateProgram = (PFNGLCREATEPROGRAMPROC) get_proc("glCreateProgram");
    glad_glAttachShader = (PFNGLATTACHSHADERPROC) get_proc("glAttachShader");
    glad_glLinkProgram = (PFNGLLINKPROGRAMPROC) get_proc("glLinkProgram");
    glad_glGetProgramiv = (PFNGLGETPROGRAMIVPROC) get_proc("glGetProgramiv");
    glad_glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC) get_proc("glGetProgramInfoLog");
    glad_glUseProgram = (PFNGLUSEPROGRAMPROC) get_proc("glUseProgram");
    glad_glDeleteProgram = (PFNGLDELETEPROGRAMPROC) get_proc("glDeleteProgram");
    glad_glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC) get_proc("glGetUniformLocation");
    glad_glUniform1f = (PFNGLUNIFORM1FPROC) get_proc("glUniform1f");
    glad_glUniform2f = (PFNGLUNIFORM2FPROC) get_proc("glUniform2f");
    glad_glUniform4f = (PFNGLUNIFORM4FPROC) get_proc("glUniform4f");
    glad_glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC) get_proc("glGenVertexArrays");
    glad_glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC) get_proc("glBindVertexArray");
    glad_glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC) get_proc("glDeleteVertexArrays");
    glad_glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC) get_proc("glEnableVertexAttribArray");
    glad_glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC) get_proc("glVertexAttribPointer");
    glad_glDrawArrays = (PFNGLDRAWARRAYSPROC) get_proc("glDrawArrays");
    glad_glActiveTexture = (PFNGLACTIVETEXTUREPROC) get_proc("glActiveTexture");
    glad_glGenTextures = (PFNGLGENTEXTURESPROC) get_proc("glGenTextures");
    glad_glBindTexture = (PFNGLBINDTEXTUREPROC) get_proc("glBindTexture");
    glad_glTexImage2D = (PFNGLTEXIMAGE2DPROC) get_proc("glTexImage2D");
    glad_glTexParameteri = (PFNGLTEXPARAMETERIPROC) get_proc("glTexParameteri");
    glad_glDeleteTextures = (PFNGLDELETETEXTURESPROC) get_proc("glDeleteTextures");
    glad_glTexSubImage2D = (PFNGLTEXSUBIMAGE2DPROC) get_proc("glTexSubImage2D");
    return 1;
}

int gladLoadGLLoader(GLADloadproc proc) {
    s_proc = proc;
    return gladLoadGL();
}

} // extern "C"
