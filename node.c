#include "node.h"
#include <stdlib.h>

struct Node {
    double *weights;
    double last_output;
    int size;
    activator act;
    activator dact;
};

struct Node *node_new(int num_inputs, activator act, activator dact, initializer init)
{
    int i;
    struct Node *ret = malloc(sizeof(struct Node));
    ret->weights = malloc(sizeof(double) * num_inputs);
    ret->inputs = malloc(sizeof(double) * num_inputs);
    ret->size = num_inputs;
    ret->act = act;
    ret->dact = dact;

    for (i = 0; i < num_inputs; ++i) ret->weights[i] = init();

    return ret;
}

void node_free(struct Node *n)
{
    if (n) free(n->weights);
    free(n);
}

double node_output(struct Node *n, double *inputs)
{
    int i;

    // Check for bias nodes
    if (n->size == 0) return 1.0;

    n->last_output = 0;
    for (i = 0; i < n->size; ++i) {
        n->last_output += n->weights[i] * inputs[i];
    }

    return act(n->last_output);
}

double node_last_value(const struct Node *n)
{
    return n->last_output;
}
