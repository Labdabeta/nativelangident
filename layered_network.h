#ifndef LAYERED_NETWORK_H
#define LAYERED_NETWORK_H

#include "node.h"
#include <stdio.h>

struct Network;

/*
 * Num inputs = num_nodes[0]
 * Num outputs = num_nodes[num_layers-1]
 */
struct Network *network_new(int num_inputs, int num_layers, int *num_nodes, activator *acts, activator *dacts);
void network_free(struct Network *n);

void network_value(const struct Network *n, double *inputs, double *outputs);

/* Outputs may be null, in which case they are discarded. */
void network_train(struct Network *n, double *inputs, double *outputs, double *real_outputs, double rate);

/* Pretty-prints */
void network_dump(const struct Network *n, FILE *f);

/* Data based IO: */
void network_save(const struct Network *n, FILE *f);

void network_load(const struct Network *n, FILE *f);

#endif /* LAYERED_NETWORK_H */
