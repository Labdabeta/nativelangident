#ifndef NODE_H
#define NODE_H

typedef double (*activator)(double);
typedef double (*initializer)(void);

struct Node;

struct Node *node_new(int num_inputs, activator act, activator dact, initializer init);
void node_free(struct Node *n);
void node_set_weight(struct Node *n, int weight, double val);
double node_get_weight(struct Node *n, int weight);
double node_output(struct Node *n, double *inputs);
double node_last_value(const struct Node *n);

#endif /* NODE_H */
