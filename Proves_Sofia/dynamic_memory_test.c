#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

typedef struct {
    unsigned long id; // Node identification
    char name[200];   // in an optimal implementation it must change to a pointer
    double lat, lon;  // Node position
    unsigned short nsucc;  // Number of node successors; i. e. length of successors
    unsigned long *successors;  // Pointer to an array of successors
} node;

unsigned long searchNode(unsigned long id, node *nodes, unsigned long nnodes) {
    // The implementation of the searchNode function goes here
    unsigned long l = 0, r = nnodes - 1, m;
    while (l <= r) {
        m = l + (r - l) / 2;
        if (nodes[m].id == id)
            return m;
        if (nodes[m].id < id)
            l = m + 1;
        else
            r = m - 1;
    }
    // id not found, we return nnodes+1
    return nnodes + 1;
}

// Function to write data to a binary file
void writeBinaryFile(const char *mapname) {
    clock_t start_time;
    FILE *mapfile;
    unsigned long nnodes;
    char *line = NULL;
    size_t len;

    start_time = clock();

    mapfile = fopen(mapname, "r");
    if (mapfile == NULL) {
        printf("Error when opening the file\n");
        exit(1);
    }

    // count the nodes
    nnodes = 0UL;
    while (getline(&line, &len, mapfile) != -1) {
        if (strncmp(line, "node", 4) == 0) {
            nnodes++;
        }
    }
    printf("Total number of nodes is %ld\n", nnodes);

    rewind(mapfile);

    node *nodes;
    char *tmpline, *field, *ptr;
    unsigned long index = 0;

    nodes = (node *)malloc(nnodes * sizeof(node));
    if (nodes == NULL) {
        printf("Error when allocating the memory for the nodes\n");
        exit(2);
    }

    while (getline(&line, &len, mapfile) != -1) {
        if (strncmp(line, "#", 1) == 0) continue;
        tmpline = line;
        field = strsep(&tmpline, "|");
        if (strcmp(field, "node") == 0) {
            field = strsep(&tmpline, "|");
            nodes[index].id = strtoul(field, &ptr, 10);
            field = strsep(&tmpline, "|");
            strcpy(nodes[index].name, field);
            for (int i = 0; i < 7; i++)
                field = strsep(&tmpline, "|");
            nodes[index].lat = atof(field);
            field = strsep(&tmpline, "|");
            nodes[index].lon = atof(field);

            nodes[index].nsucc = 0;
            nodes[index].successors = NULL;  // Initialize to NULL
            index++;
        }
    }
    printf("Assigned data to %ld nodes\n", index);
    printf("Last node has:\n id=%lu\n GPS=(%lf,%lf)\n Name=%s\n", nodes[index - 1].id, nodes[index - 1].lat, nodes[index - 1].lon, nodes[index - 1].name);

    rewind(mapfile);

    int oneway;
    unsigned long nedges = 0, origin, dest, originId, destId;
    while (getline(&line, &len, mapfile) != -1) {
        if (strncmp(line, "#", 1) == 0) continue;
        tmpline = line;
        field = strsep(&tmpline, "|");
        if (strcmp(field, "way") == 0) {
            for (int i = 0; i < 7; i++) field = strsep(&tmpline, "|");
            if (strcmp(field, "") == 0)
                oneway = 0;
            else if (strcmp(field, "oneway") == 0)
                oneway = 1;
            else
                continue;
            field = strsep(&tmpline, "|");
            field = strsep(&tmpline, "|");
            if (field == NULL) continue;
            originId = strtoul(field, &ptr, 10);
            origin = searchNode(originId, nodes, nnodes);
            while (1) {
                field = strsep(&tmpline, "|");
                if (field == NULL) break;
                destId = strtoul(field, &ptr, 10);
                dest = searchNode(destId, nodes, nnodes);
                if ((origin == nnodes + 1) || (dest == nnodes + 1)) {
                    originId = destId;
                    origin = dest;
                    continue;
                }
                if (origin == dest) continue;

                int newdest = 1;
                for (int i = 0; i < nodes[origin].nsucc; i++)
                    if (nodes[origin].successors[i] == dest) {
                        newdest = 0;
                        break;
                    }
                if (newdest) {
                    // Allocate memory for the successors array
                    nodes[origin].successors = (unsigned long *)realloc(nodes[origin].successors, (nodes[origin].nsucc + 1) * sizeof(unsigned long));
                    if (nodes[origin].successors == NULL) {
                        // Handle memory allocation failure
                        printf("Error when allocating memory for successors\n");
                        exit(3);  // or another error code
                    }

                    nodes[origin].successors[nodes[origin].nsucc] = dest;
                    nodes[origin].nsucc++;
                    nedges++;
                }
                if (!oneway) {
                    int newor = 1;
                    for (int i = 0; i < nodes[dest].nsucc; i++)
                        if (nodes[dest].successors[i] == origin) {
                            newor = 0;
                            break;
                        }
                    if (newor) {
                        // Allocate memory for the successors array
                        nodes[dest].successors = (unsigned long *)realloc(nodes[dest].successors, (nodes[dest].nsucc + 1) * sizeof(unsigned long));
                        if (nodes[dest].successors == NULL) {
                            // Handle memory allocation failure
                            printf("Error when allocating memory for successors\n");
                            exit(3);  // or another error code
                        }

                        nodes[dest].successors[nodes[dest].nsucc] = origin;
                        nodes[dest].nsucc++;
                        nedges++;
                    }
                }
                originId = destId;
                origin = dest;
            }
        }
    }

    fclose(mapfile);
    printf("Assigned %ld edges\n", nedges);
    printf("Elapsed time: %f seconds\n", (float)(clock() - start_time) / CLOCKS_PER_SEC);

    for (unsigned long i = 0; i < nnodes; i++) {
        if (nodes[i].nsucc > 4) {
            index = i;
            break;
        }
    }
    printf("Node %lu has id=%lu and %u successors:\n", index, nodes[index].id, nodes[index].nsucc);
    for (int i = 0; i < nodes[index].nsucc; i++)
        printf("  Node %lu with id %lu.\n", nodes[index].successors[i], nodes[nodes[index].successors[i]].id);

    FILE *binmapfile;
    char binmapname[80];
    strcpy(binmapname, mapname);
    strcat(binmapname, ".bin");

    binmapfile = fopen(binmapname, "wb");
    fwrite(&nnodes, sizeof(unsigned long), 1, binmapfile);
    fwrite(nodes, sizeof(node), nnodes, binmapfile);
    fclose(binmapfile);

    // Free memory for successors arrays
    for (unsigned long i = 0; i < nnodes; i++) {
        free(nodes[i].successors);
    }
    free(nodes);
}

// Function to read data from a binary file
void readBinaryFile(const char *binmapname) {
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

    printf("Total number of nodes is %ld\n", nnodes);
    printf("Elapsed time: %f seconds\n", (float)(clock() - start_time) / CLOCKS_PER_SEC);

    for (unsigned long i = 0; i < nnodes; i++) {
        if (nodes[i].nsucc > 4) {
            unsigned long index = i;
            printf("Node %lu has id=%lu and %u successors:\n", index, nodes[index].id, nodes[index].nsucc);
            for (int j = 0; j < nodes[index].nsucc; j++)
                printf("  Node %lu with id %lu.\n", nodes[index].successors[j], nodes[nodes[index].successors[j]].id);
            break;
        }
    }

    // Free memory for successors arrays
    for (unsigned long i = 0; i < nnodes; i++) {
        free(nodes[i].successors);
    }
    free(nodes);
}

int main(int argc, char *argv[]) {
    clock_t start_time;

    start_time = clock();

    if (argc > 1) {
        const char *filename = argv[1];
        size_t len = strlen(filename);

        if (len >= 4 && strcmp(filename + len - 4, ".bin") == 0) {
            // If the file has a .bin extension, read the binary file
            readBinaryFile(filename);
        } else {
            // Otherwise, assume it's a CSV file and write the binary file
            writeBinaryFile(filename);
        }
    } else {
        printf("Usage: %s <filename>\n", argv[0]);
        exit(1);
    }

    printf("Elapsed time: %f seconds\n", (float)(clock() - start_time) / CLOCKS_PER_SEC);

    return 0;
}
