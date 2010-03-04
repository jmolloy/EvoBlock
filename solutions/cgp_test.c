#include "xparameters.h"
#include "xio.h"

#include "evoBlock.h"
#include "api_adapter.h"

#include <unistd.h>

#define POP_SIZE 64
#define GEN_MAX 12500
#define MUTATION_RATE 50
#define MUTATION_PULSE_RATE 500

#define OPCODE(x) (x>>5)
#define OPC_ADD 0
#define OPC_AND 1
#define OPC_OR  2
#define OPC_XOR 3
#define OPC_NOT_A 4
#define OPC_NOT_B 5

typedef struct candidate {
    uint32_t genotype[NUM_CELLS]; // IN_A, IN_B, IN_C, FUNC - 4 : 4 : 4 : 4
    unsigned int fitness;
} candidate_t;

typedef int (*truth_table_fn_t)(int);

candidate_t candidates[POP_SIZE];
ArrayConfig configs[POP_SIZE];

uint8_t functions[] = { /*0 */  0x00,
                        /*1 */  0xFF,
                        /*AND*/ 0x80,
                        /*OR*/  0x01,
                        /*NAND*/0x7F,
                        /*NOR*/ 0xFE,
                        /*XOR*/ 0x16,
                        /*XNOR*/0xE9,
                        /*I1*/  0xF0,
                        /*I2*/  0xCC,
                        /*I3*/  0xAA,
                        /*¬I1*/ 0x0F,
                        /*¬I2*/ 0x33,
                        /*¬I3*/ 0x55,
                        /*0 */  0x00, /* Run out of functions :( */
                        /*1 */  0xFF};/* Any suggestions? */

candidate_t *run(truth_table_fn_t fn, unsigned int mask, unsigned int max_fitness, unsigned int _mask, unsigned int compare, candidate_t *start_candidate);
void gen_configs();
void mutate_population(candidate_t *parent, int mutation_rate);
candidate_t *randomise_population();

int two_bit_adder(int n) {
    if(OPCODE(n) == OPC_ADD) {
        int x = (n&0x03) + ((n>>2)&0x03);
        return x;
    } else {
        return 0;
    }
}

int logic_and(int n) {
    if(OPCODE(n) == OPC_AND) {
            int x = (n&0x03) & ((n>>2)&0x03);
            return x;
    } else return two_bit_adder(n);
}
int logic_or(int n) {
    if(OPCODE(n) == OPC_OR) {
            int x = (n&0x03) | ((n>>2)&0x03);
            return x;
    } else return logic_and(n);
}
int logic_xor(int n) {
    if(OPCODE(n) == OPC_XOR) {
            int x = (n&0x03) ^ ((n>>2)&0x03);
            return x;
    } else return logic_or(n);
}
int logic_not_a(int n) {
    if(OPCODE(n) == OPC_NOT_A) {
        unsigned int x = ~n & 0x03;
        return x;
    } else return logic_xor(n);
}
int logic_not_b(int n) {
    if(OPCODE(n) == OPC_NOT_B) {
        unsigned int x = (~n >> 2) & 0x03;
        return x;
    } else return logic_not_a(n);
}


typedef struct target {
    truth_table_fn_t fn;
    unsigned int num_inputs;
    unsigned int num_outputs;
    unsigned int mask;
    unsigned int compare;
} target_t;

int num_targets = 6;
target_t targets[] = {
    {&two_bit_adder, 8, 3, 0xF0, 0x00},
    {&logic_and, 8, 3, 0xD0, 0x00},
    {&logic_or, 8, 3, 0x90, 0x00},
    {&logic_xor, 8, 3, 0x90, 0x60},
    {&logic_not_a, 8, 3, 0x01, 0x00},
    {&logic_not_b, 8, 3, 0x01, 0x00}
};

int main(void) {
    srand(getpid());


    candidate_t *c = 0;
    int i;
//    for(i = 5; i < num_targets; i++) {
    i = 2;
        target_t t = targets[i];
        c = run(t.fn, t.num_inputs, t.num_outputs, t.mask, t.compare, c);
        if(!c) {
            exit(1);
        }
//    }
}

candidate_t *run(truth_table_fn_t fn, unsigned int num_inputs, unsigned int num_outputs, unsigned int _mask, unsigned int compare, candidate_t *start_candidate) {

    unsigned int i,g;

    // Load the truth table.
    reset_truth_table();
    for(i = 0; i < 256; i++) {
        load_truth_table(fn(i));
    }

    // Number of masked bits.
    unsigned int num_masked = 0;
    for(i = 0; i < 8; i++) {
        if(_mask & (1UL << i)) {
            num_masked ++;
        }
    }

    // Calculate max fitness = 2^(num_inputs-num_masked)  * num_outputs
    unsigned int max_fitness = (1UL << (num_inputs-num_masked)) * num_outputs;


    uint8_t mask = 0;
    for(i = 0; i < num_outputs; i++) {
        mask <<= 1;
        mask |= 1;
    }

    // Set the mask.
    set_output_mask(mask);

    // If we have a starting candidate, seed the population from that. Else,
    // randomise it.
    if(start_candidate) {
        mutate_population(start_candidate, MUTATION_RATE);
    } else {
        start_candidate = randomise_population();
    }

    int pulse_ctr = MUTATION_PULSE_RATE;
    int mutation_rate = MUTATION_RATE;
    candidate_t parent = *start_candidate;
    for(g = 0; g < GEN_MAX; g++) {

        // Create the ArrayConfigs.
        gen_configs();
        
        // Perform a search iteration.
        search(configs, POP_SIZE, num_inputs, _mask, compare);

        // Inspect the result.
        candidate_t *winner = &parent;
        for(i = 0; i < POP_SIZE; i++) {
            // Always >= for diversity.
            candidates[i].fitness = configs[i].fitness;
            if(configs[i].fitness >= winner->fitness) {
                winner = &candidates[i];
            }
        }

        //// Display some generation information
        //if((g % 100) == 0) {
        //xil_printf("\rGen: %3d, fittest: %3d / %3d",
        //           g, winner->fitness, max_fitness);
        // }

        if(winner->fitness == max_fitness) {
            xil_printf("Gen: %3d, fittest: %3d / %3d",
                       g, winner->fitness, max_fitness);
            xil_printf("\nSolution found!\n");
            return winner;
        }

        parent = *winner;

        if(pulse_ctr == 0) {
            mutation_rate = rand() & 0xFF;
            pulse_ctr = MUTATION_PULSE_RATE;
        }

        // Mutate population ready for next iteration.
        mutate_population(&parent, mutation_rate);

        mutation_rate = MUTATION_RATE;
    }

    xil_printf("Solution not found after %d generations.\n", GEN_MAX);
    return 0;
}

void gen_configs() {
    int i, j;
    for(i = 0; i < POP_SIZE; i++) {
        configs[i].fitness = 0;
        for(j = 0; j < NUM_CELLS; j++) {
            configs[i].cellConfigs[j]  = (candidates[i].genotype[j] << 4) & 0xFFF00;
            configs[i].cellConfigs[j] |= functions[candidates[i].genotype[j]&0xF];
        }
    }
}

void mutate_population(candidate_t *parent, int mutation_rate) {
    candidate_t p = *parent;

    // Don't do stochastic mutation as that's too compute expensive for the board.
    // Algorithm:
    //
    // For every candidate in the population, mutate (exactly)
    // MUTATION_RATE times.  Pick a number 'x' between
    // 0..NUM_CELLS. Increment through all cells, decrementing x. When
    // x==0, perform a mutation. Mutate by flipping a random bit.

    unsigned int i,j;
    for(i = 0; i < POP_SIZE; i++) {
        for(j = 0; j < NUM_CELLS; j++) {
            candidates[i].genotype[j] = p.genotype[j];
        }
    }

    for(i = 0; i < POP_SIZE; i++) {
        for(j = 0; j < (unsigned int)mutation_rate; j++) {
            unsigned int x = rand() % NUM_CELLS;
            unsigned int num = rand();
            // Flip bit.
            candidates[i].genotype[x] = p.genotype[x] ^ (1UL << (num&0xF));
        }
    }
}

candidate_t *randomise_population() {
    int i,j;
    for(j = 0; j < POP_SIZE; j++) {
        for(i = 0; i < NUM_CELLS; i++) {
            candidates[j].genotype[i] = rand() & 0x0000FFFF;
        }
    }
    // Always return the first pop member as the parent.
    return &candidates[0];
}

