import SCons, os

sources = [
'exp1.c',
'EvoBlockSim.cc',
]

env = Environment()
env.Replace(CC = 'g++')
env.Append(CXXFLAGS = '-O2 -fopenmp -Wall -ggdb -Wno-deprecated')

env.Program('EvoBlockSim', sources)
env.Default('EvoBlockSim')
