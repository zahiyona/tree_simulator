#include <time.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
 

// #define DEBUG_PRT

#include "QMC.h"
#include "quartets.h"



/*---------------------------------------------------------------------*/
  
void	parseParms(int argc, char *argv[], char **qrtFname, char **treeFname, int *withWeights){
	int i;
	*qrtFname =  NULL;
	my_printf ("at parse parm. argc %d\n",argc);



	for (i=1; i< argc; i++){
		my_printf ("parm %d is %s\n",i,argv[i]);

		if (! strncmp(argv[i],"qrtt=",5)){
	 		printf("found qrtt=%s\n", (argv[i]+5));
			*qrtFname = argv[i]+5;

		}
		if (! strncmp(argv[i],"otre=",5)){
			printf("found otre=%s\n", (argv[i]+5));
			*treeFname = argv[i]+5;

		}
		if (!strncmp(argv[i],"weights=on",10)){
			printf("found weights=on\n");
			*withWeights = 1;
		}
	}

	if (*qrtFname == NULL ){
		printf("ERROR !!!!!  Invalid input");
		printf("\nUsage: %s  qrtt=<quartets-file> [-weight] [filter=<filter percent>] "
		       "[otre=<output tree name> (def MXCNtree.dat)]\n",argv[0]);
		exit(2);
	}

}



/*---------------------------------------------------------------------*/




int main(int argc, char *argv[])
{
 
  unsigned long qcnt=0;
  int ecnt=0,cnt=0,i;
  char   *qrtFname=NULL;
  char   *treeFname=NULL;
  int    withWeights=0;

  //  int alpha  = 3;

  qrtNode * q_anchor=NULL;
  eleNode * e_anchor=NULL, * currE = NULL;
  int maxSp = -1;
  time_t t1;	
  treeNode *finalTree;  
  FILE *ftree;

  printf("\n\nQuartet MaxCut version 3.0 by Sagi Snir, University of Haifa\n\n");
  my_printf  ("size of short %d, int %d\n", 
	    sizeof (short),
	    sizeof(int));
  
  parseParms(argc, argv, &qrtFname, &treeFname,&withWeights);
  printf("quartet file is %s, \n",qrtFname);
  
  //  exit (7);
qcnt = readQuartets(qrtFname, &q_anchor, &e_anchor, &maxSp, withWeights);

  printf ("At main, back from readQuartets, maxSp %d\n", maxSp);

  unsigned long current_qnum = 0;
  qrtNode * current;
  for (current_qnum=0, current = q_anchor; current != NULL; current = current -> next,current_qnum++){
    my_printf ("quartet %d: %d,%d|%d,%d\n",curent_qnum,
	       current->species[0],
	       current->species[1],
	       current->species[2],
	       current->species[3]
	       );
    
  }
  
  if (current_qnum != qcnt){
    my_printf ("\n\n\n %d %d ERROR ERROR ERROR\n\n\n");
  }
  

 for (i=0, currE = e_anchor; currE != NULL; currE = currE -> next){
    ecnt++;
    my_printf("at ele %d\n", currE -> species);
 }
 
 printf ("\n\nNumber of quartets read: %d, max ele %d\n\n\n", qcnt,  maxSp);
 					      
 t1 = time(NULL);						

 printf("Started working at  %s\n", asctime(localtime(&t1)));

 // ERR("STAM STAM");
 // EXIT("staM STAM");


 
 finalTree =
 
   find_cut(e_anchor, ecnt, q_anchor, qcnt, maxSp, &cnt);
 // find_constant_ratio_cut(e_anchor, ecnt, q_anchor, qcnt, maxSp, alpha);
 
   printf("calling find cut e_anchor, %d, q_anchor, %d, %d, %d\n",
        ecnt, qcnt, maxSp, cnt);

 if (treeFname == NULL){
   ftree = fopen ("MXCNtree.dat","w");
 }
 else{
   ftree = fopen (treeFname,"w");
 }


 if (!ftree){
   ERR("Could not open output tree file MXCNtree.dat\n\n");
 }

 t1 = time(NULL);						
 printf("Ended working at  %s\n", asctime(localtime(&t1)));

 print_tree2file(finalTree, ftree);
 fprintf(ftree,";\n");
 fclose(ftree);

 return 0;
}



