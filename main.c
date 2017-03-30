#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "layered_network.h"

double sigmoid(double x) { return 1.0 / (1.0 + pow(M_E, -x)); }
double dsigmoid(double x) { return sigmoid(x)*(1.0-sigmoid(x)); }
double identity(double x) { return x; }
double unity(double x) { return 1.0; }

void test_fn(double *in, double *out) {
    out[0] = in[0] + in[1];
}

int main(int argc, char *argv[])
{
    int i;
    double outputs[2];
    int node_counts[] = {3,1};
    double inputs[2];
    double expected[2];

    srand(0);

    activator acts[] = {sigmoid, sigmoid};
    activator dacts[] = {dsigmoid, dsigmoid};
    struct Network *n = network_new(2,2,node_counts,acts,dacts);

    network_dump(n, stdout);

    for (i = 0; i < 1000; ++i) {
        inputs[0] = ((double)rand() / (double)RAND_MAX);
        inputs[1] = ((double)rand() / (double)RAND_MAX);
        test_fn(inputs,expected);
        network_train(n, inputs, NULL, expected, 0.1);
    }

    network_dump(n, stdout);

    inputs[0] = (double)rand() / (double)RAND_MAX;
    inputs[1] = (double)rand() / (double)RAND_MAX;
    test_fn(inputs, expected);
    network_value(n, inputs, outputs);

    printf("Inputs: %lf %lf\n", inputs[0], inputs[1]);
    printf("Expect: %lf %lf\n", expected[0], expected[1]);
    printf("Output: %lf %lf\n", outputs[0], outputs[1]);

    for (i = 0; i < 1000; ++i) {
        inputs[0] = ((double)rand() / (double)RAND_MAX);
        inputs[1] = ((double)rand() / (double)RAND_MAX);
        test_fn(inputs,expected);
        network_train(n, inputs, NULL, expected, 0.1);
    }

    network_dump(n, stdout);

    inputs[0] = (double)rand() / (double)RAND_MAX;
    inputs[1] = (double)rand() / (double)RAND_MAX;
    test_fn(inputs, expected);
    network_value(n, inputs, outputs);

    printf("Inputs: %lf %lf\n", inputs[0], inputs[1]);
    printf("Expect: %lf %lf\n", expected[0], expected[1]);
    printf("Output: %lf %lf\n", outputs[0], outputs[1]);

    return 0;
}
