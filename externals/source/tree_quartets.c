

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
  
void print_tree(treeNode *t, strArrayType &taxaArray, int ntaxa, FILE *ftree)
{
  //  printf ("IN PRINT TREE. node id :%d, val %d\n",t->id, t-> val);
  int nodeValue = t->val;
  if (nodeValue >= 0)
    {
	if(!taxaArray.empty())
	  fprintf(ftree,"%s", taxaArray[nodeValue].c_str());
	else
	  fprintf(ftree,"%d", nodeValue);
    }
  else {

    if (t->son != NULL){
      fprintf(ftree,"(");
      print_tree(t->son,taxaArray,ntaxa,ftree);
      //      printf("back from son %d\n",t->son->id);
      fprintf(ftree,")");
    }
    else{
      ERR("Node %d has no value and no sons\n",t->id);
    }
  }

  if (t->brother != NULL)
    {
      //      printf("node %d has a brother %d\n",t->id, t->brother->id);
      fprintf(ftree,",");
      print_tree(t->brother,taxaArray,ntaxa, ftree);
    }
  else{
    //    printf("node %d has a NO brother \n",t->id);
  }
}


/*--------------------------------------------------------------------------*/ 
eleNode * buildEleList(qrtNode * q_anchor, int ntaxa, int *nelem, int *maxSp){
  
  //	exit (3);
  int * eleVec = (int *) my_calloc (ntaxa+8, sizeof(int) );
  int i;
  eleNode *e_anchor = NULL;
  qrtNode * current = q_anchor;
  for(i=0;i<ntaxa;i++)
    eleVec[i] = 0;

  for ( current = q_anchor; current != NULL; current = current -> next){
 
    for (i =0; i < 4; i++){
      if (eleVec[current->species[i]]){
	//		printf("Not first time to see %d\n", current->species[i]);
      }
      else{
	//	printf("first time to see %d\n", current->species[i]);
	if(current->species[i] > (*maxSp))
	  *maxSp = current->species[i];
	
	eleVec[current->species[i]] = 1;
	addElement2list(&e_anchor, current->species[i]);
	(*nelem)++;
      }
    }   
  }  
  my_free(eleVec);
  return e_anchor;
}


/*---------------------------------------------------------------------*/


void buildTree(char * outTreeFname, qrtNode * q_anchor,strArrayType &taxaArray,
	       unsigned long qnum, int ntaxa)
{
  treeNode *finalTree= NULL; 
  int treeNodeCnt = 0;
  FILE *ftree = NULL;
  int maxSp = -1;
  int nelem = 0;
  eleNode * e_anchor = buildEleList(q_anchor, ntaxa, &nelem, &maxSp);

  printf ("\n\nNumber of quartets read: %ld max ele: %d\n\n\n", qnum,  maxSp);


  if(qnum==0)
    {
      printf("No qrt found for tree %s\n",outTreeFname);
      return;
    }

  if (e_anchor == NULL){
    printf("ERORR ERROR ERROR: Invalid return from buildEleList\n\n");
  }
  
  time_t t1 = time(NULL);

  printf("calling find cut num_e: %d num_q: %ld, max_sp: %d num_node: %d\n",
	 nelem, qnum, maxSp, treeNodeCnt);

  finalTree =  
    find_cut(e_anchor, nelem, q_anchor, qnum, maxSp, &treeNodeCnt);
  

  //  my_free(e_anchor);
  ftree = fopen (outTreeFname,"w");
  
  if (!ftree){
    ERR("Could not open output tree file MXCNtree.dat\n\n");
  }
  
  t1 = time(NULL);						
  printf("Ended working at  %s\n", asctime(localtime(&t1)));
  print_tree(finalTree,taxaArray,ntaxa,ftree);
  fprintf(ftree,";\n");
  fclose(ftree);
}


/*---------------------------------------------------------------------*/

int getCharArg(char *argv[], const char *argName, char **argVal, int argNum)
{
  size_t strSize = strlen(argName);
  if (! strncmp(argv[argNum],argName, strSize)){
    printf("found %s%s\n", argName, argv[argNum]+strSize);
    *argVal = argv[argNum]+strSize;
    return (1);
  }
  return (0);
}

/*---------------------------------------------------------------------*/
void printUsage()
{
    printf("\n\nUsage: tree_quartets "  
           "\n out_qrt_file=<qrt file name> "
	   "\n out_tree_file=<tree file name> The super tree output file name" 
	   "\n mat_file=<matrix file name>"
	   "\n tree_file=<tree file name>"
	   "\n mat_list_file=<matrix list file name>"
	   "\n tree_list_file=<tree list file name>"
	   "\n taxa_file=<taxa file name>"	   
	   "\n qnum_factor=<factor of required quartets related to ntaxa>"
	   "\n ntaxa=<size of taxa>"
	   "\n pcorrect=<percent of correct qrts>"
	   "\n w_pcorrect=<percent of weighted correct qrts>"
	   "\n min_taxa=<minimum taxa size for mat/tree>"
	   "\n select=<all/random/best> qrt selection"
	   "\n seperate_trees=<on/off> get seperate trees for every matrix"
	   "\n weights=<on/off> get qrt file with weights"
	   "\n");
}

/*---------------------------------------------------------------------*/
  
void parseParms(int argc, char *argv[], char **qrtFname,  
		char **treeFname, char **outTreeFname,
		char **dmatFname, char **matListFname, 
		char **treeListFname, char **taxaFname, 
		float *qnumFactor, int *ntaxa, 
		int *pcorrect, bool *weightedPcorrect, 
		int *minTaxa, int *withWeights, 
		char **select, int *seperateTrees)
{
  int i;
  *qrtFname =  NULL;
  my_printf ("at parse parm. argc %d\n",argc);
  char *str_ntaxa ;
  char *str_pcorrect ;
  char *str_weightedPcorrect ;
  char *str_minTaxa;
  char *str_qnumFactor;
  char *str_withWeights;
  char *str_seperateTrees;

  for (i=1; i< argc; i++){
    my_printf ("parm %d is %s\n",i,argv[i]);

    getCharArg(argv,"out_qrt_file=", qrtFname, i);
    getCharArg(argv,"out_tree_file=", outTreeFname, i);
    if(getCharArg(argv,"mat_file=", dmatFname, i))
      *seperateTrees = 1;
    getCharArg(argv,"tree_file=", treeFname, i);
    getCharArg(argv,"taxa_file=", taxaFname, i);
    getCharArg(argv,"mat_list_file=", matListFname, i);
    getCharArg(argv,"tree_list_file=", treeListFname, i);
    getCharArg(argv,"select=", select, i);
    if(getCharArg(argv,"qnum_factor=", &str_qnumFactor, i))
       *qnumFactor = atof(str_qnumFactor);
    if(getCharArg(argv,"min_taxa=", &str_minTaxa, i))
       *minTaxa = atoi(str_minTaxa);
    if(getCharArg(argv,"ntaxa=", &str_ntaxa, i))
      {
	*ntaxa = atoi(str_ntaxa);
      }
    if(getCharArg(argv,"pcorrect=", &str_pcorrect, i))
      {
	*pcorrect = atoi(str_pcorrect);
      }
    if(getCharArg(argv,"w_pcorrect=on", &str_weightedPcorrect, i))
      {
	*weightedPcorrect = true;
      }
    if(getCharArg(argv,"weights=on", &str_withWeights, i))
       *withWeights = 1;
        if(getCharArg(argv,"seperate_trees=on", &str_seperateTrees, i))
       *seperateTrees = 1;
  }
  

  if (((*dmatFname == NULL) && (*matListFname == NULL) && 
       (*treeFname == NULL) && (*treeListFname == NULL))){
    printf("ERROR !!!!!  missing required arg: mat_file or mat_list_file!");
    printUsage();
  }
  else 
    if ((*ntaxa) == -1){
      printf("ERROR !!!!!  missing required arg: ntaxa!");
      printUsage();
      exit(2);
    }
    else 
      if ((*pcorrect) < 0 || ((*pcorrect) > 100)){
	printf("ERROR !!!!!  wrong arg pcorrect %d !",*pcorrect);
	printUsage();
	exit(2);
      }
}




/*---------------------------------------------------------------------*/

 /*
      Read distance matrix
      input: matFname - matrix name
             taxaIndex - Index of taxa names 
      output: 
                      distMat - distance matrix
 		     spcArray - spcieces Array
 		     
 */
float * readMat(char * matFname, int **spcArray, int *pn, 
		std::map<std::string, int> &taxaIndex){
  printf("Reading matrix file %s \n", matFname);
  FILE *matFile = fopen(matFname, "r");
  int ntaxa;

  long file_size;
  char *buffer;

  if (matFile == NULL) {
    printf("Matrix file %s does not exist\n\n", matFname);
    exit(8);
  }
  
  
  if (fseek(matFile, 0 , SEEK_END) != 0) {
    /* Handle Error */
}
  
  file_size = ftell(matFile);
  if (file_size == -1) {
    /* Handle Error */
  }
  printf ("size for file %s: %ld\n", matFname, file_size);
  rewind(matFile);

  buffer = (char*)malloc(file_size);
  if (buffer == NULL) {
    printf("Malloc for buffer failed\n");
    exit(8);
    /* Handle Error */
  }
  
  if (fgets(buffer,200, matFile) == NULL) {
    printf("Could not read the first line\n");
    exit(8);
  }
  
    if (sscanf(buffer, "%d", &ntaxa) != 1){
      printf ("Scanf for #taxa failed at %s\n",buffer);
      exit(8);
    }

    my_printf ("Found ntaxa %d\n", ntaxa);
    *pn = ntaxa;
    float *distMat=  (float *) calloc (ntaxa * ntaxa, sizeof(float) ); 
   
    int i;
    
  char *ptr;

  int i1;

  for (i1 = 0; i1 < ntaxa; i1 ++){
    //    printf("Now in line %d\n", i1);
    if (fgets(buffer,file_size, matFile) == NULL) {
      printf("Dist matrix file ended too quick after %d line\n", i1);
      exit(8);
    }
    ptr = strtok(buffer," ,\t,\n");

    if(!taxaIndex.empty())
      {
	if(taxaIndex.find(ptr)!= taxaIndex.end())
	  {
	    (*spcArray)[i1] = taxaIndex[ptr];
	  }
	else
	  {
	    printf("ERORR ERROR ERROR: taxa %s in mat is not found in taxa list\n\n",ptr);
	  exit(1);
	  }
      }
    else
      {
	int taxa_num;
	if (sscanf(ptr,"%d", &taxa_num) != 1){
	  printf ("Scanf for taxa_num failed at %s\n",ptr);
	  exit (7);
	}
	(*spcArray)[i1] = taxa_num;
      }
 
    ptr = strtok(NULL," ");
  
 
    int j1;
    for (j1=0; (ptr != NULL && j1 < ntaxa); j1++){
      float f1;
      if (sscanf(ptr,"%f", &f1) != 1){
	printf ("Scanf failed at %s\n",ptr);
	exit (7);
      }
      else{
	//   	printf ("translated [%d,%d]: %f\n",i1,j1,f1);
	distMat[(i1*ntaxa) + j1] = f1;
      }
    
      ptr = strtok(NULL," ");
    }
  
    if (j1 != ntaxa){
      printf("read %d entries instead of ntaxa:%d\n", j1, ntaxa);
      exit (8);
    }
  
  }
  free(buffer);
  fclose(matFile);
  return distMat;
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



/*--------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
  
  int i1;

 int qcnt=0, ecnt=0,i, cnt = 0;
  char   *dmatFname= NULL;
  char   *taxaFname= NULL;
  char   *matListFname= NULL;
  char   *treeListFname= NULL;
  char *qrtFname=NULL;
  strArrayType taxaArray;
  strArrayType matListArray;
  strArrayType treeListArray;
  float qnumFactor = 2;
  int alpha  = 3;
  
// Easier to maintain


  
  srand ( (unsigned)time ( NULL ) );

  qrtNode * q_anchor=NULL;
  float *distMat = NULL;
  int ntaxa = -1;
  int pcorrect = 100;
  bool weightedPcorrect = false;
  int minTaxa = -1;
  int withWeights = 0;
  int seperateTrees = 0;
  char * select=NULL;
  treeNode *finalTree= NULL;  
  char * treeFname = NULL;
  char * outTreeFname = NULL;

  
  std::map<std::string, int> taxaIndex;
  
  parseParms(argc, argv, &qrtFname, &treeFname,  &outTreeFname,
	     &dmatFname,  &matListFname, &treeListFname, &taxaFname,
	     &qnumFactor, &ntaxa,  &pcorrect, &weightedPcorrect, 
	     &minTaxa, &withWeights,&select, &seperateTrees);

  //  exit (7);
  time_t t1 = time(NULL);
  
  printf("Started working at  %s\n", asctime(localtime(&t1)));
  
  unsigned long requiredQnum = pow(ntaxa,qnumFactor);

  printf("qnum = %f\n",qnumFactor);

  int genQrtList = 0;
  if(outTreeFname)
    {
      genQrtList = 1;
    }

  if(taxaFname)
    {
      readNames(taxaFname,taxaArray,ntaxa);
 
      for(int i = 0; i < ntaxa; i++)
	{
	  taxaIndex[taxaArray[i]] = i;
	}
    }
  
  int nMatrixs = 0;
  if(matListFname)
    {
      printf("Reading mat list file %s\n",matListFname);
      readNames(matListFname,matListArray,nMatrixs);
    }

  int nTrees = 0;
  if(treeListFname)
    {
      printf("Reading tree list file %s\n",treeListFname);
      readNames(treeListFname,treeListArray,nTrees);
    }

  int *spcArray = (int *) calloc(ntaxa, sizeof(int));
  
  unsigned long qnum;
  qrtArrayType qrtWeights;

  if(matListFname && seperateTrees)
    {
      for(int i = 0; i < nMatrixs; i++)
	{
	  int matNtaxa;
	  char *  matName = (char *) matListArray[i].c_str();
	  std::string treeExt = ".tree";
	  std::string treeName = matName + treeExt;
	  q_anchor = NULL;
	  qrtFname = NULL;
	  
	  distMat = readMat(matName, &spcArray, &matNtaxa,taxaIndex);
	  if (distMat == NULL){
	    printf("ERORR ERROR ERROR: Invalid return from read mat\n\n");
	    exit(1);
	  }
	  qnum = genAllMatQrts(qrtWeights, distMat,spcArray, &q_anchor, 
			       qrtFname, genQrtList, withWeights, matNtaxa, 
			       pcorrect, weightedPcorrect, false);
	  buildTree((char *) treeName.c_str(), q_anchor, taxaArray, qnum,ntaxa);
	  free (distMat);
	  distMat = NULL;
	}
      return (0);
    }

  if(dmatFname)
    {
      printf("Reading mat file %s\n",dmatFname);
      distMat = readMat(dmatFname, &spcArray, &ntaxa,taxaIndex);
      if (distMat == NULL){
	printf("ERORR ERROR ERROR: Invalid return from read mat\n\n");
	exit(1);
      }
      if(select && !strncmp(select,"random",5))
	qnum = genRandomMatQrts(qrtWeights, distMat,spcArray, &q_anchor, 
				qrtFname, genQrtList, withWeights, ntaxa,
				pcorrect, weightedPcorrect, 
				requiredQnum,false);
      else
	qnum = genAllMatQrts(qrtWeights, distMat,spcArray, &q_anchor, 
			     qrtFname, genQrtList, withWeights, ntaxa,
			     pcorrect, weightedPcorrect, false);
    }

  if(treeFname)
    {
      treeNode **leaves = NULL;	      
      treeNode * t = readTree((char *) treeFname,&leaves,ntaxa);
      
      int nTreeTaxa = 0;	      
      nTreeTaxa = getTreeTaxa(t,&spcArray,nTreeTaxa);	      
      printf("Gen mat\n");

      int realDist = 1;
      float *distMat= genMatFromTree(leaves,spcArray,nTreeTaxa,realDist);
       printf("Start gen quartets\n");

       if(select && !strncmp(select,"random",5))
	 qnum = genRandomMatQrts(qrtWeights, distMat,spcArray, &q_anchor, 
				 qrtFname, genQrtList, withWeights, nTreeTaxa,
				 pcorrect,weightedPcorrect, requiredQnum,false);
       else
	 qnum = genAllMatQrts(qrtWeights, distMat,spcArray, &q_anchor, 
			      qrtFname, genQrtList, withWeights, nTreeTaxa,
			      pcorrect,weightedPcorrect, false);
      
      printf("Finish gen quartets\n");
    }
      
  if(nMatrixs > 0 || nTrees > 0)
    {      
      if(nMatrixs > 0) 
	{
	  for(int i = 0; i < nMatrixs; i++)
	    {
	      int matNtaxa;
	      distMat = readMat((char *) matListArray[i].c_str(), &spcArray, &matNtaxa,taxaIndex);
	      
	      if(matNtaxa < minTaxa)
		{
		  printf("Skipping matrix %s, ntaxa = %d < min taxa %d\n",
			 matListArray[i].c_str(),matNtaxa,minTaxa);
		  continue;
		}
	      
	      addQuartetsWeights(distMat, spcArray, qrtWeights, 
				 matNtaxa, 100,weightedPcorrect,withWeights,true);

	      if (distMat == NULL){
		printf("ERORR ERROR ERROR: Invalid return from read mat\n\n");
	      }
	    }
	}
      
      else if(nTrees > 0)
	{
	  for(int i = 0; i < nTrees; i++)
	    {	     
	      treeNode **leaves = NULL;	      
	      treeNode * t = readTree((char *) treeListArray[i].c_str(),&leaves,ntaxa);
	      
	      int nTreeTaxa = 0;	      
	      nTreeTaxa = getTreeTaxa(t,&spcArray,nTreeTaxa);	      
	      printf("Gen mat\n");
	      int realDist = 1;
	      float *distMat= genMatFromTree(leaves,spcArray,nTreeTaxa,realDist);
	      if(nTreeTaxa < minTaxa)
		{
		  printf("Skipping tree %s, ntaxa = %d < min taxa %d\n",
			 treeListArray[i].c_str(),nTreeTaxa,minTaxa);
		  continue;
		}
	      printf("Add quartets\n");
	      addQuartetsWeights((float *) distMat, spcArray, qrtWeights, 
				 nTreeTaxa, 100, weightedPcorrect, 
				 withWeights,true);

	      printf("finished add quartets\n");
	      //      delete(distMat);
	      
	    }
	  printf ("finished reading trees!!\n");
	} 
    }
  
  if(!qrtWeights.empty())
    {
      if(pcorrect < 100)
	flipQrts(qrtWeights,pcorrect,weightedPcorrect);

      qnum = handleAllQrts(qrtWeights, &q_anchor,ntaxa, qrtFname, genQrtList, 
			   withWeights);
    }
  
  if (outTreeFname == NULL)
    return (0);
  fflush(stdout);
  buildTree(outTreeFname, q_anchor, taxaArray, qnum,ntaxa);
  return 0;
}



