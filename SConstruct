import platform
from os.path import join, basename
from os import path

env = Environment(CC = 'g++')

buildPath = "build"
    
env.Append(CPPPATH = ['include'])
env.Append(CCFLAGS = ['-g', '-pthread', '-O2', '-Wall'])
env.Append(LIBS = [ 'Xrandr','rt', 'X11', 'GLU', 'GL', 'GLEW', 'm', 'IL', 'ILU', 'ILUT'])

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
   env.Program(join(buildPath, name), objects + (srcList+['lib/%s/libglfw.a' % archName]))

