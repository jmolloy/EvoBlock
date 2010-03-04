/**@file   api_adapter.h
 * @author James Molloy <jamesm@osdev.org>
 * @date   Fri Feb 26 12:45:39 2010
 * @brief  Provides a better API to the EvoBlock. */

#ifndef API_ADAPTER_H
#define API_ADAPTER_H

#include "evoBlock.h"

/** Resets the truth table write index to zero, allowing load_truth_table
    to be called again. */
void reset_truth_table();
/** Loads 'x' into the n'th index of the truth table, where n is zero initially
    (following a reset) and increments with each call of this function. */
void load_truth_table(uint8_t x);

/** Performs a search, given a population in ArrayConfig format and the population
    size. The fitness (based on the truth table) is stored in population->fitness. */
void search(ArrayConfig *population, uint32_t pop_size, uint8_t num_inputs, unsigned int mask, unsigned int compare);

/** Sets the output mask. */
void set_output_mask(uint8_t m);

#endif
