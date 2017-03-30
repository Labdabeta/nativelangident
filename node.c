#include "node.h"
#include <stdlib.h>
#include <stdio.h>

struct Node {
    double *weights;
    double last_output;
    double last_doutput;
    int size;
    activator act;
    activator dact;
};

struct Node *node_new(int num_inputs, activator act, activator dact, initializer init)
{
    int i;
    struct Node *ret = malloc(sizeof(struct Node));
    ret->weights = malloc(sizeof(double) * (num_inputs+1));
    ret->size = num_inputs;
    ret->act = act;
    ret->dact = dact;
    ret->last_output = 0;
    ret->last_doutput = 0;

    for (i = 0; i < num_inputs+1; ++i) ret->weights[i] = init();

    return ret;
}

void node_free(struct Node *n)
{
    if (n) free(n->weights);
    free(n);
}

int node_num_weights(const struct Node *n)
{
    return n->size + 1;
}

void node_set_weight(struct Node *n, int weight, double val)
{
    if (weight <= n->size)
        n->weights[weight] = val;
}

double node_get_weight(const struct Node *n, int weight)
{
    return n->weights[weight];
}

void node_delta_weight(struct Node *n, int weight, double dval)
{
    if (weight > n->size) {
        printf("Out of bounds!\n");
        return;
    }
    n->weights[weight] += dval;
    if (n->weights[weight] > 1.0)
        n->weights[weight] = 1.0;
    if (n->weights[weight] < -1.0)
        n->weights[weight] = -1.0;
}

double node_output(struct Node *n, double *inputs)
{
    int i;

    // Check for bias nodes
    if (n->size == 0) {
        n->last_output = 1.0;
        n->last_doutput = 0.0;
        return 1.0;
    }

    n->last_output = 0;
    for (i = 0; i < n->size; ++i) {
        n->last_output += n->weights[i] * inputs[i];
    }
    n->last_output += n->weights[n->size];

    n->last_doutput = n->dact(n->last_output);
    n->last_output = n->act(n->last_output);

    return n->last_output;
}

double node_last_output(const struct Node *n)
{
    return n->last_output;
}

double node_last_doutput(const struct Node *n)
{
    return n->last_doutput;
}
