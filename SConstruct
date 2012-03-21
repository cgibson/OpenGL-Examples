import platform

env = Environment(CC = 'g++')
    
env.Append(CPPPATH = ['include'])
env.Append(CCFLAGS = ['-g', '-pthread', '-O2', '-Wall'])
env.Append(LIBS = ['Xrandr', 'rt', 'X11', 'GLU', 'GL', 'GLEW', 'm'])

objects = []

sources = [
           Glob("src/*.cpp")
           ]

# Build all modules within the source directory
for src in sources:
    objects += env.Object(src)

env.Program("quad", objects + ['test/quad.cpp','lib/libglfw.a'])