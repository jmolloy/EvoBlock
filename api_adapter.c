#include "api_adapter.h"

static unsigned int calcFitness(int x, uint8_t match);

void reset_truth_table() {
    EVOBLOCK_writeVector(0);
}

void load_truth_table(uint8_t x) {
    EVOBLOCK_writeTargetRAM(x);
}

void set_output_mask(uint8_t m) {
    EVOBLOCK_writeMask(m);
}

void search(ArrayConfig *population, uint32_t pop_size, uint8_t num_inputs, unsigned int mask, unsigned int compare) {
    
    unsigned int vector_max = 1 << num_inputs;    

    /* TODO: The *8's here: not sure if gcc's optimisations are enabled during
       build; check. Possible reduce manually to shifts. */
    unsigned int set;
    for(set = 0; set*8 < pop_size; set++) {
        // Configure the 8 Cell Arrays
        configureArray(0, &population[set*8]);
        configureArray(1, &population[(set*8)+1]);
        configureArray(2, &population[(set*8)+2]);
        configureArray(3, &population[(set*8)+3]);
        configureArray(4, &population[(set*8)+4]);
        configureArray(5, &population[(set*8)+5]);
        configureArray(6, &population[(set*8)+6]);
        configureArray(7, &population[(set*8)+7]);

        int i;
        unsigned int outputMatchData;
        unsigned int outputMatch;
        unsigned int fitness;
        for(i = 0; i < vector_max; i++) {
            if( (i & mask) != compare) continue;

            EVOBLOCK_writeVector(i);

            // Evaluate Arrays 0 to 3
            outputMatchData = EVOBLOCK_readMatch0();

            outputMatch = outputMatchData & 0xFF;
            fitness = calcFitness(i, outputMatch);
            population[set*8].fitness += fitness;

            outputMatch = (outputMatchData >> 8) & 0xFF;
            fitness = calcFitness(i, outputMatch);
            population[(set*8)+1].fitness += fitness;

            outputMatch = (outputMatchData >> 16) & 0xFF;
            fitness = calcFitness(i, outputMatch);
            population[(set*8)+2].fitness += fitness;

            outputMatch = (outputMatchData >> 24) & 0xFF;
            fitness = calcFitness(i, outputMatch);
            population[(set*8)+3].fitness += fitness;

            // Evaluate Arrays 4 to 7
            outputMatchData = EVOBLOCK_readMatch1();

            outputMatch = outputMatchData & 0xFF;
            fitness = calcFitness(i, outputMatch);
            population[(set*8)+4].fitness += fitness;

            outputMatch = (outputMatchData >> 8) & 0xFF;
            fitness = calcFitness(i, outputMatch);
            population[(set*8)+5].fitness += fitness;

            outputMatch = (outputMatchData >> 16) & 0xFF;
            fitness = calcFitness(i, outputMatch);
            population[(set*8)+6].fitness += fitness;

            outputMatch = (outputMatchData >> 24) & 0xFF;
            fitness = calcFitness(i, outputMatch);
            population[(set*8)+7].fitness += fitness;
        }

    }

}

unsigned int calcFitness(int input, uint8_t match) {
    /* TODO: make this much faster with a lookup table. */
    unsigned int bit;
	unsigned int fitness = 0;

    // Only add and graycode have 3 bit outputs.
    if( (input >> 5) != 0x00 &&
        (input >> 5) != 0x06 ) {
        match |= 4;
    }

	for (bit=0 ; bit<8 ; bit++)
	{
		if ((match & 0x1) == 1) fitness += 1;
		match >>= 1;
	}

	return fitness;

}
