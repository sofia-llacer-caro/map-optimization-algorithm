
#define INFINITY 1000000000
#define PI 3.14159265359
#define R 6.3781e6 //earth radius in meters

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "functions.h"


node * nodes;
node_complete* node_complete_variable;



int main (void)
	{	

		//Some declarations
		int i;
		time_t time_A; // A* function time
		time_t time_reading; // reading function time
		time_t time_total; // total time of code
				
		time_total = clock();

		unsigned long nnodes = 23895681UL;
				
		node_complete_variable = malloc ( sizeof(node_complete)*nnodes);
	
		int source,goal ;
		int source_id =625025;
		int goal_id =625307;

		
		time_reading = clock();

		//Read file
		reading_from_file();

		time_reading = clock()-time_reading;

		
		//Inizialization
		for ( i = 0; i < nnodes; i ++)
		{
			node_complete_variable[i].latitude = nodes[i].latitude;
			node_complete_variable[i].longitude =	nodes[i].longitude;
			node_complete_variable[i].nsucc = nodes[i].nsucc;
			node_complete_variable[i].successors = nodes[i].successors;
			node_complete_variable[i].g = INFINITY;
			node_complete_variable[i].list = 0;
			node_complete_variable[i].heuristic = INFINITY;
			node_complete_variable[i].id = nodes[i].id;
			node_complete_variable[i].parent = NULL;

		}

		free(nodes);

		// Find index of source and goal
		source= BinarySearch(source_id);
		goal = BinarySearch(goal_id);

		printf("Index of SOURCE node:  %d \n", source);
		printf("Index of GOAL node: %d \n", goal );

		printf("Computing...\n");

		time_A=clock();
		
		if ( A_star(source,goal)==0){

		time_A=clock()-time_A;
		time_total = clock() - time_total;

		printf("A path has been found!\n");


		node_complete* curr;
		long n;
		FILE * output;

		output=fopen("output_a_star.txt","w");
		fprintf(output,"# Printing path from GOAL to SOURCE\n");
		fprintf(output,"# Time elapsed by A* Algorithm %fs \n",((double)time_A)/CLOCKS_PER_SEC);
		fprintf(output, "# Time elapsed reading binary file %fs\n",((double)time_reading)/CLOCKS_PER_SEC);
		fprintf(output, "# Total time elapsed %fs\n", ((double)time_total)/CLOCKS_PER_SEC);

		printf("# Iteration | Node Id | Distance | latitude | long\n");
		fprintf(output, "# Iteration | Node Id | Distance | latitude | long\n");
		
		for ( n = 0, curr= &(node_complete_variable[goal]); curr; n++, curr=curr->parent) 
		{
			printf("%4.ld | %10.lu | %6.2f | %2.6f | %2.6f\n", n, curr->id, curr->g, curr->latitude, curr->longitude);
			fprintf(output,"%4.ld | %10.lu | %6.2f | %2.6f | %2.6f\n", n, curr->id, curr->g, curr->latitude, curr->longitude);
		}

		printf("Time elapsed by A* Algorithm %fs\n", ((double)time_A)/CLOCKS_PER_SEC );


		}


	
		else {printf("Error, no path from source to goal\n"); time_total = clock() - time_total;}

		

		printf("Total time spent : %fs \nReading function from binary file time: %fs\n",((double)time_total)/CLOCKS_PER_SEC, ((double)time_reading)/CLOCKS_PER_SEC);



		return 0;
	}
