#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <argp.h>
#include "layered_network.h"

#define TRAIN_RATE 0.1

char **languages;

/* softmax is the integral of sigmoid */
double softmax(double x) {
    double ret = log(1 + pow(M_E, x));
    if (ret != ret)
        return 0.0;
    return ret;
}

double sigmoid(double x) {
    double ret = 1.0 / (1.0 + pow(M_E, -x));
    if (ret != ret)
        return 0.0;
    return ret;
}

double dsigmoid(double x) { return sigmoid(x)*(1.0-sigmoid(x)); }

double rectifier(double x) { return (x < 0) ? 0 : x; }
double drectifier(double x) { return (x < 0) ? 0 : 1; }

double identity(double x) { return x; }
double unity(double x) { return 1.0; }

int training;
int printing;
int show_history;
int num_successes, num_trials;
int assumed_lang;
int history[1000];
int history_counter = 0;
int word_len;
int num_langs;
double *inputs;
double *expected;
double *outputs;
struct Network *net;

void runInput(char *word, int lang) {
    int i, bigindex;
    double biggest;

    for (i = 0; i < 26 * word_len; ++i)
        inputs[i] = 0.0;

    for (i = 0; i < num_langs; ++i)
        expected[i] = (lang & (1 << i) ? 1.0 : 0.0);

    for (i = 0; i < word_len; ++i)
        if (word[i] && word[i] >= 'a' && word[i] <= 'z') inputs[26 * i + word[i] - 'a' + 1] = 1.0;

    if (training)
        network_train(net, inputs, outputs, expected, TRAIN_RATE);
    else
        network_value(net, inputs, outputs);

    biggest = outputs[0];
    bigindex = 0;
    for (i = 1; i < num_langs; ++i) {
        if (outputs[i] > biggest) {
            biggest = outputs[i];
            bigindex = i;
        }
    }

    if (printing) {
        printf("Guess(%s): ", word);
        for (i = 0; i < num_langs; ++i) {
            if (lang & (1 << i))
                printf("%s,",languages[i]);
        }
        printf("\b\n\t");

        for (i = 0; i < num_langs; ++i)
            printf("%lf %s,",outputs[i],languages[i]);
        printf("\b\n");

        printf("Guess is: %s(%d) - %s\n",
                languages[bigindex],
                bigindex + 1,
                ((1<<bigindex)&lang)?"Correct!":"Wrong");

        if (training) {
            double err = 0.0;
            double mag = 0.0;

            for (i = 0; i < num_langs; ++i) {
                err += (expected[i] - outputs[i]) * (expected[i] - outputs[i]);
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

    if (show_history) {
        int i,s=0;
        for (i = 0; i < 1000; ++i) s += history[i];
        printf("Correctly guessed %d of the last thousand tests (%lf%%).\n",
                s, s/10.0);
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
        printf("Training: %s\n", (training?"ON":"OFF"));
    } else if (!strcmp(option,"input")) {
        char fname[256];
        FILE *f;

        scanf("%s",fname);
        f = fopen(fname,"r");

        if (!f) {
            printf("Could not open %s.\n", fname);
            return;
        }

        while (!feof(f)) {
            char word[256] = {0};
            int lang;
            fscanf(f,"%10s %d", word, &lang);
            runInput(word,lang);
        }

        fclose(f);

        printf("Done!\n");
    } else if (!strcmp(option,"print")) {
        printing = !printing;
        printf("Printing: %s\n", (printing?"ON":"OFF"));
    } else if (!strcmp(option,"assume")) {
        scanf("%d", &assumed_lang);
    } else if (!strcmp(option,"normal")) {
        assumed_lang = -1;
    } else if (!strcmp(option,"load")) {
        char fname[256];
        FILE *f;

        scanf("%s",fname);
        f = fopen(fname,"r");

        if (!f) {
            printf("Could not open %s.\n", fname);
            return;
        }

        network_load(net, f);
        fclose(f);
    } else if (!strcmp(option,"save")) {
        char fname[256];
        FILE *f;

        scanf("%s",fname);
        f = fopen(fname,"w");

        if (!f) {
            printf("Could not open %s.\n", fname);
            return;
        }

        network_save(net, f);
        fclose(f);
    } else if (!strcmp(option,"togglevh")) {
        show_history = !show_history;

        printf("Show history: %s\n", show_history?"ON":"OFF");
    } else {
        printf("Unknown command: %s\n", option);
    }
}

void load_net(int argc, char *argv[]) {
    /* ./nli #word_len #layers ((s/+/r) #nodes)+ */
    int i, num_inputs, num_layers, *num_nodes;
    activator *acts, *dacts;

    if (argc < 3) {
        printf("%s [WORD_LEN] [NUM_LAYERS] ((s/+/r) #nodes)+ (Language Name)+\n", argv[0]);
    }

    word_len = atoi(argv[1]);
    num_inputs = 26 * word_len;
    num_layers = atoi(argv[2]);

    acts = malloc(sizeof(activator) * num_layers);
    dacts = malloc(sizeof(activator) * num_layers);
    num_nodes = malloc(sizeof(int) * num_layers);

    for (i = 0; i < num_layers; ++i) {
        char activator = argv[2*i+3][0];

        num_nodes[i] = atoi(argv[2*i+4]);

        switch (activator) {
            case 's':
                acts[i] = sigmoid;
                dacts[i] = dsigmoid;
                break;
            case '+':
                acts[i] = softmax;
                dacts[i] = sigmoid;
                break;
            case 'r':
                acts[i] = rectifier;
                dacts[i] = drectifier;
                break;
            default:
                acts[i] = identity;
                dacts[i] = unity;
        }
    }

    num_langs = num_nodes[num_layers-1];

    languages = malloc(sizeof(char*)*num_langs);
    for (i = 0; i < num_langs; ++i)
        languages[i] = argv[2*num_layers+3+i];

    net = network_new(num_inputs, num_layers, num_nodes, acts, dacts);

    inputs = malloc(sizeof(double) * 26 * word_len);
    expected = malloc(sizeof(double) * num_langs);
    outputs = malloc(sizeof(double) * num_langs);

    free(acts);
    free(dacts);
    free(num_nodes);
}

int main(int argc, char *argv[])
{
    load_net(argc, argv);
    training = 1;
    printing = 1;
    show_history = 0;
    assumed_lang = -1;

    srand(0);

    num_successes = num_trials = 0;
    while (!feof(stdin)) {
        char word[256] = {0};
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

    return 0;
}


