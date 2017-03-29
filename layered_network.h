#ifndef LAYERED_NETWORK_H
#define LAYERED_NETWORK_H

#include "node.h"

struct Network;

/*
 * Num inputs = num_nodes[0]
 * Num outputs = num_nodes[num_layers-1]
 */
struct Network *network_new(int num_layers, int *num_nodes, activator *acts, activator *dacts);
void network_free(struct Network *n);

void network_value(const struct Network *n, double *inputs, double *outputs);

/* Outputs may be null, in which case they are discarded. */
void network_train(const struct Network *n, double *inputs, double *outputs, double *real_outputs, double rate);

#endif /* LAYERED_NETWORK_H */
