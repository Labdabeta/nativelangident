#include "layered_network.h"

#include <stdlib.h>

typedef struct Node **Layer;

struct Network {
    Layer *layers;
    int num_layers;
    int inputs;
    int outputs;
    int max_layer;
};

static double small_random_value(void)
{
    return 0.1;
}

static double one_producer(void)
{
    return 1.0;
}

static double identity(double x)
{
    return x;
}

struct Network *network_new(int num_layers, int *num_nodes, activator *acts, activator *dacts)
{
    int i;
    int last_size;
    struct Network *ret = malloc(sizeof(struct Network));
    ret->layers = malloc(sizeof(Layer) * num_layers);
    ret->max_layer = num_nodes[0];
    ret->inputs = num_nodes[0];
    ret->outputs = num_nodes[num_layers-1];

    last_size = ret->inputs;

    for (i = 1; i < num_layers; ++i) {
        int x;
        /* Extra node for bias */
        ret->layers[i] = malloc(sizeof(struct Node*) * (num_nodes[i]+1));
        for (x = 0; x < num_nodes[i]; ++x) {
            ret->layers[i][x] = node_new(last_size, acts[i], dacts[i], small_random_value);
        }

        ret->layers[i][num_nodes[i]] = node_new(0,

}

int network_get_num_inputs(const struct Network *n)
{
    if (!n->layers) return 0;

    return n->layers->num_nodes - 1; // subtract the bias
}

int network_get_num_outputs(const struct Network *n)
{
    struct LayerListNode *prev = NULL;
    struct LayerListNode *current = n->layers;

    while (current) {
        prev = current;
        current = current->next;
    }

    if (!prev) return 0;

    return prev->num_nodes - 1; // subtract the bias
}

void network_value(const struct Network *n, double *inputs, double *outputs)
{
    int i;
    double *values = malloc(sizeof(double) * n->max_layer);
    double *new_values = malloc(sizeof(double) * n->max_layer);
    struct LayerListNode *current = n->layers;

    /* initialize values */
    for (i = 0; i < n->num_inputs; ++i) {
        values[i] = inputs[i];
    }

    while (current) {
        double *swp;
        for (i = 0; i < current->num_nodes; ++i)
            new_values[i] = node_output(current->nodes[i], values);

        /* Pointer-swap values nad new_values */
        swp = values;
        values = new_values;
        new_values = swp;

        current = current->next;
    }

    if (outputs) {
        for (i = 0; i < n->outputs; ++i)
            outputs[i] = values[i];
    }

    free(values);
    free(new_values);
}

void network_train(const struct Network *n, double *inputs, double *outputs, double *real_outputs, double rate)
{
    int i;

    /* Step 1: Forward propogation */
    network_value(n, inputs, outputs);

    /* Step 2: Back propogation */

}
