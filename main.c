#include <stdio.h>
#include <math.h>

#include "layered_network.h"

double identity(double x) { return x; }
double unity(double x) { return 1.0; }

int main(int argc, char *argv[])
{
    double outputs[2];
    int node_counts[] = {3,2};
    double inputs[] = {0.0, 1.0};
    activator acts[] = {identity, identity};
    activator dacts[] = {unity, unity};
    struct Network *n = network_new(2,2,node_counts,acts,dacts);

    network_value(n, inputs, outputs);

    printf("%lf %lf\n", outputs[0], outputs[1]);

    network_dump(n, stdout);

    return 0;
}
