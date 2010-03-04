import SCons, os

sources = [
'EvoBlockSim.cc',
'api_adapter.c',
'evoBlock.c',
]

env = Environment()
env.Replace(CC = 'g++')
env.Append(CCFLAGS = '-O2 -fopenmp -Wall -ggdb -Wno-deprecated')
env.Append(CPPPATH = ['#'])

defaultTargets = []

for i in os.listdir('solutions'):
	filebits = os.path.splitext(i)
	if filebits[1] == '.c':
		defaultTargets.append('evoBlockSim-'+filebits[0]);
		env.Program('evoBlockSim-'+filebits[0], sources + ['solutions/'+i])

Default(defaultTargets)
