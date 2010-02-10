#ifndef __evoBlock_H__
#define __evoBlock_H__

#define NUM_CELLS 40
#define NUM_CONFIG_BITS 20

typedef struct
{
	Xuint32 cellConfigs[NUM_CELLS];
	Xuint32 fitness;
} ArrayConfig;

#endif //__evoBlock_H__
