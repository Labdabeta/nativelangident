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

    num_successes = num_trials = 0;
    while (!feof(stdin)) {
        char word[11] = {0};
        int i,lang;
        double inputs[10];
        double expected[4];
        double outputs[4];
        double biggest;

        fscanf(stdin, "%10s %d", word, &lang);

        if (feof(stdin)) break;

        expected[0] = (lang & 1 ? 1.0 : 0.0);
        expected[1] = (lang & 2 ? 1.0 : 0.0);
        expected[2] = (lang & 4 ? 1.0 : 0.0);
        expected[4] = (lang & 8 ? 1.0 : 0.0);

        for (i = 0; i < 10; ++i) {
            if (word[i])
                inputs[i] = (double)(word[i] - 'a' + 1) / 26.0;
            else
                inputs[i] = 0.0;
        }

        network_train(net, inputs, outputs, expected, 0.15);

        biggest = outputs[0]; i = 0;
        if (outputs[1] > biggest) { biggest = outputs[1]; i = 1; }
        if (outputs[2] > biggest) { biggest = outputs[2]; i = 2; }
        if (outputs[3] > biggest) { biggest = outputs[3]; i = 3; }

        printf("Guess: %lf English, %lf Esperanto, %lf Latin, %lf German\n",
                outputs[0], outputs[1], outputs[2], outputs[3]);


        if ((1 << i) & lang) num_successes++;

        num_trials++;
    }

    if (argc > 1) {
        FILE *f = fopen(argv[1], "wb");
        if (f) network_save(net, f);
        fclose(f);
    }

    return 0;
}


