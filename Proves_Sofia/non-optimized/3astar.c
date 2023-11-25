#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <limits.h>

#define MAXSUCC 9  // in an optimal implementation it must be removed

typedef struct {
    unsigned long id; // Node identification
    char name[200];   // in an optimal implementation it must change to a pointer
    double lat, lon;  // Node position
    unsigned short nsucc;  // Number of node successors; i.e., length of successors
    unsigned long successors[MAXSUCC];  // in an optimal implementation it must change to a pointer
    double g;  // Cost from the start node to the current node
    double f;  // Total estimated cost (g + h)
} node;

double euclideanDistance(node *node1, node *node2, node *goal) {
    double lat1 = node1->lat;
    double lon1 = node1->lon;
    double lat2 = node2->lat;
    double lon2 = node2->lon;

    double h = sqrt(pow(lat2 - goal->lat, 2) + pow(lon2 - goal->lon, 2));
    return h;
}

// Function to read data from a binary file
void readBinaryFile(const char *binmapname, unsigned long startNodeIndex, unsigned long goalNodeIndex) {
    clock_t start_time;
    unsigned long nnodes;

    start_time = clock();

    FILE *binmapfile;
    binmapfile = fopen(binmapname, "rb");
    if (binmapfile == NULL) {
        printf("Error when opening the file\n");
        exit(1);
    }

    fread(&nnodes, sizeof(unsigned long), 1, binmapfile);

    node *nodes;
    nodes = (node *)malloc(nnodes * sizeof(node));
    if (nodes == NULL) {
        printf("Error when allocating the memory for the nodes\n");
        exit(2);
    }

    fread(nodes, sizeof(node), nnodes, binmapfile);
    fclose(binmapfile);

    // Check if start and goal node indices are valid
    if (startNodeIndex < nnodes && goalNodeIndex < nnodes) {
        // Initialize g and f values
        for (unsigned long i = 0; i < nnodes; ++i) {
            nodes[i].g = INFINITY;  // Set initial cost to infinity
            nodes[i].f = INFINITY;  // Set initial total cost to infinity
        }

        nodes[startNodeIndex].g = 0.0;
        nodes[startNodeIndex].f = euclideanDistance(&nodes[startNodeIndex], &nodes[goalNodeIndex], &nodes[goalNodeIndex]);

        // A* algorithm
        while (1) {
            // Find the node with the smallest f value
            double minF = INFINITY;
            unsigned long current = nnodes;  // Invalid index
            for (unsigned long i = 0; i < nnodes; ++i) {
                if (nodes[i].f < minF && nodes[i].g < INFINITY) {
                    minF = nodes[i].f;
                    current = i;
                }
            }

            // If all nodes have been explored or goal reached, break
            if (current == nnodes || current == goalNodeIndex) {
                break;
            }

            // Expand successors of the current node
            for (unsigned short i = 0; i < nodes[current].nsucc; ++i) {
                unsigned long successor = nodes[current].successors[i];
                double tentativeG = nodes[current].g + euclideanDistance(&nodes[current], &nodes[successor], &nodes[goalNodeIndex]);

                // Update if the new path to the successor is better
                if (tentativeG < nodes[successor].g) {
                    nodes[successor].g = tentativeG;
                    nodes[successor].f = nodes[successor].g + euclideanDistance(&nodes[successor], &nodes[goalNodeIndex], &nodes[goalNodeIndex]);
                }
            }
        }

        // Print the result or do whatever you need with the path
        printf("Path from node %lu to node %lu:\n", nodes[startNodeIndex].id, nodes[goalNodeIndex].id);
        unsigned long current = goalNodeIndex;
        while (current != startNodeIndex && current < nnodes) {
            printf("%lu <- ", nodes[current].id);
            double minF = INFINITY;
            unsigned long next = nnodes;  // Invalid index
            for (unsigned short i = 0; i < nodes[current].nsucc; ++i) {
                unsigned long successor = nodes[current].successors[i];
                if (nodes[successor].f < minF) {
                    minF = nodes[successor].f;
                    next = successor;
                }
            }
            current = next;
        }
        printf("%lu\n", nodes[startNodeIndex].id);
    } else {
        printf("Invalid node indices\n");
    }

    free(nodes);

    printf("Elapsed time: %f seconds\n", (float)(clock() - start_time) / CLOCKS_PER_SEC);
}

int main(int argc, char *argv[]) {
    clock_t start_time;

    start_time = clock();

    if (argc > 3) {
        const char *filename = argv[1];
        size_t len = strlen(filename);

        if (len >= 4 && strcmp(filename + len - 4, ".bin") == 0) {
            // If the file has a .bin extension, read the binary file
            unsigned long nodeIndex1 = strtoul(argv[2], NULL, 10);
            unsigned long nodeIndex2 = strtoul(argv[3], NULL, 10);

            readBinaryFile(filename, nodeIndex1, nodeIndex2);
        } else {
            printf("Usage: %s <binary_filename> <node_index_1> <node_index_2>\n", argv[0]);
            exit(1);
        }
    } else {
        printf("Usage: %s <binary_filename> <node_index_1> <node_index_2>\n", argv[0]);
        exit(1);
    }

    printf("Elapsed time: %f seconds\n", (float)(clock() - start_time) / CLOCKS_PER_SEC);

    return 0;
}
