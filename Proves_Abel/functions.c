#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<time.h>

#include "functions.h"


#define INFINITY 1000000000
#define PI 3.14159265359
#define R 6.3781e6 //earth radius in meters

unsigned long nnodes = 23895681UL;
node * nodes;
node_complete* node_complete_variable;



void ListInicialization ( ourlist * x){

	x->start = x->end = NULL;
	x->nelems = 0; 
}



/*

double heuristic_function(node_complete *init, node_complete *prev )
{	

	// Haversine formula 
	double phi1= init->latitude/180*PI;
	double phi2= prev->latitude/180*PI;
	double lambda1=init->longitude/180*PI;
	double lambda2=prev->longitude/180*PI;
	double latitude_difference= fabs(phi1-phi2);
	double latitude_longitude= fabs(lambda1-lambda2);

	double a = sin(latitude_difference/2)*sin(latitude_difference/2)+cos(phi1)*cos(phi2)*sin(latitude_longitude/2)*sin(latitude_longitude/2);
	double c= 2*atan2(sqrt(a),sqrt(1-a));
	
	return c*R;
}



double heuristic_function (node_complete * init, node_complete * prev)
{
		// Spherical Law of Cosines

	double phi1= init->latitude/180*PI;
	double phi2= prev->latitude/180*PI;
	double lambda1=init->longitude/180*PI;
	double lambda2=prev->longitude/180*PI;
	double latitude_longitude= fabs(lambda1-lambda2);

	return acos( sin(phi1)*sin(phi2)+cos(phi1)*cos(phi2)*cos(latitude_longitude))*R;

}

*/



double heuristic_function (node_complete * init, node_complete * prev)
{

		//Equirectangular approximation
	double phi1= init->latitude/180*PI;
	double phi2= prev->latitude/180*PI;
	double lambda1=init->longitude/180*PI;
	double lambda2=prev->longitude/180*PI;
	double latitude_longitude= fabs(lambda1-lambda2);
	double latitude_difference= fabs(phi1-phi2);

	double x = latitude_longitude*cos((phi1+phi2)/2);

	return R*sqrt(x*x+latitude_difference*latitude_difference);
}


/*



double heuristic_function (node_complete * init, node_complete * prev)
{
		//0 djikstra
	

	return 0.;
}

*/



 // returns the node index from the node id
long BinarySearch(unsigned long key)
{ 
	unsigned long start=0UL, afterend=nnodes, middle;
	unsigned long try;

	while(afterend > start)
	{ 
		middle = start + ((afterend-start-1)>>1); try = node_complete_variable[middle].id; 
		if (key == try) return middle;
		else if ( key > try ) start=middle+1;
		else afterend=middle;
	}

	return -1;
}


//Delete nodes from list
void DeleteNode(ourlist * list, node_complete * target){

	if ( list->start == list->end)
	{
		list->start = list->end = NULL;
		target->prev = target-> next = NULL;

	}
	else if ( target == list->start)
	{
		list->start = target->next;
		(target->next)->prev = NULL;
		target->next = NULL; 
		target->prev = NULL;

	}
	else if ( target == list->end)
	{
		list->end = target->prev;
		(target->prev)->next = NULL;
		target->prev = NULL;
		target->next = NULL;
	}
	else 
	{
		(target->next) -> prev = target -> prev; 
		(target->prev) -> next = target -> next ; 
		target->next = target-> prev = NULL;
	}

	list->nelems--;


}

void InsertNodeAfter(ourlist * list, node_complete* target , node_complete* a_insertar){

	if ( list-> end == target)
	{
		list-> end = a_insertar;
		target-> next = a_insertar;
		a_insertar-> prev = target ;
		a_insertar->next = NULL;
	}


	else 
	{
			(target-> next)-> prev = a_insertar;
			a_insertar-> prev = target ;
			a_insertar -> next =  target->next;
			target-> next = a_insertar;
	}

	list->nelems++;


}

void InsertNodeBefore(ourlist * list, node_complete * target , node_complete * a_insertar){


	if ( list->start == target)
	{
		target->prev = a_insertar; 
		list->start = a_insertar;
		a_insertar->prev = NULL;
		a_insertar->next = target; 
	}
	
	else 
	{
		a_insertar-> prev = (target-> prev);
		a_insertar-> next = target ;
		(target->prev)-> next = a_insertar ;
		target-> prev = a_insertar;
	}
	list->nelems++;
}



// Add to openlist
void Add_to_Open (ourlist * list, node_complete * succesor){

	node_complete * curr; 
	int n;

	if ( list->nelems ==0 ){ 
		list->start = list->end = succesor;
		succesor->prev = succesor->next = NULL;
		list->nelems++;
	}

	else {
		for ( n=0, curr = list->start; curr!=NULL ; n++, curr = curr->next){
			//printf("id : %lu\n",curr->id);
			if ( succesor-> heuristic < curr->heuristic) { InsertNodeBefore (list, curr, succesor); return;}
		}
	InsertNodeAfter(list, list->end, succesor);
	}

}


// A* Algorithm

int A_star (unsigned int source, unsigned int goal)
{
	node_complete *current , *succesor ;

	int i, nsucc;
	node_complete * curr; 
	double succesor_current_cost;
	
	// create open list
	ourlist Open;

	//Inizializate Open list
	ListInicialization(&Open);

	Open.start = Open.end = &(node_complete_variable[source]); 

	// Start g at 0
	node_complete_variable[source].g=0;

        // Compute initial value of heuristic function
	node_complete_variable[source].heuristic = heuristic_function(&(node_complete_variable[source]),&(node_complete_variable[goal]));
	
	// add 1
	Open.nelems++;



	while (Open.nelems>0){

		current = Open.start ; //takes first node: the one with lowest f distance 
	
		if ( current->id == node_complete_variable[goal].id) return 0; 

		for ( i = 0; i< (current->nsucc) ; i++){


			succesor = & node_complete_variable[*(current->successors+i)]; 

	
			succesor_current_cost = current->g+heuristic_function(succesor,current); 
	

			if ( succesor->list == 1){
			
			if ( succesor->g <= succesor_current_cost) continue; 
			
			DeleteNode(&Open, succesor); // para actualizar su posicion he de borrarlo de la lista

		}

		else if ( succesor->list == 2){
			
			if ( succesor->g <= succesor_current_cost) continue;
			
			succesor->list = 1; 
		}

		else{ succesor->list = 1; }
				
		succesor->g = succesor_current_cost;
			
		succesor->heuristic = succesor->g + heuristic_function (succesor, &(node_complete_variable[goal])); 
			
		Add_to_Open ( &Open, succesor);
				
		succesor-> parent = current;
			
	
		}

		current->list = 2;
		DeleteNode (&Open,current); 


	}

	return 1;

}




void reading_from_file()
{
	int i;
	static unsigned long * allsuccesors;
	unsigned long ntotnsucc=0UL;

	FILE * fin;

	fin=fopen("binary.bin","rb");

	if ((fread(&nnodes,sizeof(unsigned long),1,fin)+fread(&ntotnsucc,sizeof(unsigned long),1,fin))!=2) printf("when reading the header of the binary data file\n");

	if ((nodes=malloc(sizeof(node)*nnodes))==NULL) printf("when allocating memory for the nodes vector\n");

	if((allsuccesors=malloc(sizeof(unsigned long)*ntotnsucc))==NULL) printf("when allocating memory for the edges vector\n");

	if (fread(nodes,sizeof(node),nnodes,fin)!=nnodes) printf("when reading nodes from the binary data file\n");

	if (fread(allsuccesors,sizeof(unsigned long),ntotnsucc,fin)!=ntotnsucc) printf("when reading successors from the binary data file\n");

	fclose(fin);

	for (i=0;i<nnodes; i++) if (nodes[i].nsucc){
		nodes[i].successors=allsuccesors;
		allsuccesors+=nodes[i].nsucc;
	}

}



