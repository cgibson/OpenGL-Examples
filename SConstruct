import platform
from os.path import join, basename
from os import path

env = Environment(CC = 'g++')

os = env["PLATFORM"]

buildPath = "build"
    
env.Append(CPPPATH = ['include'])
env.Append(CCFLAGS = ['-g', '-pthread', '-O2', '-Wall'])

print "THIS IS THE OS: " + os

if os == "darwin":
    env.Append(LINKFLAGS = ['-framework', 'OpenGL'])

    env.Append(CPPPATH = ['/usr/local/Cellar/devil/1.7.8/include',
                          '/usr/local/Cellar/glew/1.9.0/include',
                          '/usr/local/Cellar/glfw/2.7.8/include'])

    env.Append(LIBPATH = ['/usr/local/Cellar/devil/1.7.8/lib',
                          '/usr/local/Cellar/glew/1.9.0/lib', 
                          '/usr/local/Cellar/glfw/2.7.8/lib'])

    env.Append(LIBS = ['glfw','GLEW', 'IL', 'ILU', 'ILUT'])
else:
    env.Append(LIBS = ['Xrandr', 'rt', 'X11', 'GLU', 'GL', 'GLEW', 'm', 'IL', 'ILU', 'ILUT'])



objects = []

sources = Glob("src/*.cpp")

programs = {
            "quad":["test/quad.cpp"],
            "matrix":["test/matrix.cpp"],
            "interleaved":["test/interleaved.cpp"],
            "strip":["test/strip.cpp"],
            "texture":["test/texture.cpp"],
            "vectorvbo":["test/vectorvbo.cpp"],
            "ring":["test/ring.cpp"],
            "loadmesh":["test/loadmesh.cpp"],
            "envmap":["test/envmap.cpp"],
            "fbotest":["test/fbotest.cpp"],
            "vaotest":["test/vaotest.cpp"],
            "camera":["test/camera.cpp"],
            }

# Build all modules within the source directory
for file in sources:
    
    fileName = basename(str(file))
    fileName = fileName.split(".")[0]
    outDir = join(buildPath, fileName)
    
    objects += env.Object(outDir, file)

bits, linkage = platform.architecture()
if bits == '64bit':
    archName = "x64"
else:
    archName = "x32"

for name,srcList in programs.iteritems():
   env.Program(join(buildPath, name), objects + srcList) #+ (srcList+['lib/%s/%s/libglfw.a' % (archName, os)]))

