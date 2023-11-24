#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED


typedef struct {
	unsigned long id;
	char *name;
	double latitude, longitude;
	unsigned short nsucc;
	unsigned long *successors;
}node;

typedef struct node{
	unsigned long id;
	double latitude, longitude;
	unsigned short nsucc;
	unsigned long *successors;
	struct node *prev, *next,*parent; // next and prev for the open list structure. parent to know the final path
	char list; // 0: nothing, 1: open, 2: closed
	double heuristic; // heuristic distance+ g distance
	double g; // g distance: from source to this node

}node_complete;


typedef struct{

	node_complete * start, *end;
	unsigned long nelems; // number of elements on list

} ourlist; // open list structure





void ListInicialization ( ourlist * x);

double heuristic_function(node_complete *init, node_complete *prev );




 // returns the node index from the node id
long BinarySearch(unsigned long key);

/******************* Utility functions for linked list *******************/


void DeleteNode(ourlist * list, node_complete * target);

void InsertNodeAfter(ourlist * list, node_complete* target , node_complete* a_insertar);

void InsertNodeBefore(ourlist * list, node_complete * target , node_complete * a_insertar);

void Add_to_Open (ourlist * list, node_complete * succesor);


/*****************************************************************/


int A_star (unsigned int source, unsigned int goal);


void reading_from_file();

#endif
