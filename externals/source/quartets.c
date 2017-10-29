

#include <time.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include "QMC.h"
#include "quartets.h"




/*---------------------------------------------------------------------*/
void printQrt(const char *header, qrtType &qrt)
{
  printf("%s qrt: ",header);
  for(int i=0;i<4;i++)
    {
      printf(" %d",qrt.n[i]);
    }
  printf("\n");
}

/*---------------------------------------------------------------------*/
void printWeight(const char *header,qrtWeightType &weight)
{
  printf("%s weight: ",header);
  for(int i=0;i<3;i++)
    {
      printf(" %g",weight.weight[i]);
    }
  printf("\n");
}

/*---------------------------------------------------------------------*/

int floatComp(const void * in_a,const void * in_b) 
{
  const float *a = (const float *)  in_a;
  const float *b = (const float *) in_b;  
  
  if (*a==*b)
    return 0;
  else
    if (*a < *b)
        return -1;
     else
      return 1;
}

int intComp (const void * a, const void * b)
{
  return ( *(int*)a - *(int*)b );
}



/* Compute the qrt weight according to the four point method */
/*--------------------------------------------------------------------------*/



/* Init a new node with taxa and weight values */

void initNode(qrtNode &node,qrtType &qrt,qrtConfigType qrtType, float weight)
{
  if(qrtType == QRT_CONFIG_0123 || qrtType == NONE)
    {
      node.species[0] = qrt.n[0];
      node.species[1] = qrt.n[1];
      node.species[2] = qrt.n[2];
      node.species[3] = qrt.n[3];
      node.weight=weight;
      return;
    }
  if(qrtType == QRT_CONFIG_0213)
    {
      node.species[0] = qrt.n[0];
      node.species[1] = qrt.n[2];
      node.species[2] = qrt.n[1];
      node.species[3] = qrt.n[3];
      node.weight=weight;
      return;
    }
  if(qrtType == QRT_CONFIG_0312)
    {
      node.species[0] = qrt.n[0];
      node.species[1] = qrt.n[3];
      node.species[2] = qrt.n[1];
      node.species[3] = qrt.n[2];
      node.weight=weight;
      return;
    }
}

 /* get distance besteen pairs from the distance matrix */
/*--------------------------------------------------------------------------*/

float getDist(float *distMat,int i,int j, int ntaxa, float maxDist)
{
  float len =  distMat[i*ntaxa + j];
  if (len == -1) 
    len = maxDist;
  return len;
}

/* Compute the qrt weight according to the four point method */
float computeWeight(float high, float med, float low)
{
  return((high - low)/(2*high*(high-med+0.1)));
}

/* Computes the maximum distance between every pair of taxa names */
/*--------------------------------------------------------------------------*/
float getMaxDist(float *distMat, int ntaxa)
{
  int i,j;
  float max = distMat[0];
  float min = distMat[0];
  for(i =0; i < ntaxa; i++)
      for(j =0; j < ntaxa; j++)
	{
	if (distMat[i*ntaxa + j] > max)
	  max = distMat[i*ntaxa + j];
	else
	  if (distMat[i*ntaxa + j] < min)
	    min = distMat[i*ntaxa + j];
	}
  return max;
}

void swap(int &a, int &b)
{
  int tmp = b;
  b = a;
  a = tmp;
}

void sortIndex(int intArray[], int indexArray[],int size)
{

  int i,j;
  for(i=0;i<size;i++)
    indexArray[i] = i;

  for(i=0;i< size;i++)
    for(j=0;j<size-1;j++)
      {
	if(intArray[indexArray[j]] > intArray[indexArray[j+1]])
	  swap(indexArray[j],indexArray[j+1]);
      }
}

/* Add quartet weight to the qrt weights 4D array */
/*--------------------------------------------------------------------------*/

void addNewQuartet(float *distMat, int *spcArray,
		   qrtArrayType &qrtWeights, int ntaxa,float maxDist, 
		   int t0,int t1, int t2, int t3,bool addToQrtArray,qrtType &currentQrt, 
		   qrtWeightType &qrtWeight)
{
  my_printf ("at addNewQuartet %d,%d,%d,%d\n",t0,t1,t2,t3);
  qrtNode newNode;
  int minLengthTreshold = 0.00001;
  
  float weight;
  
  int sortedSpcIndex[4];

  int origQrt[4] = {t0,t1,t2,t3};
  int spc[4] = {spcArray[t0],spcArray[t1],spcArray[t2],spcArray[t3]};
  
  if (spc[0] == spc[1] ||
      spc[0] == spc[2] || 
      spc[0] == spc[3] || 
      spc[1] == spc[2] || 
      spc[1] == spc[3] || 
      spc[2] == spc[3]){
    printf ("%d,%d,%d,%d - ERROR ERROR ERROR: at add new qrt - two species are the same\n",spc[0],spc[1],spc[2],spc[3]);
    exit(8);
  }
  
  // The qrt is sorted according to the spc name 
  // for having unique quartet for different orders of qrt taxa.
  
  sortIndex(spc,sortedSpcIndex,4);
  
  int sortedSpc[4] = {spc[sortedSpcIndex[0]],spc[sortedSpcIndex[1]],
		      spc[sortedSpcIndex[2]],spc[sortedSpcIndex[3]]};
  
  
  
  int sortedQrt[4] = {origQrt[sortedSpcIndex[0]],origQrt[sortedSpcIndex[1]],
		      origQrt[sortedSpcIndex[2]],origQrt[sortedSpcIndex[3]]};
  
  
  currentQrt.set(sortedSpc[0],sortedSpc[1],sortedSpc[2],sortedSpc[3]);
  
  if(addToQrtArray > 0)
    {      
      qrtWeight = qrtWeights[currentQrt];
    }
  else
    {
      qrtWeight.reset();
    }
  
  float len01 = getDist(distMat,sortedQrt[0],sortedQrt[1],
			ntaxa,maxDist);
  float len02 = getDist(distMat,sortedQrt[0],sortedQrt[2],
			ntaxa,maxDist);
  float len03 = getDist(distMat,sortedQrt[0],sortedQrt[3],
			ntaxa,maxDist);
  float len12 = getDist(distMat,sortedQrt[1],sortedQrt[2],
			ntaxa,maxDist);
  float len13 = getDist(distMat,sortedQrt[1],sortedQrt[3],
			ntaxa,maxDist);
  float len23 = getDist(distMat,sortedQrt[2],sortedQrt[3],
			ntaxa,maxDist);

  float len0123 =  len01 + len23;
  float len0213 =  len02 + len13;
  float len0312 =  len03 + len12;

  // qsort(qrt,4,sizeof(int),intComp);

  //  printf ("spc at addNewQuartet %d,%d,%d,%d\n",sortedSpc[0],sortedSpc[1],sortedSpc[2],sortedSpc[3]);

  float lenDiff = 0;
  float lenAddDiff = 0.1;
 
  if ((len0123 <= len0213) && (len0123 <= len0312) && 
      (len0123 > minLengthTreshold))
    {
      if(len0213 > len0312)
	weight = computeWeight(len0213,len0312,len0123);
      else
	weight = computeWeight(len0312,len0213,len0123);
      
      qrtWeight.weight[0] += weight;
      
    }
  else 
    if ((len0213 <= len0123) && (len0213 <= len0312) && 
	(len0213 > minLengthTreshold))
      {
	if(len0123 > len0312)
	  weight = computeWeight(len0123,len0312,len0213);
	else
	  weight = computeWeight(len0312,len0123,len0213);
	
	qrtWeight.weight[1] += weight;
      }
    else  
      if ((len0312 <= len0123) && (len0312 <= len0213) && 
	  (len0312 > minLengthTreshold) )
	{	
	  if(len0123>len0213)
	    weight = computeWeight(len0123,len0213,len0312);
	  else
	    weight = computeWeight(len0213,len0123,len0312);
	  
	  qrtWeight.weight[2] += weight;	  
	}

  
  if(addToQrtArray)
    {	 
      qrtWeight.cnt++;
      // printQrt("add qrt: ",currentQrt);
      // printWeight(" ",qrtWeight);	  
      qrtWeights[currentQrt] = qrtWeight;
    }
}
 

/*--------------------------------------------------------------------------*/
 /* Add weights to the qrt 4D arry */

qrtNode * addQuartetsWeights(float *distMat, int *spcArray, qrtArrayType &qrtWeights,
			     int ntaxa, int pcorrect,  bool weightedPcorrect, 
			     bool withWeights,bool addToQrtArray)
{
  
  /* Reuven - generate spc random permutation array*/
  
  //  struct qrtStruct * prev, * current, *q_anchor ;
  
  my_printf ("at genQrts\n");

  float maxDist = getMaxDist(distMat, ntaxa);
  int i,t0,t1,t2,t3;
  qrtWeightType qrtWeight;
  qrtType qrt;

  for (t3 = 3; t3 < ntaxa; t3++) 
    for (t2 = 2; t2 < t3; t2++) 
      for (t1 = 1; t1 < t2; t1++)
	for (t0 = 0; t0 < t1; t0++)
	  {
	    addNewQuartet(distMat, spcArray, qrtWeights,ntaxa, 
			  maxDist, t0,t1,t2,t3,addToQrtArray,qrt,
			  qrtWeight);	    	    
	  }
  printf("\n\n\nFinished with all quartets.\n");

  
} /* computeallquartets */

/*--------------------------------------------------------------------------*/

qrtNode * addRandomQuartetsWeights(float *distMat, int *spcArray, qrtArrayType &qrtWeights,
				   int ntaxa, int pcorrect,  bool weightedPcorrect,
				   unsigned long qnum, bool withWeights,
				   bool addToQrtArray)
{
  
  
  printf ("at genQrts\n");

  float maxDist = getMaxDist(distMat, ntaxa);
  int i,t0,t1,t2,t3;

  unsigned long current_qnum = 0;
  qrtWeightType qrtWeight;
  qrtType qrt;
  qrtWeights.get_allocator().allocate(qnum);

  while(current_qnum < qnum) 
    {
      genRandomQrt(ntaxa,t0,t1,t2,t3);                
      addNewQuartet(distMat, spcArray, qrtWeights,ntaxa, 
		    maxDist, t0,t1,t2,t3,addToQrtArray,qrt,
		    qrtWeight);
      current_qnum++;
    }
  printf("\n\n\nFinished with all quartets.\n");
  
} /* computeallquartets */


/*--------------------------------------------------------------------------*/

void printQrt(qrtNode &node, FILE * qrtFile, bool withWeights)
{
  fprintf(qrtFile,"%d,%d|%d,%d",
	  node.species[0],
	  node.species[1],
	  node.species[2],
	  node.species[3]);
  if(withWeights) 
    fprintf(qrtFile,":%f", node.weight);
  fprintf(qrtFile," ");
}



/*--------------------------------------------------------------------------*/
void addQrt2list(qrtNode **q_anchor, qrtNode &node)
{
  qrtNode * current = (qrtNode *) my_malloc (sizeof (qrtNode));
  *current = node;
  current->next = *q_anchor;
  *q_anchor = current;
}



/*--------------------------------------------------------------------------*/
/* Get quartet from the weighted array and add it to the qrt list */

void handelWeightedQrt(qrtType &qrt, qrtWeightType &qrtWeight,  
		    qrtNode **q_anchor, int ntaxa, FILE *qrtFile, int genQrtList, 
		    bool withWeights, unsigned long &qnum)
{
  qrtNode node;
  qrtConfigType qrtConfig;
  bool found = false;
  for(int i = 0; i<3;i++)
    {
      if(qrtWeight.weight[i] > 0)
	{
	  found = true;
	  if(i==0) qrtConfig = QRT_CONFIG_0123;
	  else
	    if(i==1) qrtConfig = QRT_CONFIG_0213;
	    else
	      qrtConfig = QRT_CONFIG_0312;

	  initNode(node,qrt,qrtConfig,qrtWeight.weight[i]);
	  
	  if(genQrtList)
	    {
	      addQrt2list(q_anchor, node);	     
	    }

	  if(qrtFile)
	    {
	      printQrt(node,qrtFile,withWeights);
	    }
	  qnum++;
	}
    }
  if(!found)
    {
      initNode(node,qrt,NONE,0);
      printQrt(node,qrtFile,withWeights);
      qnum++;
    }
}

/*--------------------------------------------------------------------------*/
void genRandomQrt(int ntaxa, int &t0,int &t1,int &t2,int &t3)
{
  t0 = rand() % ntaxa;
  t1 = t0;
  while(t1 == t0)
    t1 = rand() % ntaxa;
  t2 = t1;
  while((t2 == t1) || (t2 == t0))
    t2 = rand() % ntaxa;
  t3 = t1;
  while((t3 == t0) || (t3 == t1) || (t3 == t2))
    t3 = rand() % ntaxa;
}


/*--------------------------------------------------------------------------*/
unsigned long handleAllQrts(qrtArrayType &qrtWeights,  qrtNode **q_anchor, 
			int ntaxa, char *qrtFname, int genQrtList, 
			bool withWeights)
{
  *q_anchor = NULL;
  int i,t0,t1,t2,t3;
  unsigned long qnum = 0;
  qrtNode node;

  FILE *qrtFile = NULL;
  if(qrtFname)
    qrtFile = fopen(qrtFname,"w");
  
  for (qrtArrayIter qrtIter = qrtWeights.begin(); qrtIter != qrtWeights.end();qrtIter++)
    {
      //	    printf("qrt %d %d %d %d\n",t0,t1,t2,t3);
      qrtType qrt = (*qrtIter).first;
      qrtWeightType qrtWeight = (*qrtIter).second;
      
      handelWeightedQrt(qrt, qrtWeight,q_anchor, ntaxa, qrtFile, genQrtList, 
			withWeights, qnum);   
    }
  
  if(qrtFname)
    fclose(qrtFile);
  return qnum;
}

/*--------------------------------------------------------------------------*/

float getQrtWeight (qrtWeightType &qrtWeight)
{
  for(int i = 0;i < 3;i++)
    if(qrtWeight.weight[i]> 0)
      {
	return(qrtWeight.weight[i]);
      }
  return(0);
}

/*--------------------------------------------------------------------------*/
float getQrtAvgWeight(qrtArrayType &qrtWeights)
{
  long wnum = 0;
  float totalWeight = 0;
  for (qrtArrayIter qrtIter = qrtWeights.begin(); 
       qrtIter != qrtWeights.end();qrtIter++)
    {
     float  weight = getQrtWeight((*qrtIter).second);
      if(weight > 0)
	{
	  totalWeight += weight;
	  wnum++;      
	}
    }
  return totalWeight/wnum;
}

/*--------------------------------------------------------------------------*/
bool getFlip(int pcorrect)
{
  int rand_percent = rand() % 100;
  return(rand_percent >= pcorrect);
}

/*--------------------------------------------------------------------------*/
bool getWeightedFlip(int pcorrect,float weight,float avgWeight)
{
  float rand_percent = rand() % 100;
  return((rand_percent/weight) >= (pcorrect/avgWeight));
}

/*--------------------------------------------------------------------------*/

void flipQrt(qrtWeightType &qrtWeight)
{
  float flippedWeight[3];

  int inc = rand() % 2;
  for(int i = 0;i<3;i++)
    {
      int flippedIndex = (i + 1 + inc) % 3;
      flippedWeight[i] = qrtWeight.weight[flippedIndex];
    }
  for(int i = 0;i<3;i++)
    {
      qrtWeight.weight[i] = flippedWeight[i];      
    }
}


/*--------------------------------------------------------------------------*/
void flipQrts(qrtArrayType &qrtWeights, int pcorrect, bool weightedPcorrect)
{
  long wnum = 0;
  float totalWeight = 0;

  if(pcorrect == 100)
    return;
  
  float qrtAvgWeight;

  if(weightedPcorrect)
    qrtAvgWeight = getQrtAvgWeight(qrtWeights);

  unsigned long flips=0;
  unsigned long qnum=qrtWeights.size();
  float maxFlips= (100-pcorrect)*qnum/100;

  // Need at most two passes on qrt list

  for(int i = 1;i<=2;i++)
    {
      for (qrtArrayIter qrtIter = qrtWeights.begin(); 
	   qrtIter != qrtWeights.end();qrtIter++)
	{
	  qrtWeightType qrtWeight = (*qrtIter).second;
	  float weight = getQrtWeight(qrtWeight);
	  
	  if((!weightedPcorrect && getFlip(pcorrect)) ||
	     (weightedPcorrect && getWeightedFlip(pcorrect,weight,qrtAvgWeight)))
	    {
	      flips++;
	      //  printf("flipped %d %d %g %g %g \n",flips,qnum,maxFlips,weight,qrtAvgWeight);
	      flipQrt((*qrtIter).second);
	    }
	  if(flips >= maxFlips) 
	    return;
	}
    }
}


/*--------------------------------------------------------------------------*/
unsigned long genAllMatQrts(qrtArrayType &qrtWeights, float *distMat,int *spcArray, 
		   qrtNode **q_anchor, char *qrtFname, int genQrtList, 
			    bool withWeights, int ntaxa, 
			    int pcorrect, bool weightedPcorrect,bool addToQrtArray)
{
  *q_anchor = NULL;
  int i,t0,t1,t2,t3;
  unsigned long qnum = 0;
  FILE *qrtFile;
  qrtType qrt;
  qrtWeightType qrtWeight;
  float maxDist = getMaxDist(distMat, ntaxa);

  bool localAddToQrtArray = addToQrtArray || weightedPcorrect;

  if(qrtFname && !weightedPcorrect)
    qrtFile  = fopen(qrtFname,"w");
    
  for (t3 = 3; t3 < ntaxa; t3++) 
    for (t2 = 2; t2 < t3; t2++) 
      for (t1 = 1; t1 < t2; t1++)
	for (t0 = 0; t0 < t1; t0++)
	  {	   
	    addNewQuartet(distMat, spcArray, qrtWeights,ntaxa,  
			  maxDist, t0,t1,t2,t3,localAddToQrtArray,qrt,qrtWeight);  

	    
	    // Quartete will be handeled later
	    
	    if(localAddToQrtArray)
	      continue;
	    
	    if((pcorrect < 100) && getFlip(pcorrect))
	      {
		flipQrt(qrtWeight);
	      }
	    handelWeightedQrt(qrt, qrtWeight,q_anchor, ntaxa, qrtFile, genQrtList, 
			      withWeights, qnum);
	  }
  if(qrtFname && !weightedPcorrect)
    fclose(qrtFile);
  return qnum;
}



/*--------------------------------------------------------------------------*/
unsigned long genRandomMatQrts(qrtArrayType &qrtWeights, float *distMat,int *spcArray, 
		   qrtNode **q_anchor, char *qrtFname, int genQrtList, 
			       bool withWeights, int ntaxa, 
			       int pcorrect,bool weightedPcorrect, 
			       unsigned long qnum,bool addToQrtArray)
{
  *q_anchor = NULL;
  int i,t0,t1,t2,t3;
  qrtType qrt;
  FILE *qrtFile;
  float maxDist = getMaxDist(distMat, ntaxa);
  bool localAddToQrtArray = addToQrtArray || weightedPcorrect;
  
  if(qrtFname && !weightedPcorrect)
    qrtFile  = fopen(qrtFname,"w");    
  
  unsigned long current_qnum = 0;
  unsigned long tmp_qnum = 0;
  while(current_qnum < qnum) 
    {
      genRandomQrt(ntaxa,t0,t1,t2,t3);          
      qrtWeightType qrtWeight;
      
      addNewQuartet(distMat, spcArray,qrtWeights,ntaxa,
		    maxDist, t0,t1,t2,t3,localAddToQrtArray,qrt,qrtWeight);  

      current_qnum++;
      //      printf("current qnum: %d %d\n",current_qnum,qnum);
      // Quartete will be handeled later
 
      if(localAddToQrtArray)
	continue;
      
      if((pcorrect < 100) && getFlip(pcorrect))
	{
	  flipQrt(qrtWeight);
	}
      
      handelWeightedQrt(qrt, qrtWeight,q_anchor, ntaxa, qrtFile, genQrtList, 
			withWeights, tmp_qnum);
      
    }
  
  if(qrtFname && !weightedPcorrect)
    fclose(qrtFile);   
  
  return current_qnum;
}

/*---------------------------------------------------------------------*/
unsigned long readQuartets(char * qrtFname, qrtNode ** q_anchor, eleNode ** e_anchor, int *maxEle, bool withWeights){
  FILE *qrtFile = fopen(qrtFname, "r");
  unsigned long n_quartets;
  qrtNode * prev, * current;

  unsigned long qnum=0;
  float minWeight,maxWeight;
  if (qrtFile == NULL){
    printf ("open for %s failed\n",qrtFname);
    exit (0);
  }
  //  exit (5);
  prev  = (*q_anchor) = (qrtNode *) my_malloc (sizeof(qrtNode));
  prev -> next = NULL;
  getQrt(*q_anchor, qrtFile, maxEle, withWeights);
  qnum=1;

  current = (qrtNode *) my_malloc (sizeof(qrtNode));

  while (	getQrt(current, qrtFile, maxEle, withWeights)){

    prev -> next = current;
    
    prev = current;
    prev -> next = NULL;
    current = (struct qrtStruct *) my_malloc (sizeof(struct qrtStruct));

    qnum++;
    if (qnum%1000000==0){
      printf("Read %ld quartets\n",qnum);
    }
  }

  free (current);
  n_quartets = qnum;
  //  exit (6);
  my_printf ("\n\n\nNumber of quartets is %d, max element %d\n\n", n_quartets, *maxEle);
  //	exit (3);
  int * eleVec = (int *) my_calloc (*maxEle+8, sizeof(int) );
  //  eleNode **currEle = e_anchor;
  for ( current = *q_anchor; current != NULL; current = current -> next){
    int i;
    for (i =0; i < 4; i++){
      if (eleVec[current->species[i]]){
	//	printf("Not first time to see %d\n", current->species[i]);
      }
      else{
	my_printf("first time to see %d\n", current->species[i]);
	eleVec[current->species[i]] = 1;
	addElement2list(e_anchor, current->species[i]);
      }
    }
    
  }

  
  return n_quartets;
}


/*---------------------------------------------------------------------*/

int   getQrt(qrtNode *q, FILE * f, int *maxEle, bool withWeights){
  //  printf ("at getQrt, max ele is %d\n\n", *maxEle);
  char line [100] = "";
  
  //  printf ("line is %s, len %d\n\n",line, strlen(line));
  
  int numRead = 0;

  fscanf(f,"%s",line);
  
  if(withWeights)
    {
      
    numRead = sscanf(line,"%d,%d|%d,%d:%f",
                     &q->species[0], &q->species[1], &q->species[2], &q->species[3],&q->weight);
    }
  else 
    {
      numRead = sscanf(line,"%d,%d|%d,%d",
		       &q->species[0], &q->species[1], &q->species[2], &q->species[3]);
      q->weight = 1;
    }
  
  
    if  ( feof(f))
    {
      my_printf ("reached EOF\n\n");
      return 0;
    }

  
  if (((withWeights) && (numRead != 5)) || 
	     ((!withWeights) && (numRead != 4))) 
    {
      printf ("ERROR ERROR ERROR: invalid quartet: %s\n\n", line);
      exit(8);
    }
  
  else {
    int i;
    //    printf ("found a quartet %d,%d|%d,%d with weight %g \n\n", q->species[0], q->species[1], q->species[2], q->species[3], q->weight);
    for ( i=0; i < 4; i++){
      if (*maxEle < q->species[i]){
	my_printf ("found a bigger element than %d: %d\n", *maxEle, q->species[i]);
	*maxEle = q->species[i];
      }
    }
  } 
  return 1;
}

