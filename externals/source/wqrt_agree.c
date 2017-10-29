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


enum modelWithWeightsType {OFF,ON,SRC,REF};

/*---------------------------------------------------------------------*/

int getCharArg(char *argv[], const char *argName, char **argVal, int argNum)
{
  size_t strSize = strlen(argName);
  if (! strncmp(argv[argNum],argName, strSize)){
    my_printf("found %s%s\n", argName, argv[argNum]+strSize);
    *argVal = argv[argNum]+strSize;
    return (1);
  }
  return (0);
}

/*---------------------------------------------------------------------*/

void qrtSort(qrtType &qrt,qrtWeightType &qrtWeight, float weight)
{

  int i,j;
  qrtType origQrt;
  for(i=0;i<4;i++)
    origQrt.n[i] = qrt.n[i];

  for(i=0;i< 4;i++)
    for(j=0;j<3 ;j++)
      {
	if(qrt.n[j] > qrt.n[j+1])
	  {
	    int tmp = qrt.n[j];
	    qrt.n[j] = qrt.n[j+1];
	    qrt.n[j+1] = tmp;
	  }
      }

  if(origQrt.n[0] > origQrt.n[1])
    {
      int tmp = origQrt.n[0];
      origQrt.n[0] = origQrt.n[1];
      origQrt.n[1] = tmp;
    }

  if(origQrt.n[0] == qrt.n[0]) 
    {
      if (origQrt.n[1] == qrt.n[1])	    
	qrtWeight.weight[0] = weight; 
      else
	if (origQrt.n[1] == qrt.n[2])	    
	  qrtWeight.weight[1] = weight;
	else
	  qrtWeight.weight[2] = weight;
    }
  else
    if(origQrt.n[0] == qrt.n[1]) 
      {
	if (origQrt.n[1] == qrt.n[2])	    
	  qrtWeight.weight[2] = weight; 
	else
	  qrtWeight.weight[1] = weight;
      }

    else
      qrtWeight.weight[0] = weight;
    
}
/*---------------------------------------------------------------------*/



void printUsage()
{
    printf("\n\nUsage: genQuartetsFromMat "  
	   "\n ref_tree=<tree file name> The reference tree name" 
	   "\n src_tree=<tree file name> The source tree name"
	   "\n qrt_file=<qrt file name>"
	   "\n tree_list=<tree list file name>"
	   "\n out=<out file name>"
	   "\n taxa_file=<taxa file name>"	   
	   "\n qnum_factor=<factor of quartets related to ntaxa>"
	   "\n ntaxa=<size of taxa>"
	   "\n correct=<% of correct qrts>"
	   "\n min_ntaxa=<minimum size of taxa>"
	   "\n select=<all/random> qrt selection"
	   "\n weights=<src/ref/all> compute qrt with weights"
	   "\n");
}

/*---------------------------------------------------------------------*/
  
void parseParms(int argc, char *argv[], char **refTreeFname,  char **srcTreeFname, 
		char **treeListFname, char **taxaFname, char **qrtFname,  char **outFname,
		float *qnumFactor, int *ntaxa, int *correct, int *minTaxa, modelWithWeightsType *modelWithWeights, 
		char **select)
{
  int i;

  my_printf ("at parse parm. argc %d\n",argc);
  char *str_ntaxa;
  char *str_correct;
  char *str_minTaxa;
  char *str_qnumFactor;
  char *str_weights;
 

  for (i=1; i< argc; i++){
    my_printf ("parm %d is %s\n",i,argv[i]);

    getCharArg(argv,"ref_tree=", refTreeFname, i);
    getCharArg(argv,"src_tree=", srcTreeFname, i);
    getCharArg(argv,"taxa=", taxaFname, i);
    getCharArg(argv,"qrt_file=", qrtFname, i);
    getCharArg(argv,"tree_list=", treeListFname, i);
    getCharArg(argv,"out=", outFname, i);
    getCharArg(argv,"select=", select, i);
    if(getCharArg(argv,"qnum_factor=", &str_qnumFactor, i))
       *qnumFactor = atof(str_qnumFactor);
    if(getCharArg(argv,"min_taxa=", &str_minTaxa, i))
       *minTaxa = atoi(str_minTaxa);
    if(getCharArg(argv,"ntaxa=", &str_ntaxa, i))
      {
	*ntaxa = atoi(str_ntaxa);
	printf("ntaxa %s is %d\n",str_ntaxa,*ntaxa);
      }
    if(getCharArg(argv,"correct=", &str_correct, i))
      {
	*correct = atoi(str_correct);
	printf("correct %s is %d\n",str_correct,*correct);
      }
    if(getCharArg(argv,"weights=on", &str_weights, i))
      *modelWithWeights = ON;
    if(getCharArg(argv,"weights=src", &str_weights, i))
      *modelWithWeights = SRC;
    if(getCharArg(argv,"weights=ref", &str_weights, i))
      *modelWithWeights = REF;
  }
  

  if (((*refTreeFname == NULL) && (*treeListFname == NULL)) && (*qrtFname == NULL)){
    printf("ERROR !!!!!  missing required arg: ref_tree_file or tree_list_file!");
    printUsage();
  }
    else 
      if ((*ntaxa) == -1){
      printf("ERROR !!!!!  missing required arg: ntaxa!");
      printUsage();
      exit(2);
    }
}



/*---------------------------------------------------------------------*/
/* Read neames from a file, where every name is in a seperate line
     and return an array of names 
 					   */

void readNames(char *fname,strArrayType &namesArray ,int &num_of_names)
{
   FILE *namesFile = fopen(fname, "r");

  long file_size;
  char *buffer;

  if (namesFile == NULL) {
    printf("file %s does not exist\n\n", fname);
    exit(8);
  }
  
  if (fseek(namesFile, 0 , SEEK_END) != 0) {
    /* Handle Error */
}
  file_size = ftell(namesFile);
  if (file_size == -1) {
    /* Handle Error */
  }
  printf ("size for file %s: %ld\n", fname, file_size);
  rewind(namesFile);

  buffer = (char*)malloc(file_size);
  if (buffer == NULL) {
    printf("Malloc for buffer failed\n");
    exit(8);
    /* Handle Error */
  }
    
  char *ptr;
  
  
  num_of_names=0;

  while (fgets(buffer,file_size, namesFile) != NULL) {
    ptr = strtok(buffer," ,\t,\n");
    if(!ptr)
      break;
	namesArray.push_back(ptr);
	num_of_names++;
	//     	ptr = strtok(NULL," ");
  }

  fclose (namesFile);

}

/* 
Compute qrt agreement score 
 Input:  Qrt tree  weights - qrtTree1Weight, qrtTreeWeight2 
 output - tree1Weight, tree2Weight - The weights of the given quartets
*/
/*---------------------------------------------------------------------*/
float weightedQrtAgree(qrtWeightType &qrtSrcWeight,qrtWeightType &qrtRefWeight,
		       float &srcWeight,float &refWeight,float &qrtAgree, modelWithWeightsType modelWithWeights)
{
  //  float srcWeights[3] =  {qrtSrcWeight.q0123,qrtSrcWeight.q0213,qrtSrcWeight.q0312};
  // float refWeights[3] =  {qrtRefWeight.q0123,qrtRefWeight.q0213,qrtRefWeight.q0312};

  float qrtAgreeWeight = 0;
  qrtAgree = 1;
  int i,j;
  for(i=0;i<3;i++)
    for(j=0;j<3;j++)
      {
	srcWeight = qrtSrcWeight.weight[i];
	refWeight = qrtRefWeight.weight[j];
	if((qrtSrcWeight.weight[i]>0)&& (qrtRefWeight.weight[j]>0))
	  {
	    if(modelWithWeights==ON)
	      qrtAgreeWeight = srcWeight * refWeight;
	    else
	      if(modelWithWeights==SRC)
		qrtAgreeWeight = srcWeight;
	      else
		if(modelWithWeights==REF)
		  qrtAgreeWeight = refWeight;
	    if (i != j) 
	      {
		qrtAgree = -1;
	      }	      
	    return(qrtAgreeWeight);
	  }
      }
  qrtAgree = -1;
  if(modelWithWeights==SRC)
    return(srcWeight);
  if(modelWithWeights==REF)
    return(refWeight);
  return(0);
}

/*---------------------------------------------------------------------*/
/* Compute qrt agreement score for distance matrix */
  
float  matWeightedQrtAgree(float *distMat, int *spcArray, qrtArrayType &qrtWeights,
			   int ntaxa, modelWithWeightsType modelWithWeights,
			   float &qrtAgreeScore,float &unresolvedScore)
{
  
  //  struct qrtStruct * prev, * current, *q_anchor ;
  
  printf ("at matWeightedQrtAgree\n");

  float maxDist = getMaxDist(distMat, ntaxa);
  int i,t0,t1,t2,t3;
  qrtNode qNode;
  qrtArrayType dummyQrtWeights;
  qrtWeightType qrtNodeRefTreeWeight;
  qrtWeightType qrtNodeSrcTreeWeight;
  qrtType refQrt;

  float qrtAgreeWeight = 0;
  float refTreeWeight = 0;
  float srcTreeWeight = 0;
  float qrtAgree;

  float totalwQrtAgree = 0;
  float totalSqrRefWeight = 0;
  float totalSqrSrcWeight = 0;
  float totalQrtAgree = 0;
  float totalAbsWeight = 0;
  unsigned long qrtNum = 0;

  std::map<int, int> spcArrayIndex;

  printf("Start qrt agree\n"); 
  
  qrtType currentQrt;

  // Init spc index

  for(i=0;i<ntaxa;i++)
    {
      spcArrayIndex[spcArray[i]]=i;
    }
  
  unsigned long unresolved = 0;

  i = 0;
  
  qrtType dummyQrt;
  for (qrtArrayIter qrtIter = qrtWeights.begin(); qrtIter != qrtWeights.end();qrtIter++)
    {
      	 
      refQrt = (*qrtIter).first;
      qrtNodeRefTreeWeight = (*qrtIter).second;
      
      t0 = spcArrayIndex[refQrt.n[0]];
      t1 = spcArrayIndex[refQrt.n[1]];
      t2 = spcArrayIndex[refQrt.n[2]];
      t3 = spcArrayIndex[refQrt.n[3]];

      addNewQuartet(distMat, spcArray, dummyQrtWeights, ntaxa,
		    maxDist, t0, t1, t2, t3, false, dummyQrt,qrtNodeSrcTreeWeight);
      
      //           printQrt("get src qrt: ",refQrt);
      //      printWeight("src ",qrtNodeSrcTreeWeight);
      //      printWeight("ref ",qrtNodeRefTreeWeight);
      
      int qrtAgreeExtraWeight =  (qrtAgreeWeight > 0) ? 2 : 1;
      
      qrtAgreeWeight = qrtAgreeExtraWeight * weightedQrtAgree(qrtNodeSrcTreeWeight,qrtNodeRefTreeWeight,
					srcTreeWeight,refTreeWeight,qrtAgree,modelWithWeights);
      // printf("qrt agree %g\n",qrtAgree);

      
      totalwQrtAgree += qrtAgree*qrtAgreeWeight;
      totalQrtAgree += qrtAgree;
      
      if(refTreeWeight == 0 || srcTreeWeight == 0)
	unresolved ++;

      qrtNum++;
      
      if(modelWithWeights == ON)
	{
	  totalSqrRefWeight += refTreeWeight * refTreeWeight;
	  totalSqrSrcWeight += srcTreeWeight * srcTreeWeight;
	}
      else  
	{
	  totalAbsWeight += qrtAgreeWeight;	  
	}
      
      
      //	  	  		printf("tmp Score is: %5.1f %5.1f %5.1f %5.1f %5.1f %5.1f\n",refTreeWeight,srcTreeWeight,
      //	   qrtAgreeWeight,totalwQrtAgree,totalSqrRefWeight,totalSqrSrcWeight);
      
    }
  
  
  float weightedQrtAgreeScore = 0;
  
  if(modelWithWeights == ON)
    {
      weightedQrtAgreeScore =  2*totalwQrtAgree/(totalSqrRefWeight + totalSqrSrcWeight)*100;
    }
  else  if(modelWithWeights != OFF)
    {
      weightedQrtAgreeScore =  totalwQrtAgree/totalAbsWeight*100;
    }
  
  
  qrtAgreeScore = totalQrtAgree/qrtNum*100;
  unresolvedScore = float(unresolved/qrtNum)*100;
  printf("Score is : agree:%5.1f wagree:%5.1f totwAgree:%5.1f totSqrRef%5.1f totSqrSrc:%5.1f totAbs:%5.1f unres:%g unresScore:%5.1f\n",
	 qrtAgreeScore,weightedQrtAgreeScore,
	 totalwQrtAgree,totalSqrRefWeight,totalSqrSrcWeight,totalAbsWeight,
	 unresolved,unresolvedScore);
  printf("Finish qrt agree\n"); 
  return(weightedQrtAgreeScore);
} 

/*---------------------------------------------------------------------*/

float treeQrtAgree(char* treeFname,int *spcArray,int ntaxa, int minTaxa,
		   qrtArrayType &qrtWeights,modelWithWeightsType modelWithWeights,
		   float &qrtAgree,float unresolved)
{
  treeNode **leaves = NULL;	      
  treeNode * t = readTree(treeFname,&leaves,ntaxa);
  //  print_tree(t,0);
  int nTreeTaxa = 0;	      
  nTreeTaxa = getTreeTaxa(t,&spcArray,nTreeTaxa);	      
  printf("Gen mat\n");

  bool withWeights = (modelWithWeights == SRC) || (modelWithWeights == ON);

  float *distMat= genMatFromTree(leaves,spcArray,nTreeTaxa,withWeights);
  printf("finish gen mat\n");
  float weightedQrtAgree = 0;
  
  if(nTreeTaxa < minTaxa)
    {
      printf("Skipping tree %s, ntaxa = %d < min taxa %d\n",
	     treeFname,nTreeTaxa,minTaxa);;
    }
  else
    {
      weightedQrtAgree = matWeightedQrtAgree((float *) distMat, spcArray, qrtWeights, 
					     nTreeTaxa, modelWithWeights,
					     qrtAgree,unresolved);
    }

  return(weightedQrtAgree);
}


/*---------------------------------------------------------------------*/

void tree2qrt(char* treeFname,int *spcArray,int ntaxa,int minTaxa,
	      qrtArrayType &qrtWeights,bool withWeights, float qnumFactor, bool addToQrtArray)
{
  treeNode **leaves = NULL;	      
  treeNode * t = readTree(treeFname,&leaves,ntaxa);
  print_tree(t,0);
  int nTreeTaxa = 0;
  
  fflush(stdout);
  nTreeTaxa = getTreeTaxa(t,&spcArray,nTreeTaxa);
  printf("Gen mat\n");
  fflush(stdout);
  float *distMat= genMatFromTree(leaves,spcArray,nTreeTaxa,withWeights);
  printf("Finished Gen mat\n");
  unsigned long qnum = pow(ntaxa,qnumFactor); 
  
  if(nTreeTaxa < minTaxa)
    {
      printf("Skipping tree %s, ntaxa = %d < min taxa %d\n",
	     treeFname,nTreeTaxa,minTaxa);;
    }
  else
    {
      printf("Add quartets \n");
      //     genRandomMatQrts(qrtWeights,distMat,spcArray, 
      //		       NULL,"",0, withWeights, nTreeTaxa, 100,qnum,addToQrtArray);
      
      addRandomQuartetsWeights((float *) distMat, spcArray, qrtWeights, 
			       nTreeTaxa, 100, false, qnum, withWeights, 
			       addToQrtArray);
      printf("finished add quartets\n");
    }
  //      delete(distMat);
}

/*--------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
  
  int i1;

 int qcnt=0, ecnt=0,i, cnt = 0;

  char   *taxaFname= NULL;
  char   *qrtFname= NULL;
  char   *treeListFname= NULL;
  strArrayType taxaArray;
  strArrayType treeListArray;
  
  // Easier to maintain

  srand ( (unsigned)time ( NULL ) );
  
  qrtNode * q_anchor=NULL;
  float *distMat = NULL;
  int ntaxa = -1;
  int correct = 100;
  int minTaxa = -1;
  float qnumFactor = 2;
  modelWithWeightsType modelWithWeights = OFF;

  char * select=NULL;
  char * srcTreeFname = NULL;
  char * refTreeFname = NULL;
  char * outFname = NULL;
  
  std::map<std::string, int> taxaIndex;
  
  parseParms(argc, argv, &refTreeFname, &srcTreeFname,
	     &treeListFname, &taxaFname,  &qrtFname, &outFname,
	     &qnumFactor, &ntaxa,  &correct, &minTaxa, &modelWithWeights,&select);


  FILE *outFile;
  
  if (outFname)
    {
      outFile = fopen(outFname, "w");
      if (outFile == NULL) {
	printf("file %s does not exist\n\n", outFname);
	exit(8);
      }
    }
  else
    outFile = stdout;
 
  //  exit (7);
  time_t t1 = time(NULL);
  
  printf("Started working at  %s\n", asctime(localtime(&t1)));
  
  if(taxaFname)
    {
      readNames(taxaFname,taxaArray,ntaxa);
      
      for(int i = 0; i < ntaxa; i++)
	{
	  taxaIndex[taxaArray[i]] = i;
	}
    }

  qrtArrayType qrtWeights;

  if(qrtFname)
    {
      int maxSp =-1;
      qrtNode * q_anchor=NULL;
      eleNode * e_anchor=NULL, * currE = NULL;

      qrtWeightType refQrtWeight;

      bool withWeights = (modelWithWeights == REF) || (modelWithWeights == ON);
      printf("with weights: %d\n",modelWithWeights);
      qcnt = readQuartets(qrtFname, &q_anchor, &e_anchor, &maxSp, withWeights);
      
      printf ("At main, back from readQuartets, maxSp %d\n", maxSp);
      
      qrtNode * current;
      qrtType refQrt;

      for (current = q_anchor; current != NULL; current = current -> next){
	refQrtWeight.reset();
	for(i=0;i<4;i++)
	  {
	    refQrt.n[i] = current->species[i];
	  }

	// printQrt("orig ",refQrt);
        // printWeight(" ",refQrtWeight);


	qrtSort(refQrt,refQrtWeight,current->weight);

	// printQrt(" sorted",refQrt);
        // printWeight(" ",refQrtWeight);
	qrtWeights[refQrt] = refQrtWeight;
      }      
    }
  
  int nTrees = 0;
  if(treeListFname)
    {
      printf("Reading tree list file %s\n",treeListFname);
      readNames(treeListFname,treeListArray,nTrees);
    }

  int *spcArray = (int *) calloc(ntaxa, sizeof(int));
  
  unsigned long qnum;

  if(refTreeFname)
    {
      bool withWeights = modelWithWeights == REF || modelWithWeights == ON;
      
      tree2qrt(refTreeFname, spcArray,ntaxa,minTaxa,qrtWeights,withWeights,qnumFactor,true);
    }
  
  float weightedQrtAgreeScore = 0;
  float qrtAgreeScore = 0;
  float unresolved = 0;

  if(srcTreeFname)
    {

      weightedQrtAgreeScore = treeQrtAgree(srcTreeFname,spcArray, ntaxa, minTaxa,
					   qrtWeights,modelWithWeights,
					   qrtAgreeScore,unresolved);

      fprintf(outFile,"%s %5.1f %5.1f %5.1f\n",srcTreeFname,qrtAgreeScore,
	      weightedQrtAgreeScore,unresolved);
      if(outFname!=NULL)
	printf(" %s: %5.1f %5.1f %5.1f\n",srcTreeFname,qrtAgreeScore,
	       weightedQrtAgreeScore, unresolved);
    }
  
  else if(nTrees > 0)
    {
      for(int i = 0; i < nTrees; i++)
	{

	  weightedQrtAgreeScore = treeQrtAgree((char *) treeListArray[i].c_str(),spcArray, 
					       ntaxa, minTaxa,qrtWeights,modelWithWeights,
					       qrtAgreeScore,unresolved);
	  fprintf(outFile,"%s %g %g\n",treeListArray[i].c_str(),qrtAgreeScore,weightedQrtAgreeScore);
	  if(outFname!=NULL)
	    printf("%s: %g %g\n",treeListArray[i].c_str(),qrtAgreeScore,weightedQrtAgreeScore);
	}
    }
  fclose(outFile);
}




