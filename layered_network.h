#ifndef LAYERED_NETWORK_H
#define LAYERED_NETWORK_H

#include "node.h"

struct Network;

struct Network *network_new(void);
void network_free(struct Network *n);
void network_add_layer(struct Network *n, int num_nodes, activator act, activator dact);
int network_get_num_inputs(const struct Network *n);
int network_get_num_outputs(const struct Network *n);

void network_value(const struct Network *n, double *inputs, double *outputs);

/* Outputs may be null, in which case they are discarded. */
void network_train(const struct Network *n, double *inputs, double *outputs, double *real_outputs, double rate);

#endif /* LAYERED_NETWORK_H */
