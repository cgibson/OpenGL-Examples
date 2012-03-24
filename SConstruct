import platform

env = Environment(CC = 'g++')
    
env.Append(CPPPATH = ['include'])
env.Append(CCFLAGS = ['-g', '-pthread', '-O2', '-Wall'])
env.Append(LIBS = [ 'Xrandr','rt', 'X11', 'GLU', 'GL', 'GLEW', 'm'])

objects = []

sources = [
           Glob("src/*.cpp")
           ]

# Build all modules within the source directory
for src in sources:
    objects += env.Object(src)

if ARGUMENTS.get('do64', 0):
	env.Program("quad", objects + ['test/quad.cpp', 'lib/x64/libglfw.a'])
else:
	env.Program("quad", objects + ['test/quad.cpp', 'lib/x32/libglfw.a'])

#env.Program("quad", objects + ['test/quad.cpp', 'lib/x32/libglfw.a'])
