#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


#include "QMC.h"


/* function 'timer()' for mesuring processor time */

//#include "timer.c"
#include <math.h>


#define DIM 3

#define DEBUG 1

#define EPSILON .01
#define NSTEPS 280

int nmin;

void printVec(float *v)
{
  int i;
  printf("Vector is ");

  for (i=0;i<DIM;i++)
    {
      printf("%g ",v[i]);
    }
  printf("\n");

}

float dotProduct(float *v1,float *v2)
{
  float sum = 0.0;

  int i;
  /*
  printf("V1 ");
  printVec(v1);
  printf("V2 ");
  printVec(v2);
  */

  for ( i = 0;i< DIM; i++)
    {
      sum += v1[i]*v2[i];
    }
  
  // printf("Dotproduct is %g\n", sum); 


  return sum;
}

void normalize(float *vec)
{
  float sum = dotProduct(vec,vec);

  {int i;

  for ( i = 0;i< DIM; i++)
    {
      vec[i] = vec[i]/sqrt(sum);
    }
  }
}


#define N_NODE( i ) ( (i) - ndp + nmin )
#define MY_N_NODE( i ) ( (i) - ndp )


float randfloat()
{
  return ((float) rand())/((float) RAND_MAX);
}

/*-------------------------------------------------------------------------------------*/
void printMat(int n, float * adjMat){
  int j, i;

  printf ("\n\nin print Matrix:\n");
  for (i = 0; i < n; i++){
    for (j = 0; j < n; j++){
      if (adjMat[n*i +j] > 0){
	printf ("dist  between %d and %d is %g\n",
		i, j, adjMat[n*i +j]);
      }
    }
  }
}
/*-------------------------------------------------------------------------------------*/

double scoreNode  ( int n, float * adjMat, int * part, int j )
{
  double score=0;
  int i=0;
  int otherPart = 0;
  for (i = 0; i < n; i++){
    if (part[i] != part[j])
      {
	otherPart = 1;
	score += (double) adjMat[j*n + i];
      }
  }
  //  if(!otherPart) score = -1e+10;
  //  my_printf("Node %d on side %d has score %g\n", j, part[j],score);
  
  return score;
}

double scorePart  ( long n, float *adjMat, int *part )

{
  double score=0;
  
  {int j;
    for ( j=0;j<n;j++)
      {
	score += scoreNode(n, adjMat, part,j);
      }
  }
  my_printf("Computed score of partition to be %g\n", score);
  return score;
}


double  computeDelta(long n, float *adjMat, int *part, int j)
{
  double value=0;

  value -= scoreNode(n, adjMat, part,j);

  part[j] = (part[j] ? 0 : 1);


  value += scoreNode(n, adjMat, part,j);


  part[j] = (part[j] ? 0 : 1);

  if (0)
    {
      printf ("node %d, delta is %g\n", j, 2*value);
    }

  return 2*value;
  // REuven - return value;
}


// int *maxCutHack ( n, adjMat, p_ratio)
int  *maxCutHack (long  n, float *adjMat, int *minPart)
/*
long   n;         number of nodes 
float * adjMat;   adjacency  matrix  
int *minPart;
*/
{
  float ** vectors = NULL;
  int partCnt[2] = {0,0}; 
  int bestPartCnt[2] = {n,0}; // initialized to an empty cut (the lowest max)
  int *temp_part = NULL;
  
  int *bestPart = NULL;
 
  // keeps if the best partition is trivial (singleton or empty)
  int trivial_cut = 1; 
  int found_null_cut = 0;
  float  bestScore = 0; // initialized to an empty cut (the lowest max)
  int i;

  bestPart = (int *) my_calloc(n,sizeof(int)); // initialized to an empty cut (the lowest max) 
  temp_part = (int *) my_calloc(n,sizeof(int));
  vectors = (float **) my_calloc(n,sizeof(float *));
 
  
  for ( i=0;i<n;i++)
    {
      int dim;
      vectors[i] = NULL;
      vectors[i] = (float *) my_calloc(DIM,sizeof(float));
      for (dim=0;dim<DIM;dim++)
	{
	  vectors[i][dim]= randfloat()-0.5;
	}
      normalize(vectors[i]);

    }

 //    printMat(n, adjMat);

 //   printf ("after printMat\n\n");


 int step;
 //  printf ("Doing embedding.\n");
 
 
 for (step=0; step < NSTEPS; step++)
   {
     int j, i;
     for (j =0;j<n;j++){
       for (i = 0; i < n; i++){
	 
	 if (adjMat[j*n + i]){
	   
	   float *neighborVector = vectors[i];
	   int dim;
	   for (dim=0;dim< DIM;dim++)
	     {
	       vectors[j][dim] += -EPSILON*neighborVector[dim]* adjMat[j*n + i];
	     }
	 }
	 
       }

       normalize(vectors[j]);
     }
   }  // for steps 
 


#define PLANES 5
  //#define SCOREPART {for(int i=0 ;i < n; i++){ 
 
  if (bestScore > 0){
    EXIT ("Invaklid val for bestScore %g\n",bestScore);
  }
  {int i;  
  for (i=0; i< n; i++){
    if (bestPart[i] != 0){
      EXIT("BestPart is not initialized to 0 at cell %d\n",i);
    }
  }
  }
  {int tries, singleton_tries;
    for (singleton_tries = 0; (singleton_tries < 10) && (trivial_cut);
	 singleton_tries++){
      for (tries=0; tries < PLANES; tries++)
	{

	  float score;
	  float randVec[DIM];
	  int dim;
	  int i;
	  int *auxPtr;
	  for (dim=0;dim<DIM;dim++)
	    {
	      randVec[dim]= randfloat()-0.5;
	    }
	  // my_printf("Random vector is \n");  
	  //printVec(randVec);
      
	  partCnt[0] = partCnt[1] = 0;


	  for (i=0; i< n; i++)
	    {
	      /*
	      	  my_printf ("determining side\n");
		  printVec(vectors[i]);
		  my_printf ("Dot is %g\n", dotProduct(randVec,vectors[i]));
	      */

	      if (dotProduct(randVec, vectors[i]) <=0.0)
		{
		  temp_part[i] = 0;
		  partCnt[0]++;
		}
	      else
		{
		  temp_part[i] = 1;
		  partCnt[1]++;
		}
	    }
	  if (partCnt[0] == 0 || partCnt[1] == 0){
	    found_null_cut = 1;
	  }
          my_printf("part count %d %d\n",partCnt[0],partCnt[1]);
	  score = scorePart(n, adjMat, temp_part);
	  my_printf ("Cut score for hyperplane %d to %g.\n", tries, score);
	  if (bestScore <= score)
	    {
	      //
	      // We keep the new cut if scores are equal but prev cut was
	      // trivial (empty or singleton)
	      //
	      int new_trivial_cut =  ((partCnt[0] < 2) || (partCnt[1] < 2));
	      if ((trivial_cut &!new_trivial_cut)  || (bestScore < score)){
		my_printf("keeping the cut found\n");

		if (bestScore == score){
		  my_printf ("swap because of trivial part\n");
		}
		else 
		  my_printf("Found a better score %g > %g\n", score, bestScore);

		auxPtr = bestPart;
		bestPart = temp_part;
		bestPartCnt[0] = partCnt[0];
		bestPartCnt[1] = partCnt[1];
		temp_part = auxPtr;
		//		int improve = score - bestScore;
		trivial_cut = new_trivial_cut;
		bestScore = score;
	      }
	    }
	  else
	    {
	      //  do nothing
	    }
	}
      my_printf("part0 %d, part1 %d\n", bestPartCnt[0], bestPartCnt[1]);
      if (bestPartCnt[0] == 1 || bestPartCnt[1] == 1){
	my_printf("exited the PLANEs loop with singleton cut\n");
      }
    }
    if (bestPartCnt[0] == 1 || bestPartCnt[1] == 1){
      WARN("exited the singleton tries loop with singleton cut, tries %d, n=%ld, score=%g.\n",
	   singleton_tries,n, bestScore);
    }     
    if (bestScore < 0){
      ERR("Exited from loop after %d with negative bestScore %g"
	  " for n=%ld \n", singleton_tries, bestScore,n);
      if (found_null_cut){
	EXIT("BUG: negative bestScore %g but found null cut\n", bestScore);
	
      }
    }
  }
  


  my_free(temp_part); 
  {int i;
  for ( i=0;i<n;i++)
    {
      free (vectors[i]); vectors[i] = NULL;
    }
  }

  free (vectors); vectors = NULL;

  my_printf("\nbefore FM, print the optimal partition found\n");
  {int i;
    for ( i =0; i< n; i++) {
      my_printf ("Node %d is on side %d\n",
		 i, bestPart[i]);
      if (bestPart[i]> 1 || bestPart[i] < 0){
	ERR("invalid part number %d for node %d\n",bestPart[i], i);
      }
    }
  }
  /*-------------------------------------------------------------------------------
    Here we call the Fiduccia Mattheyses improvement to improve the cut found.
  ----------------------------------------------------------------------------------*/
  // Reuven - Need to skip it as it dosen't converges in weighted quartests 
     
     {int i,countPart;
       countPart = 0;
       for ( i =0; i< n; i++) {
	 countPart +=  bestPart[i];
       }
       my_printf("\n count parts %d\n", countPart);
       if(countPart !=0 && countPart != n)
	 {
	    my_printf("\n start FiducciaMattheyses!!!!!\n");
	   	   FiducciaMattheyses(bestPart, adjMat, n);
	 }
     }
     
  //  int negativeWeight=0;
  //  int positiveWeight=0;

  partCnt[0] = partCnt[1] = 0;

  for ( i =0; i< n; i++) {
    my_printf ("Node %d is on side %d\n",
	       i, bestPart[i]);
    if (bestPart[i]> 1 || bestPart[i] < 0){
      ERR("invalid part number %d for node %d\n",bestPart[i], i);
    }
    partCnt[bestPart[i]]++;
    /*
    {
    int j=0;

      for (j = 0; j < n; j++){
	if (part[j] != part[i])
	      {
		if (adjMat[j*n +i] < 0)
		  negativeWeight += adjMat[j*n +i]; 
		else
		  positiveWeight += adjMat[j*n +i]; 
	      }
      }
    }
    */
  }
  if (partCnt[0] + partCnt[1] != n){
    ERR("(partCnt[0] + partCnt[1]) do not sum to n=%ld rather to %d\n",
	n, (partCnt[0] + partCnt[1])); 
  }
  //  printf ("Partition has positive weight %d and negative weight %d \n", positiveWeight,
  // negativeWeight);
  
  /*
    We check whether the smaller part is empty or a singleton part
  */

  *minPart = (partCnt[0] > partCnt[1]? partCnt[1]: partCnt[0]);

  //  if ((!positiveWeight) || (positiveWeight <= negativeWeight)){
  if (!(*minPart)){
    // the trivial (empty cut) is the best;
    my_printf("The empty cut is the best cut\n");
    //    *p_ratio = 0;
  }
  else
    // if (! negativeWeight){  // no bad edges in the cut - the perfect cut
    if (*minPart == 1){
      my_printf ("\n\nA singleton part was found\n");
      //      *p_ratio = -1;    
    }
  /*
    else {
      *p_ratio = - (float) (positiveWeight) / negativeWeight;
      }
  */
  return bestPart;

	
}


/*------------------------------------------------------------------------*/


void FiducciaMattheyses(int * bestPart, float *adjMat, int n)
{
  int *temp_part = NULL;
 

  int improving = 1;
  int *frozen = NULL;
  int j;
  float *deltaScore = NULL;

  double score;
  double bestScore;
  double minScoreDiffThreshold = 0.0001; // A limit to computation absolute error  
  double minScoreRelThreshold = 0.01; // A relative limit to score improving    
  int switchNode = -1;
  double bestDelta = 0;

  temp_part = (int *) my_calloc(n, sizeof(int));
  frozen = (int *)    my_calloc(n, sizeof(int));
  deltaScore = (float *) my_calloc(n, sizeof(float));

  while(improving)
    {
      improving =0;
      {int i;
		
	if (!temp_part || !bestPart){
	  ERR("Invalid values for part %p and bestPart %p\n",
	      temp_part, bestPart);
	}
	for ( i=0;i< n;i++)
	  {
	    temp_part[i] = bestPart[i];
	    frozen[i] = 0;
	  }
      }
      score = scorePart(n, adjMat, temp_part);

      my_printf ("Cut score FM initially to %g.\n", score);

      bestScore = score;

 
      for (j=0;j< n;j++)
	{
	  deltaScore[j] = computeDelta(n, adjMat, temp_part, j);
	  my_printf ("Delta score for node %d is %g\n", j,deltaScore[j]);
	}

      {int nswap = 0;
      for (nswap=0; nswap < n; nswap++)
	{
      if (0)
	{
	  int i;
	  for (i=0; i< n;i++)
	    {
	      my_printf(" check-1: deltaScore is %g from node %d \n",  deltaScore[i],i);
	    }
	}
	  // findBestUnfrozenNode(part,frozen,deltaScore,n);



      if (0)
	{
	  int i;
	  for (i=0; i< n;i++)
	    {
	      my_printf(" check0: deltaScore is %g from node %d \n",  deltaScore[i],i);
	    }
	}
          int foundNewSwitchNode = 0;
	  for (j = 0; j<n;j++)
	    {
	      if (!frozen[j])
		{
		  if ((switchNode < 0) || bestDelta < deltaScore[j])
		    {
		      switchNode = j;
		      bestDelta = deltaScore[j];
		      foundNewSwitchNode = 1;
      //		      if (0)
			{
			  my_printf(" bestdelta is %g from node %d \n",  bestDelta,j);
			}
		    }

		}
	    }
	  if (foundNewSwitchNode == 0 ) {my_printf ("done with pass of f-m\n"); break;}
	  for (j = 0; j<n;j++) {
	    if (adjMat[switchNode * n + j]){

	      if (temp_part[j] == temp_part[switchNode])
		deltaScore[j] -= 4 * adjMat[switchNode * n + j];
		//Reuven - deltaScore[j] -= adjMat[switchNode * n + j];
	      else
		deltaScore[j] += 4 * adjMat[switchNode * n + j];
	      // deltaScore[j] += adjMat[switchNode * n + j];
	      if (DEBUG)
		{
		  my_printf (" other node %d has new delta %g\n", j, deltaScore[j]);

		}
	    }
	  }

	  temp_part[switchNode] = (temp_part[switchNode] ? 0 : 1);
	  frozen[switchNode] = 1;
	  if (DEBUG)
	    {
	      int i;
	      for (i=0; i< n;i++)
		{
		  my_printf(" check1: deltaScore is %g from node %d \n",  deltaScore[i],i);
		}
	    }
	  my_printf("----x----- Recomputed  score in FM  %g\n", scorePart(n, adjMat, temp_part));

	  score +=deltaScore[switchNode];
	  deltaScore[switchNode] = - deltaScore[switchNode];
	  my_printf("Made FM move number %d switched Node %d to side %d and score is now %g\n", 
		 nswap, switchNode, temp_part[switchNode], score);
	  if (DEBUG)
	    {int i;
	      for (i=0; i< n;i++)
		{
		  my_printf("check2: deltaScore is %g from node %d \n",  deltaScore[i],i);
		}
	    }
	  if (0)
	    {
	      double testScore = scorePart(n, adjMat, temp_part);
	      my_printf("Made FM move number %d switched Node %d to side %d and score is now %g\n", 
		 nswap, switchNode, temp_part[switchNode], score);
	      my_printf("Recomputed  score in FM  %g\n", testScore);
	      double tmpScore = abs(score);
	      if (tmpScore == 0) 
		tmpScore += 0.0001; 
	      if (abs(score - testScore)/tmpScore > minScoreRelThreshold && 
		  (abs(score - testScore) > minScoreDiffThreshold))
		{
		  printf ("ERROR ERROR ERROR: found different scores %g and %g - %g \n", score, testScore,
			  minScoreDiffThreshold);
		exit (7);
	      }
	    }
	  float tmpScore = abs(score);
	  if (tmpScore ==0) tmpScore += 0.0001; 
	  if ((score - bestScore)/tmpScore > minScoreRelThreshold && 
	      (abs(score - bestScore) > minScoreDiffThreshold))
	    {
	      my_printf("New best score in FM %d from %g to %g\n", nswap, bestScore,score);
	      bestScore = score;
	      if (DEBUG)
		{
		  my_printf("Recomputed  score in FM  %g\n", scorePart(n, adjMat, temp_part));
		}
	      {int i;
	      for (i=0;i< n;i++)
		{
		  bestPart[i] = temp_part[i];
		  improving = 1;
		}	    
	      }
	    }

	  if (0)
	    {
	      int i;
	      for (i=0; i< n;i++)
		{
		  my_printf(" check4-: deltaScore is %g from node %d \n",  deltaScore[i],i);
		}
	    }
	  if (0)
	    {
	      int i;
	      for (i=0; i< n;i++)
		{
		  printf(" check4: deltaScore is %g from node %d \n",  deltaScore[i],i);
		}
	    }
	}
	  if (0)
	    {
	      int i;
	      for (i=0; i< n;i++)
		{
		  printf(" check3: deltaScore is %g from node %d \n",  deltaScore[i],i);
		}
	    }
      }
    }  // while (improving)

  free(frozen); frozen = NULL;
  free (deltaScore); deltaScore = NULL;
  free(temp_part); temp_part  = NULL;

}

/*------------------------------------------------------------------------*/



int * cutGraph(
	       int n,                // number of nodes (adjacency matrix size)
	       float * adjMat,         // adjacency matrix 
	       //	       float * p_ratio)      // The ratio between positive and negative edges found
	       int *minPart) 	/* The size of the smallest part  */
{

  //   float t;
   int *bestPart = NULL;



   //   srand ( 2* (unsigned)time ( NULL ) );
   my_printf("c\nc maxcut Hack - \nc\n");




   my_printf("c nodes: %d\n", n);
   


   bestPart = maxCutHack ( n, adjMat, minPart);


   return bestPart;

 }
