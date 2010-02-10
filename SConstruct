import SCons, os

sources = [
'exp1.c',
'EvoBlockSim.cc',
]

env = Environment()
env.Replace(CC = 'g++')

env.Program('EvoBlockSim', sources)
env.Default('EvoBlockSim')
