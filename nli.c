#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "layered_network.h"

#define WORD_LEN 10
#define NUM_LANGS 2
#define TRAIN_RATE 0.1

char *languages[] = {"English", "Esperanto"};

double sigmoid(double x) { return 1.0 / (1.0 + pow(M_E, -x)); }
double dsigmoid(double x) { return sigmoid(x)*(1.0-sigmoid(x)); }

int training;
int printing;
int num_successes, num_trials;
int history[1000];
int history_counter = 0;
int num_inputs = 26 * WORD_LEN;
int num_layers = 2;
int num_nodes[] = { 18, 2 };
int assumed_lang;
activator acts[] = {sigmoid, sigmoid};
activator dacts[] = {dsigmoid, dsigmoid};
struct Network *net;

void runInput(char *word, int lang) {
    int i, bigindex;
    double inputs[26 * WORD_LEN] = {0};
    double expected[NUM_LANGS];
    double outputs[NUM_LANGS];
    double biggest;

    for (i = 0; i < NUM_LANGS; ++i)
        expected[i] = (lang & (1 << i) ? 1.0 : 0.0);

    for (i = 0; i < WORD_LEN; ++i)
        if (word[i]) inputs[26 * i + word[i] - 'a' + 1] = 1.0;

    if (training)
        network_train(net, inputs, outputs, expected, TRAIN_RATE);
    else
        network_value(net, inputs, outputs);

    biggest = outputs[0];
    bigindex = 0;
    for (i = 1; i < NUM_LANGS; ++i) {
        if (outputs[i] > biggest) {
            biggest = outputs[i];
            bigindex = i;
        }
    }

    if (printing) {
        printf("Guess(%s): ", word);
        for (i = 0; i < NUM_LANGS; ++i) {
            if (lang & (1 << i))
                printf("%s,",languages[i]);
        }
        printf("\b\n\t");

        for (i = 0; i < NUM_LANGS; ++i)
            printf("%lf %s,",outputs[i],languages[i]);
        printf("\b\n");

        printf("Guess is: %s(%d) - %s\n",
                languages[bigindex],
                bigindex + 1,
                ((1<<bigindex)&lang)?"Correct!":"Wrong");

        if (training) {
            double err = 0.0;
            double mag = 0.0;

            for (i = 0; i < NUM_LANGS; ++i) {
                err += (outputs[i] - expected[i]) * (outputs[i] - expected[i]);
                mag += expected[i] * expected[i];
            }

            printf("Error was %lf%%\n", 100.0*sqrt(err)/sqrt(mag));
        }
    }

    if ((1 << bigindex) & lang) {
        num_successes++;
        history[history_counter = (history_counter + 1) % 1000] = 1;
    } else {
        history[history_counter = (history_counter + 1) % 1000] = 0;
    }

    num_trials++;
}

void handle_option(char *option) {
    if (!strcmp(option,"dump")) {
        network_dump(net, stdout);
    } else if (!strcmp(option,"rate")) {
        printf("Correctly guessed %d of %d (%lf%%) of tests.\n",
                num_successes, num_trials,
                (double)num_successes * 100.0 / (double) num_trials);
    } else if (!strcmp(option,"history")) {
        int i,s=0;
        for (i = 0; i < 1000; ++i) s += history[i];
        printf("Correctly guessed %d of the last thousand tests (%lf%%).\n",
                s, s/10.0);
    } else if (!strcmp(option,"train")) {
        training = !training;
    } else if (!strcmp(option,"load")) {
        char fname[256];
        FILE *f;

        scanf("%s",fname);
        f = fopen(fname,"r");

        if (!f) {
            printf("Could not open %s.\n", fname);
            return;
        }

        while (!feof(f)) {
            char word[11];
            int lang;
            fscanf(f,"%10s %d", word, &lang);
            runInput(word,lang);
        }

        fclose(f);
    } else if (!strcmp(option,"print")) {
        printing = !printing;
    } else if (!strcmp(option,"assume")) {
        scanf("%d", &assumed_lang);
    } else if (!strcmp(option,"normal")) {
        assumed_lang = -1;
    }
}

int main(int argc, char *argv[])
{
    net = network_new(num_inputs, num_layers, num_nodes, acts, dacts);
    training = 1;
    printing = 1;
    assumed_lang = -1;

    if (argc > 1) {
        FILE *f = fopen(argv[1], "rb");
        if (f) {
            network_load(net, f);
            fclose(f);
        }
    }

    srand(0);

    num_successes = num_trials = 0;
    while (!feof(stdin)) {
        char word[11] = {0};
        int lang;

        fscanf(stdin, "%10s", word);

        if (feof(stdin)) break;

        if (word[0] == '-') {
            handle_option(word + 1);
            continue;
        }

        if (assumed_lang < 0) {
            fscanf(stdin, " %d", &lang);
        } else {
            lang = assumed_lang;
        }

        if (feof(stdin)) break;

        runInput(word,lang);
    }

    if (argc > 1) {
        FILE *f = fopen(argv[1], "wb");
        if (f) network_save(net, f);
        fclose(f);
    }

    return 0;
}


