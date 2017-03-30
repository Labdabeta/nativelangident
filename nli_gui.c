#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "layered_network.h"

double sigmoid(double x) { return 1.0 / (1.0 + pow(M_E, -x)); }
double dsigmoid(double x) { return sigmoid(x)*(1.0-sigmoid(x)); }

int main(int argc, char *argv[])
{
    int num_successes, num_trials;
    int num_inputs = 10;
    int num_layers = 2;
    int num_nodes[] = { 18, 4 };
    activator acts[] = {sigmoid, sigmoid};
    activator dacts[] = {dsigmoid, dsigmoid};
    struct Network *net = network_new(num_inputs, num_layers, num_nodes, acts, dacts);

    if (argc > 1) {
        FILE *f = fopen(argv[1], "rb");
        if (f) network_load(net, f);
        fclose(f);
    }

// TODO

    if (argc > 1) {
        FILE *f = fopen(argv[1], "wb");
        if (f) network_save(net, f);
        fclose(f);
    }

    return 0;
}
