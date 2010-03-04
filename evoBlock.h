#ifndef __evoBlock_H__
#define __evoBlock_H__

#include "xparameters.h"
#include "xio.h"

#include "EvoBlockSim.h"
#include <stdlib.h>

typedef Xuint32 uint32_t;
typedef Xuint8 uint8_t;

#define NUM_CELLS 40
#define NUM_CONFIG_BITS 20

typedef struct
{
	Xuint32 cellConfigs[NUM_CELLS];
	Xuint32 fitness;
} ArrayConfig;

void randomiseArrayConfig(ArrayConfig *config);
void EVOBLOCK_writeVector(Xuint32 vector);
void EVOBLOCK_writeTargetRAM(Xuint32 input);
void EVOBLOCK_writeMask(Xuint32 data);
void configureArray(Xuint32 arrayNum, ArrayConfig *config);
Xuint32 EVOBLOCK_readMatch0();
Xuint32 EVOBLOCK_readMatch1();

#endif //__evoBlock_H__
