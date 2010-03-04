#include "evoBlock.h"

void randomiseArrayConfig(ArrayConfig *config)
{
	Xuint32 cellNum = 0;

	// Randomise cell configs
	for (cellNum=0 ; cellNum<NUM_CELLS ; cellNum++)
	{
		config->cellConfigs[cellNum] = rand();
	}
}

EvoBlockSim ebs[8];
Xuint32 hack_vector = 0;

void EVOBLOCK_writeVector(Xuint32 vector) {
	hack_vector = vector;
}

void EVOBLOCK_writeTargetRAM(Xuint32 input) {
	ebs[0].SetTruthTable(hack_vector, input);
	ebs[1].SetTruthTable(hack_vector, input);
	ebs[2].SetTruthTable(hack_vector, input);
	ebs[3].SetTruthTable(hack_vector, input);
	ebs[4].SetTruthTable(hack_vector, input);
	ebs[5].SetTruthTable(hack_vector, input);
	ebs[6].SetTruthTable(hack_vector, input);
	ebs[7].SetTruthTable(hack_vector++, input);
}

void EVOBLOCK_writeMask(Xuint32 data) {
	ebs[0].SetMask(data);
	ebs[1].SetMask(data);
	ebs[2].SetMask(data);
	ebs[3].SetMask(data);
	ebs[4].SetMask(data);
	ebs[5].SetMask(data);
	ebs[6].SetMask(data);
	ebs[7].SetMask(data);
}

void configureArray(Xuint32 arrayNum, ArrayConfig *config) {
	for(int i=0; i < 5; ++i)
		for (int j=0; j < 8; ++j)
			ebs[arrayNum].Program(i, j, config->cellConfigs[i*8 + j]);
}

// Arrays 0 - 3
Xuint32 EVOBLOCK_readMatch0() {
	Xuint32 result = ebs[0].Calc(hack_vector) | (ebs[1].Calc(hack_vector) << 8) | (ebs[2].Calc(hack_vector) << 16) | (ebs[3].Calc(hack_vector) << 24);
	return result;
}

// Arrays 4-7
Xuint32 EVOBLOCK_readMatch1() {
	Xuint32 result = ebs[4].Calc(hack_vector) | (ebs[5].Calc(hack_vector) << 8) | (ebs[6].Calc(hack_vector) << 16) | (ebs[7].Calc(hack_vector) << 24);
	hack_vector++;
	return result;
}
