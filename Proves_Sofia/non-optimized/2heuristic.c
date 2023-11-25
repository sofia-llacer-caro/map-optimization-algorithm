#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define MAXSUCC 9  // in an optimal implementation it must be removed

typedef struct {
    unsigned long id; // Node identification
    char name[200];   // in an optimal implementation it must change to a pointer
    double lat, lon;  // Node position
    unsigned short nsucc;  // Number of node successors; i.e., length of successors
    unsigned long successors[MAXSUCC];  // in an optimal implementation it must change to a pointer
} node;

double euclideanDistance(node *node1, node *node2) {
    double lat1 = node1->lat;
    double lon1 = node1->lon;
    double lat2 = node2->lat;
    double lon2 = node2->lon;

    return sqrt(pow(lat2 - lat1, 2) + pow(lon2 - lon1, 2));
}

// Function to read data from a binary file
void readBinaryFile(const char *binmapname, unsigned long nodeIndex1, unsigned long nodeIndex2) {
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

    // Example: Calculate and print the distance between selected nodes
    if (nodeIndex1 < nnodes && nodeIndex2 < nnodes) {
        double distance = euclideanDistance(&nodes[nodeIndex1], &nodes[nodeIndex2]);
        printf("Distance between nodes %lu and %lu: %lf\n", nodes[nodeIndex1].id, nodes[nodeIndex2].id, distance);
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
