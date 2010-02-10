#ifndef __evoBlock_H__
#define __evoBlock_H__

#include "EvoBlockSim.h"
#include <stdlib.h>

#define NUM_CELLS 40
#define NUM_CONFIG_BITS 20

typedef struct
{
	Xuint32 cellConfigs[NUM_CELLS];
	Xuint32 fitness;
} ArrayConfig;

void randomiseArrayConfig(ArrayConfig *config)
{
	Xuint32 cellNum = 0;
	Xuint32 randNum = 0;

	// Randomise cell configs
	for (cellNum=0 ; cellNum<NUM_CELLS ; cellNum++)
	{
		config->cellConfigs[cellNum] = rand();
	}
}

EvoBlockSim ebs = EvoBlockSim();
Xuint32 hack_vector = 0;

void EVOBLOCK_writeVector(Xuint32 vector) {
	hack_vector = vector;
}

void EVOBLOCK_writeTargetRAM(Xuint32 input) {
	ebs.SetTruthTable(hack_vector++, input);
}

void EVOBLOCK_writeMask(Xuint32 data) {
	ebs.SetMask(data);
}

void configureArray(Xuint32 arrayNum, ArrayConfig *config) {
	// TODO
}

Xuint32 EVOBLOCK_readMatch0() {
}

Xuint32 EVOBLOCK_readMatch1() {
}

#endif //__evoBlock_H__
