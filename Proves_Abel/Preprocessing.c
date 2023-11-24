#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Same structure as in main.c
typedef struct {
	unsigned long id;  //node identification
	char *name;
	double lat, lon;  // node position
	unsigned short nsucc; //number of node successors
	unsigned long *successors;
}node;

// Stores both the index and the id
typedef struct{
	unsigned long id;
	int index;
}box;

// Some declaration
unsigned long nnodes = 23895681UL;

node * nodes;

box * auxiliar = NULL;

short size = 0; //size of the box

unsigned int z = 0; // cardinal



// Returns 0 if is type node, 1 if is type way and 2 if relation. -1 if none of the above
int class_line(char *line)
{

	if(strcmp(line,"node")==0)
		return 0;
	else if (strcmp(line,"way")==0)
		return 1;
	else if (strcmp(line,"relation")==0)
		return 2;
	else
		return -1;
}

// Returns the index from the id node. 
long BinarySearch(unsigned long key, node *list, unsigned long ListLength)
{ 
	unsigned long start=0UL, AfterEnd=ListLength, middle;
	unsigned long try;

	while(AfterEnd > start)
	{ 
		middle = start + ((AfterEnd-start-1)>>1); try = list[middle].id; 
		if (key == try) return middle;
		else if ( key > try ) start=middle+1;
		else AfterEnd=middle;
	}

	return -1;
}




void reading (FILE *fp)
{
	int clase=0,counter = 0;
	char * last_clase = NULL;
	unsigned short oneway = 0;

	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	char *temp = NULL;

	if (fp == NULL) exit(EXIT_FAILURE);



	while((read = getline(&line, &len, fp))!=-1)
    { 
		while((temp=strsep(&line,"|"))!=NULL)
    		{
    			if (counter ==0)  
    			{
    				clase = class_line(temp); 

    				if (clase!=-1) {if(size>1) ConnectingSuccesors(oneway); free(auxiliar); auxiliar = NULL; size = 0; oneway = 0;}
    			}

				if (clase==0 ) {fnode(counter, temp);}

				else if(clase==1 ) 
					{ 
					
						if (counter==7 && strcmp(temp,"oneway")==0) oneway=1; 
						 
						if (counter>=9) ConnectingNodes(temp); 

					}

				else if (clase==-1 )

							ConnectingNodes(temp); 

							
		
				
				counter++;
			}

			z++;
			counter = 0;
			

	}

	
}

void writing(){
	FILE *fin;
	int i;
	
	unsigned long ntotnsucc=0UL;
	for(i=0;i<nnodes; i++) ntotnsucc+=nodes[i].nsucc;


	if((fin=fopen("binary.bin","wb"))==NULL) printf("The binary Output file can't be opened\n");

	/* Global data---header */
	 if((fwrite(&nnodes,sizeof(unsigned long),1,fin)+
	 	fwrite(&ntotnsucc,sizeof(unsigned long),1,fin))!=2) printf("When initializing the output binary data file\n");

	 /* Writting all nodes */

	 if( fwrite(nodes,sizeof(node),nnodes,fin)!=nnodes) printf ("When writing nodes to the output binary data file\n");

	//Writting successors in blocks
	for (i=0;i<nnodes; i++) 
		if(nodes[i].nsucc)
		{
			if(fwrite(nodes[i].successors,sizeof(unsigned long),nodes[i].nsucc,fin)!=nodes[i].nsucc)
			printf("When writing edges to the output binary data file\n");	
		}


	fclose(fin);
}



// If *temp node is registered (it appears as node) is stored in auxiliar, a box-type variable. 
// if node is not registered, we will skip it and link the previous to this to next to this
void ConnectingNodes(char * temp)
{

	long index = BinarySearch(atol(temp),nodes,nnodes);
	if (index == -1 ) return ; 

	size++;
	auxiliar = realloc(auxiliar,sizeof(box)*(size));
	auxiliar[size-1].id = atol(temp);
	auxiliar[size-1].index = index;

}

// It connects the differents nodes in the box variable auxiliar. 
void ConnectingSuccesors(int oneway)
{
	int i;
	
		for(i=0;i<size-1;i++)
    		{	    	

    		 	nodes[auxiliar[i].index].nsucc+=1;
    		 	nodes[auxiliar[i].index].successors = realloc(nodes[auxiliar[i].index].successors,sizeof(unsigned long)*nodes[auxiliar[i].index].nsucc);
    		 	nodes[auxiliar[i].index].successors[nodes[auxiliar[i].index].nsucc-1] = auxiliar[i+1].index;

    		 	if (oneway==0)
    		 	{
    		 		nodes[auxiliar[i+1].index].nsucc++;
    		 		nodes[auxiliar[i+1].index].successors = realloc(nodes[auxiliar[i+1].index].successors,sizeof(unsigned long)*nodes[auxiliar[i+1].index].nsucc);
    		 		nodes[auxiliar[i+1].index].successors[nodes[auxiliar[i+1].index].nsucc-1] = auxiliar[i].index;


    		 	}
    		}



}

// It registers the nodes and their parameters in a node array  
void fnode (int counter, char *temp){

	if(counter ==1) 		nodes[z].id = atol(temp);
    	else if (counter==9) 		nodes[z].lat = atof(temp);
    	else if (counter==10)  		nodes[z].lon = atof(temp);


}



int main (){

	int i;

	if((nodes = (node*)malloc(nnodes*sizeof(node)))==NULL) perror("When allocating memory for the nodes vector");
	
	int counterLoop=0;

	// Initialization

	for(i=0;i<nnodes;i++)
	{
		nodes[i].id = 0;
		nodes[i].nsucc = 0;
		nodes[i].successors = malloc(sizeof(unsigned long)*1);
		if( (i+1) % (nnodes/10) == 0){
			counterLoop++;		
			printf("Inizialization at %d % \n",counterLoop*10);
		
		}
	}
    
	printf("Finished inizialization, now Reading and Writing \nPlease expect at least 20 seconds and 3 minutes at maximum \n");
	

	time_t time;
	time = clock();

	FILE *fp; fp = fopen("andorra.csv","r"); reading(fp);  fclose(fp);  writing();

	time = clock()-time;

	printf("The elapsed time by reading and writing functions is: %fs \n",((double)time)/CLOCKS_PER_SEC);
	

	return 0;
}
