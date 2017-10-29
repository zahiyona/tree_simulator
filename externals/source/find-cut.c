#include <time.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
 

// #define DEBUG_PRT

#include "QMC.h"



void *my_calloc(int n, int size){
  void *addr = calloc(n, size);
  if (addr) {return addr;}
  else {
    //    ERR("Calloc for space %d failed\n",size);
    printf("ERROR RROR ERROR: Calloc for space %d failed\n",size);
    exit(8);
    
  }
}



void *my_malloc(int size){
  void *addr = malloc(size);
  if (addr) {return addr;}
  else {
    //    ERR("Malloc for space %d failed\n",size);
    printf("ERROR RROR ERROR: Malloc for space %d failed\n",size);
    exit(8);
    
  }
}


#define TESTIND(a,b) {if (a < 0 || a > b) \
      {printf("ERROR ERROR ERROR. Invalid value for " #a " %d\n",a); exit(8);} \
  }
/*---------------------------------------------------------------------*/
  

/*---------------------------------------------------------------------
  this procedure adds an element to a list  

---------------------------------------------------------------------*/

void addElement2list(eleNode **e_anchor, int eleId)
{
  eleNode * temp = (eleNode *) my_malloc (sizeof (eleNode));
  temp -> species = eleId;
  temp -> next = *e_anchor;
  *e_anchor = temp;
}

/*---------------------------------------------------------------------*/
  
treeNode *make_star_tree(eleNode * e_anchor, int ecnt, int maxSp, int *pCnt){

  int i=0;
  treeNode * father = newNode (NULL,(*pCnt)++);
  eleNode * current = e_anchor;

  while (current)     {
    treeNode * leaf = newNode (NULL,(*pCnt)++);
    leaf->val = current->species;
    addSon(father, leaf);
    e_anchor = current;
    current = current -> next;
    free(e_anchor);
    i++;
  }
  
  return father;
}

/*---------------------------------------------------------------------*/

void printPart (int *part, int graphSize)
{
  int i=0;
  int partCnt[2] = {0,0};
  printf("\n\nPart returned for graph with %d nodes is\n", graphSize);
  for (i=0; i < graphSize; i++){
    printf ("element %d is on side %d\n", i, part[i]);
  }
  printf("\nNumber of elements in parts is %d/%d\n\n",partCnt[0], partCnt[1]);
}

/*---------------------------------------------------------------------*/

void handleQuartet(qrtNode * qptr, int * sp2i, int graphSize, float *goodEdges, float *badEdges){
  /*
  printf ("at handle quartet for quartet : %d,%d|%d,%d\n",
		    qptr->species[0],
		    qptr->species[1],
		    qptr->species[2],
		    qptr->species[3]);
  
  */
  int a1,a2,a3,a4;
  int sp0,sp1,sp2,sp3;
  float weight;

  sp0  =  qptr->species[0];
  sp1  =  qptr->species[1];
  sp2  =  qptr->species[2];
  sp3  =  qptr->species[3];
  weight = qptr->weight;

  if (sp2i[sp0]== -1){
    ERR ("No conversion to %d\n",sp0);
  }
  if (sp2i[sp1]== -1){
    ERR ("No conversion to %d\n",sp1);
  }
  if (sp2i[sp2]== -1){
    ERR ("No conversion to %d\n",sp2);
  }
  if (sp2i[sp3]== -1){
    ERR ("No conversion to %d\n",sp3);
  }
  
  a1 =  sp2i[sp0]; 
  a2 =  sp2i[sp1]; 
  a3 =  sp2i[sp2]; 
  a4 =  sp2i[sp3]; 

  if (a1 == a2 || a1 == a3 || a1 == a4 || a2 == a3 || a2 == a4 || a3 == a4){
    printf ("%d,%d,%d,%d - ERROR ERROR ERROR: two species are the same\n",a1,a2,a3,a4);
    exit(8);
  }

  badEdges[a1*graphSize + a2] += weight;
  badEdges[a2*graphSize + a1] += weight;
  badEdges[a3*graphSize + a4] += weight;
  badEdges[a4*graphSize + a3] += weight;

  goodEdges[a1*graphSize + a4] += weight;
  goodEdges[a4*graphSize + a1] += weight;
  goodEdges[a1*graphSize + a3] += weight;
  goodEdges[a3*graphSize + a1] += weight;
  goodEdges[a2*graphSize + a4] += weight;
  goodEdges[a4*graphSize + a2] += weight;
  goodEdges[a2*graphSize + a3] += weight;
  goodEdges[a3*graphSize + a2] += weight;
  
  /*
  printf("added (%d,%d), (%d,%d), (%d,%d), (%d,%d) good edges\nand (%d,%d),"
	 " (%d,%d) bad edges with weight %g .\n",
	 a1, a4, a1, a3, a2, a4, a2, a3,
	 a1, a2, a3, a4, weight);

  */
}

/*---------------------------------------------------------------------*/

void computeMat(int graphSize, float * goodEdges, float * badEdges, float * adjMat, float alpha){

  int i,j;

  my_printf("\n\nThe resulting graph is:\n");
  for (i=0; i < graphSize; i++){
    for (j=0; j < graphSize; j++){
      if ((goodEdges[i*graphSize + j]) || (goodEdges[j*graphSize + i]) ||
	  (badEdges[i*graphSize + j]) || (badEdges[j*graphSize + i])){
	if ((badEdges[i*graphSize + j]) !=  (badEdges[j*graphSize + i])){
	  printf ("ERROR ERROR ERROR goodEdges %d, %d = %e while  goodEdges %d, %d = %e\n\n",
		  i,j, badEdges[i*graphSize + j], j,i, badEdges[j*graphSize + i]);
	  exit(7);
	} 
	if ((goodEdges[i*graphSize + j]) !=  (goodEdges[j*graphSize + i])){
	  printf ("ERROR ERROR ERROR goodEdges %d, %d = %e while  goodEdges %d, %d = %e\n\n",
		  i,j, goodEdges[i*graphSize + j], j,i, goodEdges[j*graphSize + i]);
	  exit(7);
	} 
	/*
	printf("There are %e good edges and %e bad edges between %d and %d\n",
	       goodEdges[i*graphSize + j] + goodEdges[j*graphSize + i],
	       (badEdges[i*graphSize + j]) + (badEdges[j*graphSize + i]),i,j);
        */
	adjMat[i*graphSize + j] = goodEdges[i*graphSize + j] -
	  alpha * badEdges[i*graphSize + j];
      }
      else {
	adjMat[i*graphSize + j] = 0;
      }
    }
  }

  for (i=0; i < graphSize; i++){
    for (j=0; j < graphSize; j++){
      if (adjMat[i * graphSize +j] != adjMat[j * graphSize +i]){
	printf ("ERROR ERROR: adjMat is not symmetric at %d and %d\n\n",i,j);
	exit(7);
      }
      if (adjMat[i * graphSize +j] != 0){
		my_printf("the edge between %d and %d has weight %e\n",i,j, (adjMat[i * graphSize +j] ));
      }
    }
  }



}
/*---------------------------------------------------------------------
Compute the upper bound for the pareametric search

------------------------------------------------------------------------*/
float computeUpperBound(qrtNode *q_anchor)
{
  qrtNode *currentQrt;
  float minWeight = 10000000;
  float sumWeights = 0;
  float upBound;

  for (currentQrt = q_anchor; currentQrt != NULL; currentQrt = currentQrt -> next)
  {
    if(currentQrt->weight == 0)
      continue;
    sumWeights += currentQrt->weight;
    if (currentQrt->weight < minWeight)
    {
      minWeight = currentQrt->weight;
    }
  }
  
  if(minWeight == 0)
  {
    return 0;
  }

  upBound = (4 * sumWeights) / minWeight;
  my_printf("upper bound sum_weights: %e min_weight %e up_bound %e\n", sumWeights, minWeight,upBound );  
    return upBound;
    
}
/*---------------------------------------------------------------------
  
  This procedure performs the parametric search
  ---------------------------------------------------------------------*/

int * paraSearch(int graphSize, float * goodEdges, float * badEdges, qrtNode *q_anchor, unsigned long qcnt){
  /*
    in this procedure we seek for the optimal ratio between good and bad edges-
    the maximal s.t. is still better from the empty cut.
  */

  int i;
  float * adjMat =  (float *) my_calloc (graphSize*graphSize, sizeof(float) );

  int *part = NULL, *lastPart = NULL;

  int minPart = -1;
  int lastMinPart = -1;
  float alpha =2, lastAlpha = 0;
  int initial_tries; 
  float upBound, lowBound;

  float maxUpBound = computeUpperBound(q_anchor);

   my_printf ("\n\nIN parametric search, graphSize is %d\n", graphSize);

  if(maxUpBound == 0)
  {
    my_printf("All Qrts have 0 weight!!");
    return NULL;
  }

 /*---------------------------------------------------------------------
  
  We start with a ratio 2 that is the ratio of a singleton cut and
  therefore guaranteed to exist. It can also be shown that a non
  singleton cut with ratio at least 2 exist. Therefore, in this
  initial loop, we stay until some non trivial loop is found (we end
  after 100 rounds and report error.
  ---------------------------------------------------------------------*/

  computeMat(graphSize, goodEdges, badEdges, adjMat, alpha); // compute mat only once


  for (initial_tries = 0; (initial_tries < 100 && lastPart == NULL); 
       initial_tries++){
    my_printf("at initial_tries loop, alpha is %4.1f\n",alpha);
    
    part = cutGraph(graphSize, adjMat, &minPart);
    if (part == NULL){
      ERR("Problem in cutGraph in initial cut"
	  " graphSize %d, alpha %e\n",graphSize, alpha );
      return NULL;
    }
    my_printf ("\n\nAfter cut, MinPart is %d \n\n", minPart);
    
    if (minPart < 2){
      my_printf ("Non meaningful cut returned; minPart= %d\n", minPart);
      my_free(part);
    }
    else {
      lastPart = part;  part = NULL;
    }

    lastMinPart = minPart;
    minPart = -1;
    //    firstRatio = ratio;
  } // initial part loop;

   if (lastPart == NULL || part != NULL){
     ERR("No meaningful part was found in initial loop. tries %d, "
	 "lastMinPart %d, alpha %e\n", initial_tries, lastMinPart, alpha);
     return NULL;
   }


 /*---------------------------------------------------------------------
  
   Here we arrive with some initial part exist and we look for un
  upper bound for a cut.
  We end if we arrive at a ratio bigger than 4 * #quartets meaning
  there is a perfect cut - w/o negative edges, or an empty cut,
  meaning no cut satisfies this ratio.
  ---------------------------------------------------------------------*/

 while (1){
     alpha *= 2;
     my_printf("at upper bound loop, alpha is %4.1f\n",alpha);
    computeMat(graphSize, goodEdges, badEdges, adjMat, alpha);

    if (part != NULL){
      EXIT("part is not NULL before cutGraph at UB loop\n");
    }
    part = cutGraph(graphSize, adjMat, &minPart);
    if (part == NULL){
      ERR("Problem in cutGraph in upper bound loop, "
	  " graphSize %d, alpha %e\n",graphSize, alpha );
      return NULL;
    }
    my_printf ("\n\nAfter cut, MinPart is %d \n\n", minPart);
    /*    
	  if (ratio == -1){
	  for (i=0; i < graphSize; i++){
	  printf ("element %d is on side %d\n", i, part[i]);
	  }
	  my_free (lastPart); 
	  return part;
	  }
    */
    if (minPart == 1){
      ERR("A singleton cut should not return at this stage");
      return 0;
    }


    if (minPart == 0){
      my_printf ("Empty  cut was found in UB loop\n");
      my_free(part);
      part = NULL;
      break;
    }
    
      
    if (alpha > maxUpBound){
      my_printf("A non empty cut was found with alpha  %4.1f bigger than 4 * qcnt=%d\n", alpha, 4 * qcnt);
      my_printf ("\nA perfect cut was found\n\n");
      my_free(part);
      break;
    }
    if (minPart < 0){
      ERR("Invalid part returned from maxCutHack: %d\n", minPart);
    }
    lastMinPart = minPart;
    minPart = -1;
    //    firstRatio = ratio;
    my_free (lastPart);
    lastPart = part;  part = NULL;
    lastAlpha = alpha;
    alpha *= 2;
  } // upper bound loop;

  my_printf ("alpha %e\n", alpha);

  my_printf ("\n\nAfter upper bound loop, alpha %4.1f, lastAlpha %4.1f, lastMinPart %d\n",
	  alpha, lastAlpha, lastMinPart);
      

  if (!lastPart){  // we MUST have found some non empty cut in the loop above
    ERR("Must have found some non empty cut in the first loop");
    return NULL;
  }
  

    if (lastMinPart == 1){
       ERR ("A singleton part is the best found at first loop; minPart= %d\n", minPart);
       return NULL;
    }

    if (part != NULL){
      EXIT("part must be null after initial loop,"
	   " alpha %e \n",alpha);
    }
 
  
  // Binary search loop for the biggest alpha s.t. a non empty cut is returnd

  upBound  = alpha; // for this alpha we found empoty cut
  lowBound = alpha /2 - 0.5; // for this one a non empty cut was found
  //  search between the upper and lower bound
  //  float bestRatio = -1;
  while (1){
    if (upBound < lowBound) {
      ERR(" upBound %4.5f < lowBound %4.5f\n\n", upBound, lowBound);
    }
    alpha =  (upBound + lowBound) / 2.0;
    my_printf ("in binary search loop. bounds are %g - %g, alpha %g\n",
	    upBound, lowBound, alpha);
    computeMat(graphSize, goodEdges, badEdges, adjMat, alpha);
    /*-------------------------------------------------------------------
      This is the call to the SDP engine that returns a MaxCut
      partition according to the edges and the the weight parameter
      alpha. 
      -------------------------------------------------------------------*/
    if (part != NULL){
      EXIT("part must be null  before cutGraph at bin search loop,"
	   " alpha %e \n",alpha);
    }
    part = cutGraph(graphSize, adjMat, &minPart);
    if (part == NULL){
      ERR("Problem in cutGraph in binary search loop, "
	  "graphSize=%d, alpha %e\n",graphSize, alpha);
      return NULL;
    }
   //    printf ("\n\nAfter cut, ratio returned %3.2f\n\n",ratio);
    my_printf ("\n\nAfter cut, minPart is %d\n\n",minPart);
 
    if (minPart == 1){
      
      WARN("A singleton part returned in binary search; minPart= %d. "
	   "Might be  avery small graph and many conflicting quartets. graphSize=%d. "
	   "If graph is small (<=6) we use cut obtained in first loop."
	   "Otherwise, we abort. \n", minPart, graphSize);
      if (graphSize <=6){
	if (lastPart == NULL){
	  ERR("No lastPart was found at binary search when a singleton"
	      "cut returned\n");
	  return NULL;
	}
	else{
	  break;
	}
      }
      else{
	ERR("A  singleton part returned in binary search for a big"
	    "graph, size %d.\n",graphSize);
	return NULL;
      }
      
    }
   
    if (minPart == 0){
      my_printf ("no cut was found for alpha %4.1f\n",alpha);
      upBound = alpha;
      my_free(part);
      part = NULL;
    }
    else{
      if (minPart < 2){
	ERR("Something strange here. We must have had minPart >=2 but got %d\n",
	    minPart);
	return NULL;
      }
      lowBound = alpha;
      my_free (lastPart);
      lastPart = part;
      part = NULL;
    }
    if ((upBound - lowBound <= 1) || 
	(lowBound > 0 && ((upBound - lowBound)/lowBound) < 0.00001))
	{
      my_printf("reached a threshold. upper %3.2f, lower %3.2f. Exiting\n",upBound, lowBound);
      my_printf("\nSide 0 of returned part\n");
      //      int sum=0;
      for (i=0; i < graphSize; i++){
	if (!lastPart[i]){
	  my_printf("%d, ",i);
	}
      }
      my_printf("\n\nNow side 1 of returned part\n");
      for (i=0; i < graphSize; i++){
	if (lastPart[i]){
	  my_printf("%d, ",i);
	}
      }
      my_printf("\n\n");
      break;
    }
  }

  if (part != NULL){
    my_free (part);
  }
  my_free (adjMat); 

  if (lastPart == NULL){
    ERR(" lastPart is NULL  at end of  binary search \n");
    return NULL;
  }
  return lastPart;
  
}


/*---------------------------------------------------------------------
  this procedure adds uncovered elements to the root of a tree

---------------------------------------------------------------------*/

void addUncovered(treeNode *root,               // the root of the tree
		  eleNode *uncovered,          // the list of uncovered elements
		  int *pCnt)
{
  eleNode *current = uncovered;
  
  my_printf ("In addUncovered, adding to root %d\n", root->id);
  
  while (current){
   
    treeNode * leaf = newNode (NULL,(*pCnt)++);
    
	my_printf ("Now adding element %d\n",current -> species);
    
	leaf -> val = current -> species;
    addSon(root, leaf);
    uncovered = current;
    current = current -> next;
    my_free(uncovered);
  }
  
}


/*---------------------------------------------------------------------
  this procedure modifies all the taxa from the "other" part to the
  outgroup taxon. It also chains all the elements in the part that are
  uncovered by any quartet in a separate chain to be added to the
  returned tree resulted from the quartets.

---------------------------------------------------------------------*/
void add_outgroup(
	       qrtNode *q_anchor,
	       eleNode **e_anchor,
	       int *p_ncovered,
	       eleNode **pUncovered,
	       int *part,
	       int side,
	       int outg,
	       int *sp2i,
	       int maxSp){
  
  
  
  int  *covered = (int *) my_calloc((maxSp+4),sizeof(int));
  qrtNode *currentq;

  int i, j;
  int out = 0;
  eleNode *currente;

  my_printf ("\nIn add_outgroup for side %d. Outg is %d\n\n",side, outg);
  if (side != 0 && side != 1){
    ERR("Side must be either 0 or 1 but found %d\n",side);
  }

  for (i=0,  currentq = q_anchor; currentq != NULL; currentq = currentq -> next, i++){
    out=0;
    my_printf ("\nquartet %d: %d,%d|%d,%d converted to %d,%d|%d,%d\n", i,
	    currentq->species[0], currentq->species[1], currentq->species[2], currentq->species[3],
	    sp2i[currentq->species[0]], sp2i[currentq->species[1]], 
	    sp2i[currentq->species[2]], sp2i[currentq->species[3]]);
  
  
    for (j=0; j < 4; j++){
      if (part[sp2i[currentq->species[j]]] != side) {
	/*
	printf("Species %d is on the other side. transforming to outgroup %d\n",
	       currentq->species[j], outg);
	*/
	out++;
	if (out > 1){
	  ERR("Two outs in quartet  %d: %d,%d|%d,%d converted to %d,%d|%d,%d\n", i,
	    currentq->species[0], currentq->species[1], currentq->species[2], currentq->species[3],
	    sp2i[currentq->species[0]], sp2i[currentq->species[1]], 
	    sp2i[currentq->species[2]], sp2i[currentq->species[3]]);
	}
	currentq->species[j] = outg;
      }
      else{
	//	printf ("taxon %d is in the right side\n",currentq->species[j]);
      }
      covered[currentq->species[j]]++; // We also count the outg as covered

    } // for j...


    if (out){
      my_printf ("quartet %d was changed to: %d,%d|%d,%d \n",i,
	      currentq->species[0], currentq->species[1], currentq->species[2], currentq->species[3]);
    }
    else{
      my_printf ("No change quartet %d:  %d,%d|%d,%d \n", i,
	      currentq->species[0], currentq->species[1], currentq->species[2], currentq->species[3]);
    }


  } //  for ( current = q_anchor...
 
  my_printf("\nChecking for uncovered taxa\n");
  for (currente  = *e_anchor; currente != NULL; currente = currente->next){
    if (part[sp2i[currente->species]] != side){
      if (currente->species != outg){ // Outg must be in the other part
	ERR("Found that species %d has part %d that differs from side %d\n",
	    currente->species, part[sp2i[currente->species]], side);
      }
      else{
	my_printf("Outg %d must be in the other side of all %d\n",currente->species, side);
      }
    }

    my_printf("Checking for element %d converted to %d.\n", 
	   currente->species, sp2i[currente->species]);
    if (covered[currente->species]){
      my_printf("elemen %d is covered %d times by quartets\n",
	     currente->species, covered[currente->species]);
      (*p_ncovered)++;
      *e_anchor = currente;
      e_anchor = &(currente ->next);
    }
    else{
      my_printf("Element %d is not covered. Need to be removed from list\n",currente->species);
      *pUncovered = currente;
      pUncovered = &(currente ->next);
      //      exit(8);
    }
  }
  *pUncovered =  *e_anchor = NULL;


  my_free(covered);
}

/*---------------------------------------------------------------------
  this procedure gets a list of quartets and a cut (partition) over the set of species.
  It splits the quartets into two sets according to the cut where a
  quartet is in some part if it has at least 3 members in that part. A
  quartet which has no 3 members in a cut is either satisfied or
  violated and does not proceed to further steps and its memory is freed. 

---------------------------------------------------------------------*/
void splitQuartets(qrtNode * q_anchor, 
		   int *final_part,
		   unsigned long qcnt,   
		   int *i2sp, 
		   int *sp2i,
		   qrtNode **current0,
		   unsigned long *qcnt0,
		   qrtNode **current1,
		   unsigned long *qcnt1)
{
  int i=0;
  qrtNode * current;
  //  qrtNode *q_anchor0 = *current0, *q_anchor1 = *current1;

  my_printf("\n\nAt splitQuartets\n");

  for (i=0,  current = q_anchor; current != NULL; i++){
    qrtNode *next = current -> next;
    /*
    printf ("quartet %d: %d,%d|%d,%d converted to %d,%d|%d,%d\n", i,
	    current->species[0], current->species[1], current->species[2], current->species[3],
	    sp2i[current->species[0]], sp2i[current->species[1]], 
	    sp2i[current->species[2]], sp2i[current->species[3]]);
    */
    int cnt[2] = {0,0};
    int j=0;
    for (j=0; j < 4; j++){
      if (final_part[sp2i[current->species[j]]] > 1 || final_part[sp2i[current->species[j]]] < 0){
	ERR("Invalid part %d for species %d at quartet  %d,%d|%d,%d\n",
	    final_part[sp2i[current->species[j]]], current->species[j], 
	    current->species[0], current->species[1], current->species[2], current->species[3]);
      }
      cnt[final_part[sp2i[current->species[j]]]]++; // count how many species fall in each part
    } // for j...

    if (cnt[0] + cnt[1] != 4){
      ERR ("cnt[0]:%d + cnt[1]:%d != 4\n", cnt[0] , cnt[1]);
    }
    if (cnt[0] >2){
      /*
      printf ("quartet  %d,%d|%d,%d is in part 0 with %d members\n",
	      current->species[0], current->species[1],
	      current->species[2], current->species[3], cnt[0]);
      */
      (*qcnt0)++;
      *current0 = current;
      current0 = &(current ->next);
    }
    else if (cnt[1] >2){
      /*
      printf ("quartet  %d,%d|%d,%d is in part 1 with %d members\n",
	      current->species[0], current->species[1],
	      current->species[2], current->species[3], cnt[1]);
      */
      (*qcnt1)++;
      *current1 = current;
      current1 = &(current ->next);
    }
    else {
      /*
      printf ("quartet  %d,%d|%d,%d is not in ANY part with %d members in 0 and  %d members in 1\n",
	      current->species[0], current->species[1], 
	      current->species[2], current->species[3], cnt[0], cnt[1]);
      */
      free (current);
    }
    
    current = next;
  } //  current = q_anchor; 
  *current0 =  *current1 = NULL;


}

/*------------------------------------------------------------------------------

This procedure is called after a cut is found. It first splits the
elements chain into two chains according to their part in the split.
Then it splits the quartets to the parts.
--------------------------------------------------------------------------------*/

treeNode *splitElements(eleNode * e_anchor, 
			int ecnt, 
			qrtNode *q_anchor, 
			unsigned long qcnt, 
			int *part, 
			int *i2sp, 
			int *sp2i,
			int maxSp,
			int *pCnt){
  
  eleNode *e_anchor0 = NULL;
  eleNode *e_anchor1 = NULL;
  eleNode **current0 = &e_anchor0;
  eleNode **current1 = &e_anchor1;
  eleNode *current;

  treeNode *root0;
  treeNode *root1;
  treeNode * new_root;

  int outg1 = -1, outg0 = -1;
  unsigned long qcnt0 = 0, qcnt1 = 0;

  qrtNode *q_anchor0 = NULL, *q_anchor1 = NULL;
  int i=0, cnt0=0, cnt1=0;

  my_printf("\n\n\nAt splitElements, \n\n");

  my_printf("\n ecnt %d \nSide 0 of returned part\n", ecnt);
  // int sum=0;
      for (i=0; i < ecnt; i++){
	if (!part[i]){
	  my_printf("%d, ",i);
	}
      }
      my_printf("\n\nNow side 1 of returned part\n");
      for (i=0; i < ecnt; i++){
	if (part[i]){
	  my_printf("%d, ",i);
	}
      }
      my_printf("\n\n");


  for (i = 0, current = e_anchor; current != NULL; current = current -> next, i++){
    if (part[sp2i[current->species]] > 1 ||  part[sp2i[current->species]] < 0){
      ERR("Invalid part %d for species %d\n",part[sp2i[current->species]], current->species);
    }
    if (part[sp2i[current->species]] == 0){
      //      printf ("Species %d is in part 0: %d\n", current->species, part[sp2i[current->species]]);
      *current0 = current;
      current0 = &(current ->next);
      cnt0++;
      outg1 = current->species;
    }
    else {
      //      printf ("Species %d is in part 1: %d\n", current->species, part[sp2i[current->species]]);
      *current1 = current;
      current1 = &(current ->next);
      cnt1++;
      outg0 = current->species;
    }
  } //  for current =  e_anchor
  *current1 = *current0 = NULL;
  
  if (outg0 < 0 || outg1 < 0){
    ERR("One of the outg's %d and %d is unset\n",outg0, outg1);
  }

  if (cnt0 + cnt1 != ecnt){
    ERR("number of ele in part 0: %d plus no. of ele in part 1: %d, differs from total number of ele %d\n",
	cnt0, cnt1, ecnt);
  }

  /*
  printf ("\n\n%d elements in component 0:\n",cnt0);
  for (i=0,  current = e_anchor0; current != NULL; current = current -> next, i++){
    printf ("%d, ", current->species);
  }
  */
  if (cnt0 < 2){
    ERR("Only one element in part 0");
  }
 
  my_printf("Now adding the outg0 %d  to the elements list\n",outg0);
 addElement2list(&e_anchor0, outg0);
 
 /*
   printf ("\n\n%d elements in component 1:\n", cnt1);
   for (i=0,  current = e_anchor1; current != NULL; current = current -> next, i++){
   printf ("%d, ", current->species);
   }
 */
  if (cnt1 < 2){
    ERR("Only one element in part 1");
  }
  
 
  my_printf("Now adding the outg1 %d  to the elements list\n",outg1);
  addElement2list(&e_anchor1, outg1);
  

  /*
    Now we split the quartets into quartets that cuntinue recurse
    (unaffected or deffered) with each part and quartets that are
    split 2|2 (satisfied or 
  */
  
  splitQuartets(q_anchor, part,  qcnt, i2sp, sp2i, &q_anchor0, &qcnt0, &q_anchor1, &qcnt1);
  
  //  qrtNode *currentq;
  my_printf ("After split quartets, back at split elements\n");

  /*
  printf ("\nThere are %d quartets in component 0:\n", qcnt0);
  for (i=0,currentq = q_anchor0; currentq != NULL; currentq = currentq -> next, i++){
      printf ("quartet  %d,%d|%d,%d is in part 0 \n",
	      currentq->species[0], currentq->species[1],
	      currentq->species[2], currentq->species[3]);
      
  }
  printf ("total number %d\n", i);


  printf ("\nThere are %d quartets in component 1:\n", qcnt1);
  for (i=0,currentq = q_anchor1; currentq != NULL; currentq = currentq -> next, i++){
      printf ("quartet  %d,%d|%d,%d is in part 1 \n",
	      currentq->species[0], currentq->species[1],
	      currentq->species[2], currentq->species[3]);
      
  }
  printf ("total number %d\n", i);
  */


  if (!qcnt0){
    my_printf("No quartet fits to part 0. Returning a tree over the taxa\n");
    root0 =  make_star_tree(e_anchor0, cnt0, maxSp, pCnt);
  }
  else {
   
    eleNode *uncovered0 = NULL;
    int ncovered0 = 0;
    eleNode *currente;

	my_printf("Number of quartets fit to part 0 is %d\n",qcnt0);
	/*
      here we modify all the deferred quartets in part 0 to contain the outgroup
      (taken as some species in the other part). We also devide the
      element list to elements that are covered by some quartets and
      those who are not.
    */
    add_outgroup(q_anchor0, &e_anchor0, &ncovered0, &uncovered0, part, 0, outg0, sp2i, maxSp);
    my_printf("\nAfter add outgroup. Number of covered elementssssssssss is %d\n",ncovered0);
    
    
   
 
    my_printf ("\nUncovered elements in component 0:\n");
    for (i=0, currente = uncovered0; currente != NULL; currente = currente -> next, i++){
      my_printf ("Element  %d is uncovered in component 0 \n", currente->species);
      
    }
    my_printf ("total number %d\n", i);
    my_printf ("\n Covered elements in component 0:\n");
    for (i=0, currente = e_anchor0; currente != NULL; currente = currente -> next, i++){
      my_printf ("Element  %d is covered in component 0 \n", currente->species);
      
    }
    my_printf ("total number %d\n", i);


    my_printf("\nBefore find_cut. #ele is %d\n",ncovered0);

    root0 = find_cut(e_anchor0, ncovered0, q_anchor0, qcnt0, maxSp, pCnt);
    my_printf("\nAfter find_cut. Now adding uncovered\n");
    
    if (uncovered0 != NULL)
      {
	addUncovered(root0, uncovered0, pCnt);
      }
    else{
      my_printf("no need to add uncovered0\n");
    }
  }

  /*  
      printf("\n\nFinal tree 0:\n");
      print_tree(root0,0);
  */
 

  if (!qcnt1){
    my_printf("No quartet fits to part 1. Returning a tree over the taxa\n");
    root1 =  make_star_tree(e_anchor1, cnt1, maxSp, pCnt);
  }
  else {
    eleNode *uncovered1 = NULL;
    eleNode *currente;

	int ncovered1 = 0;
    /*
      here we modify all the deferred quartets in part 1 to contain the outgroup
      (taken as some species in the other part). We also devide the
      element list to elements that are covered by some quartets and
      those who are not.
    */
    add_outgroup(q_anchor1, &e_anchor1, &ncovered1, &uncovered1, part, 1, outg1, sp2i, maxSp);
    my_printf("\nAfter add outgroup. Number of covered elements is %d\n",ncovered1);
    
    my_printf ("\nUncovered elements in component 1:\n");
    for (i=0, currente = uncovered1; currente != NULL; currente = currente -> next, i++){
      my_printf ("Element  %d is uncovered in component 1 \n", currente->species);
      
    }
    my_printf ("total number %d\n", i);
    my_printf ("\n Covered elements in component 1:\n");
    for (i=0, currente = e_anchor1; currente != NULL; currente = currente -> next, i++){
      my_printf ("Element  %d is covered in component 1 \n", currente->species);
      
    }
    my_printf ("total number %d\n", i);
 

    my_printf("\nBefore find_cut. #ele is %d\n",ncovered1);

    root1 = find_cut(e_anchor1, ncovered1, q_anchor1, qcnt1, maxSp, pCnt);
    my_printf("\nAfter find_cut for part 1. Now adding uncovered\n");
    

    if (uncovered1 != NULL)
      {
	addUncovered(root1, uncovered1, pCnt);
      }
    else{
      my_printf("no need to add uncovered1\n");
    }

  }


  
  my_free(part);

  /*
  printf("\n\nBefore join_trees for tree:\n");
  print_tree(root0,0);
  printf("  with outg %d,\n\nand tree:\n",outg0);
  print_tree(root1,0);
  printf("  with outg %d\n\n\n",outg1);
  */
  
  new_root = join_trees(root0, outg0, root1, outg1, maxSp, pCnt);
  
  /*
  printf("\n\nAfter join_trees. Tree is:\n");
  print_tree(new_root,0);
  */  
  
  //  exit(7);
  
  return new_root;
  
}

/*---------------------------------------------------------------------*/


treeNode *find_cut(
		   eleNode * e_anchor,
		   int ecnt, 
		   qrtNode * q_anchor, 
		   unsigned long qcnt, 
		   int maxSp, 
		   int *pCnt){
  int  *i2sp = (int *) my_calloc((ecnt+4),sizeof(int));
  int  *sp2i = (int *) my_calloc((maxSp+4),sizeof(int));
 
  //  int m=0; // number of edges in adjMat
  int graphSize = ecnt;
  float * goodEdges = NULL;
  float * badEdges =  NULL;
  unsigned long i;
  eleNode *e_tmp;
  qrtNode *current;
  int *final_part = NULL;
  treeNode *retTree;
  time_t t1;		

  my_printf("\n\nAt find_cut for %d  elements\n",ecnt);
  
  if (!qcnt){
    ERR("No quartets fed to find_cut\n");
  }
  
  for (i=0; i < maxSp+4; i++){
    sp2i[i] = -1;
  }
  goodEdges =  (float *) my_calloc (graphSize*graphSize, sizeof(float) );
  badEdges =   (float *) my_calloc (graphSize*graphSize, sizeof(float) );
 
  //  printf ("\n\nIn Maxcut. Elements are:\n\n");


  for (i=0,  e_tmp = e_anchor; e_tmp != NULL; e_tmp = e_tmp -> next, i++){
    i2sp[i] = e_tmp -> species;
    sp2i[e_tmp -> species] = i;
        my_printf ("ele %d, converted to %d\n", e_tmp -> species, i);
  }
  
  
  for (i=0; i < maxSp+4; i++){
    my_printf("sp %d is converted to %d\n", i , sp2i[i]);
  }
 

  my_printf("finished with all quartet. Before para search\n");
  

  for (i=0,  current = q_anchor; current != NULL; i++, current = current -> next){

    if ((sp2i[current->species[0]] < 0) ||
	(sp2i[current->species[1]] < 0) ||
	(sp2i[current->species[2]] < 0) ||
	(sp2i[current->species[3]] < 0) ){
      printf ("quartet %ld: %d,%d|%d,%d converted to %d,%d|%d,%d\n", i,
	      current->species[0], current->species[1], current->species[2], current->species[3],
	      sp2i[current->species[0]], sp2i[current->species[1]], 
	      sp2i[current->species[2]], sp2i[current->species[3]]);
      ERR("Uncovered element in a quartet\n");
    }
	handleQuartet(current,sp2i, graphSize, goodEdges, badEdges);
  }
  my_printf("finished with all quartet. Before para search\n");
  
  if (i != qcnt){
    printf ("ERROR ERROR ERROR: number of quartets %ld is not what I saw %ld in findCut\n\n",
	    qcnt, i);
    exit (7);
  } 
 


  // find the optimal cut

  final_part = paraSearch( graphSize,  goodEdges,  badEdges, q_anchor, qcnt);
  if (!final_part){
    FILE *ferr;
    ferr = fopen ("wMXCN.log","w");		
 	
    t1 = time(NULL);		
    //    local = localtime(&t1);				      
    fprintf(ferr,"%s-----------------------\n", asctime( localtime(&t1)));   
    fprintf(ferr,"ERROR ERROR ERROR: ");    
    fprintf (ferr,"An invalid part was returned for graphSize %d and #qrt %ld\n", graphSize, qcnt);
    
    for (i=0,  current = q_anchor; current != NULL; i++, current = current -> next){
      fprintf (ferr,"quartet %ld: %d,%d|%d,%d converted to %d,%d|%d,%d\n", i,
	      current->species[0], current->species[1], current->species[2], current->species[3],
	      sp2i[current->species[0]], sp2i[current->species[1]], 
	      sp2i[current->species[2]], sp2i[current->species[3]]);
    }
    
    fclose(ferr);				\
    exit(8);
  }
  free (goodEdges); goodEdges = NULL;
  free (badEdges); badEdges = NULL;

  /*
  for (i=0; i < maxSp+1; i++){
    if (sp2i[i] >= 0 ){
      printf ("taxe %d elem %d is on side %d\n", i, sp2i[i], final_part[sp2i[i]]);
    }
  }
  */
  my_printf("\nBack from para search, graphSize %d\nSide 0 of returned part\n", graphSize);
  //      int sum=0;
      for (i=0; i < graphSize; i++){
	if (!final_part[i]){
	  my_printf("%d, ",i);
	}
      }
      my_printf("\n\nNow side 1 of returned part\n");
      for (i=0; i < graphSize; i++){
	if (final_part[i]){
	  my_printf("%d, ",i);
	}
      }
      my_printf("\n\n");

  
  retTree =  
  splitElements(e_anchor,  ecnt, q_anchor,  qcnt, final_part, i2sp, sp2i, maxSp, pCnt);
  
  
  my_printf("\n\nBack at find_cut after splitElements. Tree is:    \n");

  // print_tree(retTree,0);
 
  if (ecnt >= 100){
    printf("Returning a tree of size %d\n",ecnt);
  }
  return retTree;
  
  exit(8);

}
 /*---------------------------------------------------------------------*/

void find_constant_ratio_cut(eleNode * e_anchor, int ecnt, qrtNode * q_anchor, unsigned long qcnt, int maxSp, float alpha){
  
  int  *i2sp = (int *) my_calloc((ecnt+4),sizeof(int));
  int  *sp2i = (int *) my_calloc((maxSp+4),sizeof(int));
  
  int m=0; // number of edges in adjMat
  int graphSize = ecnt;
  float * goodEdges =  (float *) my_calloc (graphSize*graphSize, sizeof(float) );
  float * badEdges =  (float *) my_calloc (graphSize*graphSize, sizeof(float) );
  float * adjMat =  (float *) my_calloc (graphSize*graphSize, sizeof(float) );
  qrtNode *current;
 
  int i,j;
  eleNode *e_tmp;

  for (i=0; i < ecnt+4; i++){
    i2sp[i] = -1;
  }
  
  for (i=0; i < maxSp+4; i++){
    sp2i[i] = -1;
  }
  
  printf ("\n\nIn Maxcut. Elements are:\n\n");

  for (i=0,  e_tmp = e_anchor; e_tmp != NULL; e_tmp = e_tmp -> next, i++){
    i2sp[i] = e_tmp -> species;
    sp2i[e_tmp -> species] = i;
    printf ("ele %d, converted to %d\n", e_tmp -> species, i);
  }
  
  for (i=0; i < maxSp+4; i++){
    printf("sp %d is converted to %d\n", i , sp2i[i]);
  }


  for (i=0,  current = q_anchor; current != NULL; current = current -> next){
    my_printf ("quartet %d: %d,%d|%d,%d converted to %d,%d|%d,%d\n",i++,
	    current->species[0], current->species[1], current->species[2], current->species[3],
	    sp2i[current->species[0]], sp2i[current->species[1]], 
	    sp2i[current->species[2]], sp2i[current->species[3]]);
	handleQuartet(current,sp2i, graphSize, goodEdges, badEdges);
  }
  

  computeMat(graphSize, goodEdges, badEdges, adjMat, alpha);

  m=0;
  for (i=0; i < graphSize; i++){
    for (j=0; j < graphSize; j++){
      if (adjMat[i * graphSize +j] != adjMat[j * graphSize +i]){
	printf ("ERROR ERROR: adjMat is not symmetric at %d and %d\n\n",i,j);
	exit(7);
      }
      if (adjMat[i * graphSize +j] != 0){
	m++;
	printf("the edge between %d and %d has weight %e\n",i,j, (adjMat[i * graphSize +j] ));
      }
    }
  }


  //  part = cutGraph(graphSize, adjMat, &ratio);
 
  
  free (goodEdges); goodEdges = NULL;
  free (badEdges); badEdges = NULL;
  free (adjMat); adjMat = NULL;

}


/*---------------------------------------------------------------------*/


int  ckQuartets(qrtNode * q_anchor, unsigned long qcnt){
  unsigned long i;
  qrtNode *q_tmp;
  for (i=0,  q_tmp = q_anchor; q_tmp != NULL; q_tmp = q_tmp -> next, i++){
  }
  
  if (i != qcnt){
    printf("Unequal number of quartets %ld and qrt cnt %ld\n", i, qcnt);
    exit(8);
  }
  return 1;
}


/*---------------------------------------------------------------------*/

int  ckElements(eleNode * e_anchor, int ecnt){
  int i;
  eleNode *e_tmp;
  for (i=0,  e_tmp = e_anchor; e_tmp != NULL; e_tmp = e_tmp -> next, i++){
  }
  
  if (i != ecnt){
    printf ("Unequal number of elements %d and ele cnt %d\n", i, ecnt);
    exit(8);
  }
  return 1;
}



