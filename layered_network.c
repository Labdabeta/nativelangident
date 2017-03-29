#include "layered_network.h"

#include <stdlib.h>

typedef struct Node **Layer;

struct Network {
    Layer *layers;
    activator *act;
    activator *dact;
    int num_layers;
    int *num_nodes;
    int inputs;
    int outputs;
    int max_layer;
};

static double small_random_value(void)
{
    return 0.1;
}

static double get_back_last_output(struct Network *n, double *inputs, int layer, int weight)
{
    if (layer > 0)
        return node_last_output(n->layers[layer-1][weight]);
    else
        return inputs[weight];
}

struct Network *network_new(int num_inputs, int num_layers, int *num_nodes, activator *acts, activator *dacts)
{
    int i;
    int last_size;
    struct Network *ret = malloc(sizeof(struct Network));
    ret->layers = malloc(sizeof(Layer) * num_layers);
    ret->act = malloc(sizeof(activator) * num_layers);
    ret->dact = malloc(sizeof(activator) * num_layers);
    ret->max_layer = num_nodes[0];
    ret->num_nodes = malloc(sizeof(int) * num_layers);
    ret->inputs = num_inputs;
    ret->outputs = num_nodes[num_layers-1];
    ret->num_layers = num_layers;

    last_size = ret->inputs;

    for (i = 0; i < num_layers; ++i) {
        int x;
        ret->layers[i] = malloc(sizeof(struct Node*) * num_nodes[i]);
        for (x = 0; x < num_nodes[i]; ++x)
            ret->layers[i][x] = node_new(last_size, acts[i], dacts[i], small_random_value);
        if (num_nodes[i] > ret->max_layer)
            ret->max_layer = num_nodes[i];
        last_size = num_nodes[i];
        ret->num_nodes[i] = num_nodes[i];
        ret->act[i] = acts[i];
        ret->dact[i] = dacts[i];
    }

    return ret;
}

void network_free(struct Network *n)
{
    if (n) {
        int i;
        for (i = 0; i < n->num_layers; ++i) {
            int x;
            for (x = 0; x < n->num_nodes[i]; ++x)
                node_free(n->layers[i][x]);
            free(n->layers[i]);
        }
        free(n);
    }
}

void network_value(const struct Network *n, double *inputs, double *outputs)
{
    int i;
    double *values = malloc(sizeof(double) * n->max_layer);
    double *new_values = malloc(sizeof(double) * n->max_layer);

    /* initialize values */
    for (i = 0; i < n->inputs; ++i) {
        values[i] = inputs[i];
    }

    for (i = 0; i < n->num_layers; ++i) {
        int x;
        double *swp;
        for (x = 0; x < n->num_nodes[i]; ++x)
            new_values[x] = node_output(n->layers[i][x], values);

        /* Pointer-swap values nad new_values */
        swp = values;
        values = new_values;
        new_values = swp;
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
    int layer,node;
    double **deltas;

    /* Step 1: Forward propogation */
    network_value(n, inputs, outputs);

    /* Step 2: Construct Deltas */
    deltas = malloc(sizeof(double*) * n->num_layers);
    for (layer = 0; layer < n->num_layers; ++layer) {
        deltas[layer] = malloc(sizeof(double) * n->num_nodes[layer]);
        for (node = 0; node < n->num_nodes[layer]; ++node)
            deltas[layer][node] = 0.0;
    }

    /* Step 2a: Output deltas */
    layer = n->num_layers - 1;
    for (node = 0; node < n->num_nodes[layer]; ++node) {
        double val, dval, target;
        struct Node *x = n->layers[layer][node];

        val = node_last_output(x);
        dval = node_last_doutput(x);
        target = real_outputs[node];
        deltas[layer][node] = dval * (val - target) * (val - target);
    }

    /* Step 2b: Hidden layer deltas */
    for (layer = n->num_layers - 2; layer >= 0; --layer) {
        for (node = 0; node < n->num_nodes[layer]; ++node) {
            int w, num_weights;
            double delta, last_output;
            struct Node *x = n->layers[layer][node];

            delta = 0.0;
            for (w = 0; w < n->num_nodes[layer+1]; ++w) {
                struct Node *otherNode = n->layers[layer+1][w];
                delta += deltas[layer+1][w] * node_get_weight(otherNode, node);
            }

            deltas[layer][node] = node_last_doutput(x) * delta;
        }
    }

    /* Step 3: Back propogation */
    for (layer = 0; layer < n->num_layers; ++layer) {
        for (node = 0; node < n->num_nodes[layer]; ++node) {
            int weight, num_weights;
            struct Node *x = n->layers[layer][node];

            num_weights = node_num_weights(x);
            for (weight = 0; weight < num_weights; ++weight)
                node_delta_weight(x, weight, rate * deltas[layer][node] * node_last_output(x));
            /* Update the bias */
            node_delta_weight(x, num_weights, rate * deltas[layer][node]);
        }
    }
}

void network_dump(const struct Network *n, FILE *f)
{
    int layer, node, weight;

    for (layer = 0; layer < n->num_layers; ++layer) {
        fprintf(f,"Layer %d:\n",layer);
        for (node = 0; node < n->num_nodes[layer]; ++node) {
            int num_weights;
            struct Node *x = n->layers[layer][node];

            fprintf(f,"\tNode %d: ",node);

            num_weights = node_num_weights(x);
            for (weight = 0; weight < num_weights-1; ++weight)
                fprintf(f,"%lf,",node_get_weight(x,weight));
            fprintf(f,"%lf\n",node_get_weight(x,num_weights-1));
        }
    }
}

#define WRITE(f,val) do { fwrite(&(val),sizeof(val),1,f); } while (0)
void network_save(const struct Network *n, FILE *f)
{
    int layer, node, weight;

    /* Write each layer */
    for (layer = 0; layer < n->num_layers; ++layer) {
        for (node = 0; node < n->num_nodes[layer]; ++node) {
            int num_weights;
            struct Node *x = n->layers[layer][node];

            num_weights = node_num_weights(x);
            for (weight = 0; weight < num_weights; ++weight) {
                double w = node_get_weight(x,weight);
                WRITE(f,w);
            }
        }
    }
}

#define READ(f,val) do { fread(&(val),sizeof(val),1,f); } while (0)
void network_load(const struct Network *n, FILE *f)
{
    int layer, node, weight;

    /* Write each layer */
    for (layer = 0; layer < n->num_layers; ++layer) {
        for (node = 0; node < n->num_nodes[layer]; ++node) {
            int num_weights;
            struct Node *x = n->layers[layer][node];

            num_weights = node_num_weights(x);
            for (weight = 0; weight < num_weights; ++weight) {
                double w;
                READ(f,w);
                node_set_weight(x, weight, w);
            }
        }
    }
}
