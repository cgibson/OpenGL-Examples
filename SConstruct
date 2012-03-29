import platform

env = Environment(CC = 'g++')
    
env.Append(CPPPATH = ['include'])
env.Append(CCFLAGS = ['-g', '-pthread', '-O2', '-Wall'])
env.Append(LIBS = [ 'Xrandr','rt', 'X11', 'GLU', 'GL', 'GLEW', 'm', 'IL', 'ILU', 'ILUT'])

objects = []

sources = [
           Glob("src/*.cpp")
           ]

programs = {
            "quad":["test/quad.cpp"],
            "matrix":["test/matrix.cpp"],
            "interleaved":["test/interleaved.cpp"],
            "strip":["test/strip.cpp"],
            "texture":["test/texture.cpp"]
            }

# Build all modules within the source directory
for src in sources:
    objects += env.Object(src)

bits, linkage = platform.architecture()
if bits == '64bit':
    archName = "x64"
else:
    archName = "x32"
    
for name,srcList in programs.iteritems():
   env.Program(name, objects + (srcList+['lib/%s/libglfw.a' % archName]))

