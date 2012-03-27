import platform

env = Environment(CC = 'g++')
    
env.Append(CPPPATH = ['include'])
env.Append(CCFLAGS = ['-g', '-pthread', '-O2', '-Wall'])
env.Append(LIBS = [ 'Xrandr','rt', 'X11', 'GLU', 'GL', 'GLEW', 'm'])

objects = []

sources = [
           Glob("src/*.cpp")
           ]

programs = {
            "quad":["test/quad.cpp"],
            "interleaved":["test/interleaved.cpp"],
            "strip":["test/strip.cpp"],
            "matrix":["test/matrix.cpp"]
           }

# Build all modules within the source directory
for src in sources:
    objects += env.Object(src)

if ARGUMENTS.get('do64', 0):
    archName = "x64"
else:
    archName = "x32"
    
for name,srcList in programs.iteritems():
   env.Program(name, objects + (srcList+['lib/%s/libglfw.a' % archName]))

