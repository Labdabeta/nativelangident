#ifndef LAYER_H
#define LAYER_H

#include "node.h"

struct Layer;

struct Layer *layer_new(int num_nodes, activator act, activator dact, int isInput);
void layer_connect(struct Layer *input, struct Layer *output);


#endif /* LAYER_H */
