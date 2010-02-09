#include "xparameters.h"
#include "xio.h"

#include "evoBlock.h"

#define POP_SIZE			128		// The Population size
#define SET_SIZE			16			// The number of sets, always POP_SIZE / 8
#define TOURNAMENT_SIZE	5			// The number of individuals used in tournament selection
#define MUTATION_RATE	2			// Mutate MUTATION_RATE out of 256 times.
#define MAX_FITNESS		16			// Max fitness
#define MAX_GEN			500		// Maximum number of generations per run

// Prototypes
void loadTargetTruthTable();
void randomisePopulation(ArrayConfig *pop, Xuint32 popSize);
void mutateArrayConfig(ArrayConfig *source, ArrayConfig *dest, Xuint32 mutationRate);
Xuint32 tournamentSelect(ArrayConfig* population, Xuint32 popSize, Xuint32 tournamentSize);
Xuint32 calcFitness(Xuint32 diffData);

// Globals
ArrayConfig pop1[POP_SIZE];
ArrayConfig pop2[POP_SIZE];

ArrayConfig *population;
ArrayConfig *nextPopulation;
ArrayConfig *tempPopulation;

int main(void)
{
	print("Starting Experiment 1\n");

	Xuint32 p = 0;
	Xuint32 gen = 0;
	Xuint32 set = 0;
	Xuint32 vector = 0;
	Xuint32 outputMatchData, outputMatch;
	Xuint32 outputData, output;
	Xuint32 fitness;
	Xuint32 leastFit;
	Xuint32 averageFitness;
	Xuint32 fittest;
	Xuint32 parent1, parent2;
	Xuint32 tournament;
	Xuint32 randNum = 0;
	Xuint32 searching;

	// Load the EvoBlock Target RAM with the target truth table
	loadTargetTruthTable();

	// Set Mask
	// Only 2 outputs. COut Bit 1, Sum bit 0
	EVOBLOCK_writeMask(0x3);

	while(1)
	{
		// Print out the settings for the run
		xil_printf("Population size: %d, tournamentSize: %d, mutationRate: %d\n",
						POP_SIZE, TOURNAMENT_SIZE, MUTATION_RATE);

		// Initialise Populations
		population = pop1;
		nextPopulation = pop2;
		randomisePopulation(population, POP_SIZE);

		gen = 0;
		searching = 1;

		while(searching)
		{
			// Reset each individual's fitness to 0
			for (p=0 ; p<POP_SIZE ; p++) population[p].fitness = 0;

			for (set=0 ; set<SET_SIZE ; set++)
			{
				// Configure the 8 Cell Arrays
				configureArray(0, &population[set*8]);
				configureArray(1, &population[(set*8)+1]);
				configureArray(2, &population[(set*8)+2]);
				configureArray(3, &population[(set*8)+3]);
				configureArray(4, &population[(set*8)+4]);
				configureArray(5, &population[(set*8)+5]);
				configureArray(6, &population[(set*8)+6]);
				configureArray(7, &population[(set*8)+7]);

				// Iterate through input vectors
				// 8 vectors as there are 3 inputs (2^3=8)
				for (vector=0; vector<8 ; vector++)
				{
					EVOBLOCK_writeVector(vector);

					// Evaluate Arrays 0 to 3
					outputMatchData = EVOBLOCK_readMatch0();

					outputMatch = outputMatchData & 0xFF;
					fitness = calcFitness(outputMatch);
					population[set*8].fitness += fitness;

					outputMatch = (outputMatchData >> 8) & 0xFF;
					fitness = calcFitness(outputMatch);
					population[(set*8)+1].fitness += fitness;

					outputMatch = (outputMatchData >> 16) & 0xFF;
					fitness = calcFitness(outputMatch);
					population[(set*8)+2].fitness += fitness;

					outputMatch = (outputMatchData >> 24) & 0xFF;
					fitness = calcFitness(outputMatch);
					population[(set*8)+3].fitness += fitness;

					// Evaluate Arrays 4 to 7
					outputMatchData = EVOBLOCK_readMatch1();

					outputMatch = outputMatchData & 0xFF;
					fitness = calcFitness(outputMatch);
					population[(set*8)+4].fitness += fitness;

					outputMatch = (outputMatchData >> 8) & 0xFF;
					fitness = calcFitness(outputMatch);
					population[(set*8)+5].fitness += fitness;

					outputMatch = (outputMatchData >> 16) & 0xFF;
					fitness = calcFitness(outputMatch);
					population[(set*8)+6].fitness += fitness;

					outputMatch = (outputMatchData >> 24) & 0xFF;
					fitness = calcFitness(outputMatch);
					population[(set*8)+7].fitness += fitness;
				}
			}

			fittest = 0;
			leastFit = 0;
			averageFitness = 0;

			for (p=0 ; p<POP_SIZE ; p++)
			{
				if (population[p].fitness > population[fittest].fitness) fittest = p;
				if (population[p].fitness < population[leastFit].fitness) leastFit = p;
				averageFitness += population[p].fitness;
			}

			averageFitness /= POP_SIZE;

			// Display some generation information
			xil_printf("Gen: %3d, fittest: %3d (%3d), Config: %08X, leastFit: %3d (%3d), average %3d\n",
							gen, fittest, population[fittest].fitness, population[fittest].cellConfigs[0],
							leastFit, population[leastFit].fitness, averageFitness);

			// Check if solution found
			if (population[fittest].fitness == MAX_FITNESS)
			{
				xil_printf("Solution Found, Gen: %d, fittest: %3d (%3d)\n", gen, fittest, population[fittest].fitness);

				for (p=0 ; p<NUM_CELLS ; p++) xil_printf("Cell %2d config: %08X\n", p, population[fittest].cellConfigs[p]);

				searching = 0;
				continue;
			}

			// If reached the generation limit, end this run
			if (gen == MAX_GEN)
			{
				xil_printf("Solution Not found, Gen: %d, fittest: %3d (%3d)\n", gen, fittest, population[fittest].fitness);
				searching = 0;
				continue;
			}

			// Create next population
			for (tournament=0 ; tournament<POP_SIZE ; tournament++)
			{
				// Use Tournament Selection to get one individual
				parent1 = tournamentSelect(population, POP_SIZE, TOURNAMENT_SIZE);

				// Mutate the individual and place into next population
				mutateArrayConfig(&population[parent1], &nextPopulation[tournament], MUTATION_RATE);
			}

			// Swap Populations
			tempPopulation = population;
			population = nextPopulation;
			nextPopulation = tempPopulation;

			// Increment the generation number
			gen++;
		}
	}
}

// ----------------------------------------------------------------------------------------------------------
// loadTargetTruthTable
// Load the target RAM with the target truth table
// ----------------------------------------------------------------------------------------------------------
void loadTargetTruthTable()
{
	// Initialise vector to 0
	// This will automatically increment with every write to the Target RAM
	EVOBLOCK_writeVector(0);

	Xuint32 vector;
	Xuint32 data;

	// Iterate though all test vectors loading the correct output value
	for(vector=0 ; vector<256 ; vector++)
	{
		// Calculate the output value
		data = ((vector>>2)&0x1) + ((vector>>1)&0x1) + (vector&0x1);		// B + A + CIn

		// Load the Target RAM
		EVOBLOCK_writeTargetRAM(data);
	}
}

// ----------------------------------------------------------------------------------------------------------
// randomisePopulation
// Iterate through the population randomizin each individual
// ----------------------------------------------------------------------------------------------------------
void randomisePopulation(ArrayConfig *pop, Xuint32 popSize)
{
	Xuint32 individual;

	// Iterate though the population
	for(individual=0 ; individual<popSize ; individual++)
	{
		// Randomise the array configuration
		randomiseArrayConfig(&pop[individual]);
	}
}

// ----------------------------------------------------------------------------------------------------------
// tournamentSelect
// Select 'tournamentSize' individuals randomly from 'population', return the number of the fittest
//	Note: Contenders can the same
// ----------------------------------------------------------------------------------------------------------
Xuint32 tournamentSelect(ArrayConfig* population, Xuint32 popSize, Xuint32 tournamentSize)
{
	Xuint32 p;
	Xuint32 randNum;
	Xuint32 contender[tournamentSize];

	// Select random contenders
	for (p=0 ; p<tournamentSize ; p++)
	{
		// Get a random individual number
		randNum = rand() % popSize;

		// Store contender number
		contender[p] = randNum;
	}

	// Find Fittest Contender
	Xuint32 fittest = contender[0];

	for (p=0 ; p<tournamentSize ; p++)
	{
		if (population[contender[p]].fitness > population[fittest].fitness) fittest = contender[p];
	}

	return fittest;
}

// ----------------------------------------------------------------------------------------------------------
// mutateArrayConfig
// Iterate through cell configs, flip config bit if randum number (0 to 255) is less that MUTATION_RATE
// ----------------------------------------------------------------------------------------------------------
void mutateArrayConfig(ArrayConfig *source, ArrayConfig *dest, Xuint32 mutationRate)
{
	Xuint32 cellNum;
	Xuint32 bit;
	Xuint32 config;
	Xuint32 randNum;

	for (cellNum=0 ; cellNum<NUM_CELLS ; cellNum++)
	{
		// Get source config
		config = source->cellConfigs[cellNum];

		for (bit=0 ; bit<NUM_CONFIG_BITS ; bit++)
		{
			// If mutating, flip bit
			randNum = rand() & 0xFF;
			if (randNum < mutationRate) config ^= (1 << bit);
		}

		// Write config to destination
		dest->cellConfigs[cellNum] = config;
	}
}

// ----------------------------------------------------------------------------------------------------------
// calcFitness
// Sum the number of '1' in the matchData (This is the number of matching bits)
// ----------------------------------------------------------------------------------------------------------
Xuint32 calcFitness(Xuint32 matchData)
{
	Xuint32 bit;
	Xuint32 fitness = 0;

	for (bit=0 ; bit<8 ; bit++)
	{
		if ((matchData & 0x1) == 1) fitness += 1;
		matchData >>= 1;
	}

	return fitness;
}
