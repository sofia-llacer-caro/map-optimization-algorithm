#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
    unsigned long id; // Node identification
    char name[200];
    double lat, lon;  // Node position
    unsigned short nsucc;  // Number of node successors; i.e., length of successors
    unsigned long successors[9];

    // A* specific information
    double cost; // Cost to reach this node from the start node
    double heuristic; // Heuristic value for A*
    struct Node* parent; // Parent node in the path
} Node;

// Function to calculate the Euclidean distance between two nodes
double calculateDistance(Node *node1, Node *node2) {
    double dx = node1->lat - node2->lat;
    double dy = node1->lon - node2->lon;
    return sqrt(dx * dx + dy * dy);
}

// A* algorithm function
void astar(Node *nodes, unsigned long nnodes, Node *start, Node *goal) {
    // TODO: Initialize open and closed lists

    // TODO: Add the start node to the open list

    while(/* TODO: Check if open list is not empty */) {
        // TODO: Get the node with the lowest f_cost from the open list

        // TODO: Move the current node to the closed list

        // TODO: If the current node is the goal, reconstruct the path and return

        // TODO: Expand the neighbors of the current node
        for(/* TODO: Loop through neighbors */) {
            // TODO: Calculate tentative g_cost

            // TODO: If neighbor is not in the open list or new g_cost is lower
            // TODO: Update neighbor's information and add it to the open list
        }
    }

    // TODO: No path found
}

int main() {
    // TODO: Read the map and load nodes and edges

    // TODO: Find start and goal nodes

    // TODO: Run A* algorithm
    // astar(nodes, nnodes, start, goal);

    return 0;
}
