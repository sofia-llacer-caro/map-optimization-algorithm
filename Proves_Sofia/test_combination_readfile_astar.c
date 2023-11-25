#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#define MAXSUCC 9

typedef struct Node {
    unsigned long id;
    char name[200];
    double lat, lon;
    unsigned short nsucc;
    unsigned long successors[MAXSUCC];

    // A* specific information
    double cost;
    double heuristic;
    struct Node* parent;
} Node;

typedef struct ListNode {
    Node* node;
    struct ListNode* next;
} ListNode;

unsigned long searchNode(unsigned long id, Node *nodes, unsigned long nnodes) {
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

    Node *nodes;
    char *tmpline, *field, *ptr;
    unsigned long index = 0;

    nodes = (Node *)malloc(nnodes * sizeof(Node));
    if (nodes == NULL) {
        printf("Error when allocating the memory for the nodes\n");
        exit(2);
    }

    while (getline(&line, &len, mapfile) != -1) {
        if (strncmp(line, "#", 1) == 0)
            continue;
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
            index++;
        }
    }
    printf("Assigned data to %ld nodes\n", index);
    printf("Last node has:\n id=%lu\n GPS=(%lf,%lf)\n Name=%s\n", nodes[index - 1].id, nodes[index - 1].lat, nodes[index - 1].lon, nodes[index - 1].name);

    rewind(mapfile);

    int oneway;
    unsigned long nedges = 0, origin, dest, originId, destId;
    while (getline(&line, &len, mapfile) != -1) {
        if (strncmp(line, "#", 1) == 0)
            continue;
        tmpline = line;
        field = strsep(&tmpline, "|");
        if (strcmp(field, "way") == 0) {
            for (int i = 0; i < 7; i++)
                field = strsep(&tmpline, "|");
            if (strcmp(field, "") == 0)
                oneway = 0;
            else if (strcmp(field, "oneway") == 0)
                oneway = 1;
            else
                continue;
            field = strsep(&tmpline, "|");
            field = strsep(&tmpline, "|");
            if (field == NULL)
                continue;
            originId = strtoul(field, &ptr, 10);
            origin = searchNode(originId, nodes, nnodes);
            while (1) {
                field = strsep(&tmpline, "|");
                if (field == NULL)
                    break;
                destId = strtoul(field, &ptr, 10);
                dest = searchNode(destId, nodes, nnodes);
                if ((origin == nnodes + 1) || (dest == nnodes + 1)) {
                    originId = destId;
                    origin = dest;
                    continue;
                }
                if (origin == dest)
                    continue;

                int newdest = 1;
                for (int i = 0; i < nodes[origin].nsucc; i++)
                    if (nodes[origin].successors[i] == dest) {
                        newdest = 0;
                        break;
                    }
                if (newdest) {
                    if (nodes[origin].nsucc >= MAXSUCC) {
                        printf("Maximum number of successors (%d) reached in node %lu.\n", MAXSUCC, nodes[origin].id);
                        exit(5);
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
                        if (nodes[dest].nsucc >= MAXSUCC) {
                            printf("Maximum number of successors (%d) reached in node %lu.\n", MAXSUCC, nodes[dest].id);
                            exit(5);
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
    fwrite(nodes, sizeof(Node), nnodes, binmapfile);
    fclose(binmapfile);

    free(nodes);
}

void readBinaryFile(const char *binmapname, Node** nodes, unsigned long* nnodes) {
    clock_t start_time;

    start_time = clock();

    FILE *binmapfile;
    binmapfile = fopen(binmapname, "rb");
    if (binmapfile == NULL) {
        printf("Error when opening the file\n");
        exit(1);
    }

    fread(nnodes, sizeof(unsigned long), 1, binmapfile);

    *nodes = (Node *)malloc(*nnodes * sizeof(Node));
    if (*nodes == NULL) {
        printf("Error when allocating the memory for the nodes\n");
        exit(2);
    }

    fread(*nodes, sizeof(Node), *nnodes, binmapfile);
    fclose(binmapfile);

    printf("Total number of nodes is %ld\n", *nnodes);
    printf("Elapsed time: %f seconds\n", (float)(clock() - start_time) / CLOCKS_PER_SEC);
}

double calculateDistance(Node *node1, Node *node2) {
    double dx = node1->lat - node2->lat;
    double dy = node1->lon - node2->lon;
    return sqrt(dx * dx + dy * dy);
}

void addToOpenList(ListNode** openList, Node* newNode) {
    // ... (your existing code for adding to open list)
}

Node* removeFromOpenList(ListNode** openList) {
    // ... (your existing code for removing from open list)
}

void addToClosedList(ListNode** closedList, Node* newNode) {
    // ... (your existing code for adding to closed list)
}

int isInOpenList(ListNode* openList, Node* targetNode) {
    // ... (your existing code for checking if a node is in open list)
}

void reconstructAndPrintPath(Node* start, Node* goal) {
    // ... (your existing code for reconstructing and printing the path)
}

void astar(Node* nodes, unsigned long nnodes, Node* start, Node* goal) {
    ListNode* openList = NULL;
    ListNode* closedList = NULL;

    start->cost = 0.0;
    start->heuristic = calculateDistance(start, goal);
    start->parent = NULL;
    addToOpenList(&openList, start);

    while (openList != NULL) {
        Node* current = removeFromOpenList(&openList);
        addToClosedList(&closedList, current);

        if (current == goal) {
            reconstructAndPrintPath(start, goal);
            return;
        }

        for (int i = 0; i < current->nsucc; i++) {
            Node* neighbor = &nodes[current->successors[i]];

            double tentativeGCost = current->cost + calculateDistance(current, neighbor);

            if (!isInOpenList(openList, neighbor) || tentativeGCost < neighbor->cost) {
                neighbor->cost = tentativeGCost;
                neighbor->heuristic = calculateDistance(neighbor, goal);
                neighbor->parent = current;

                if (!isInOpenList(openList, neighbor)) {
                    addToOpenList(&openList, neighbor);
                }
            }
        }
    }

    printf("No path found!\n");
}

Node* findStartNode(Node* nodes, unsigned long nnodes) {
    // TODO: Implement logic to find the start node
    return &nodes[0];
}

Node* findGoalNode(Node* nodes, unsigned long nnodes) {
    // TODO: Implement logic to find the goal node
    return &nodes[nnodes - 1];
}

int main(int argc, char *argv[]) {
    clock_t start_time;

    start_time = clock();

    if (argc > 1) {
        const char *filename = argv[1];
        size_t len = strlen(filename);

        if (len >= 4 && strcmp(filename + len - 4, ".bin") == 0) {
            // If the file has a .bin extension, read the binary file
            Node* nodes;
            unsigned long nnodes;

            readBinaryFile(filename, &nodes, &nnodes);

            // Find start and goal nodes
            Node* start = findStartNode(nodes, nnodes);
            Node* goal = findGoalNode(nodes, nnodes);

            // Run A* algorithm
            astar(nodes, nnodes, start, goal);

            free(nodes);
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
