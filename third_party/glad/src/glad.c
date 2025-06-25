#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glad/glad.h>

#ifdef _WIN32
#include <windows.h>
static HMODULE libgl;
typedef PROC (WINAPI *loadproc)(LPCSTR);
#else
#include <dlfcn.h>
static void *libgl;
typedef void *(*loadproc)(const char *);
#endif

static void *get_proc(const char *proc);
static int open_gl(void);
static void close_gl(void);

PFNGLCLEARPROC glClear;
PFNGLCLEARCOLORPROC glClearColor;
PFNGLENABLEPROC glEnable;
PFNGLDISABLEPROC glDisable;
PFNGLVIEWPORTPROC glViewport;
PFNGLDRAWELEMENTSPROC glDrawElements;
PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLCREATESHADERPROC glCreateShader;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLGETPROGRAMIVPROC glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
PFNGLDELETESHADERPROC glDeleteShader;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLUNIFORM1IPROC glUniform1i;
PFNGLUNIFORM1FPROC glUniform1f;
PFNGLUNIFORM3FPROC glUniform3f;
PFNGLUNIFORM4FPROC glUniform4f;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
PFNGLDELETEPROGRAMPROC glDeleteProgram;
PFNGLDELETEBUFFERSPROC glDeleteBuffers;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
PFNGLGETSTRINGPROC glGetString;

static int open_gl(void) {
#ifdef _WIN32
    libgl = LoadLibraryA("opengl32.dll");
    return libgl != NULL;
#else
    libgl = dlopen("libGL.so.1", RTLD_LAZY | RTLD_GLOBAL);
    if (!libgl) {
        libgl = dlopen("libGL.so", RTLD_LAZY | RTLD_GLOBAL);
    }
    return libgl != NULL;
#endif
}

static void close_gl(void) {
    if (libgl) {
#ifdef _WIN32
        FreeLibrary((HMODULE)libgl);
#else
        dlclose(libgl);
#endif
        libgl = NULL;
    }
}

static void *get_proc(const char *proc) {
    void *res;

#ifdef _WIN32
    res = (void *)GetProcAddress((HMODULE)libgl, proc);
#else
    res = dlsym(libgl, proc);
#endif
    return res;
}

int gladLoadGL(void) {
    return gladLoadGLLoader((void *(*)(const char *))get_proc);
}

int gladLoadGLLoader(void *(*load)(const char *name)) {
    if (!open_gl()) {
        return 0;
    }

    glClear = (PFNGLCLEARPROC)load("glClear");
    glClearColor = (PFNGLCLEARCOLORPROC)load("glClearColor");
    glEnable = (PFNGLENABLEPROC)load("glEnable");
    glDisable = (PFNGLDISABLEPROC)load("glDisable");
    glViewport = (PFNGLVIEWPORTPROC)load("glViewport");
    glDrawElements = (PFNGLDRAWELEMENTSPROC)load("glDrawElements");
    glGenBuffers = (PFNGLGENBUFFERSPROC)load("glGenBuffers");
    glBindBuffer = (PFNGLBINDBUFFERPROC)load("glBindBuffer");
    glBufferData = (PFNGLBUFFERDATAPROC)load("glBufferData");
    glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)load("glGenVertexArrays");
    glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)load("glBindVertexArray");
    glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)load("glVertexAttribPointer");
    glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)load("glEnableVertexAttribArray");
    glCreateShader = (PFNGLCREATESHADERPROC)load("glCreateShader");
    glShaderSource = (PFNGLSHADERSOURCEPROC)load("glShaderSource");
    glCompileShader = (PFNGLCOMPILESHADERPROC)load("glCompileShader");
    glCreateProgram = (PFNGLCREATEPROGRAMPROC)load("glCreateProgram");
    glAttachShader = (PFNGLATTACHSHADERPROC)load("glAttachShader");
    glLinkProgram = (PFNGLLINKPROGRAMPROC)load("glLinkProgram");
    glUseProgram = (PFNGLUSEPROGRAMPROC)load("glUseProgram");
    glGetShaderiv = (PFNGLGETSHADERIVPROC)load("glGetShaderiv");
    glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)load("glGetShaderInfoLog");
    glGetProgramiv = (PFNGLGETPROGRAMIVPROC)load("glGetProgramiv");
    glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)load("glGetProgramInfoLog");
    glDeleteShader = (PFNGLDELETESHADERPROC)load("glDeleteShader");
    glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)load("glGetUniformLocation");
    glUniform1i = (PFNGLUNIFORM1IPROC)load("glUniform1i");
    glUniform1f = (PFNGLUNIFORM1FPROC)load("glUniform1f");
    glUniform3f = (PFNGLUNIFORM3FPROC)load("glUniform3f");
    glUniform4f = (PFNGLUNIFORM4FPROC)load("glUniform4f");
    glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)load("glUniformMatrix4fv");
    glDeleteProgram = (PFNGLDELETEPROGRAMPROC)load("glDeleteProgram");
    glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)load("glDeleteBuffers");
    glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)load("glDeleteVertexArrays");
    glGetString = (PFNGLGETSTRINGPROC)load("glGetString");

    return 1;
}