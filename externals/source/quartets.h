#ifndef QUARTETS_H
#define QUARTETS_H
#include <map>
#include <vector>
#include <string>

typedef std::vector<std::string> strArrayType;
enum qrtConfigType  {QRT_CONFIG_0123,QRT_CONFIG_0213,QRT_CONFIG_0312,NONE,QRT_CONFIG_MULT};

struct qrtWeightType
{
  /*
  weight[0] = weight(01|23);
  weight[1] = weight(02|13);
  weight[2] = weight(03|12);
  */

  float weight[3];
  int cnt;
  qrtWeightType()
  {
    reset();
  }
  
  void reset()
  {
    weight[0] = 0;
    weight[1] = 0;
    weight[2] = 0;
    cnt = 0;
  }

  qrtConfigType getConfig()
  {
    int mult = 0;
    qrtConfigType qrtConfig = NONE;
    
    if(weight[0] > 0)
      {
	qrtConfig = QRT_CONFIG_0123;
	mult++;
      }
    if(weight[1] > 0)
      {
	qrtConfig = QRT_CONFIG_0213;
	mult++;
      }

    if(weight[2] > 0)
      {
	qrtConfig = QRT_CONFIG_0312;
	mult++;
      }
    if(mult > 1)
      {
	qrtConfig = QRT_CONFIG_MULT;
      }
    return(qrtConfig);
  }
};
  
struct qrtType
{
  /*
  weight[0] = weight(01|23);
  weight[1] = weight(02|13);
  weight[2] = weight(03|12);
  */
  int n[4];
  
  qrtType(int n0,int n1,int n2,int n3)
  {
    n[0] = n0;
    n[1] = n1;
    n[2] = n2;
    n[3] = n3; 
  }

  qrtType()
  {
    n[0] = 0;
    n[1] = 0;
    n[2] = 0;
    n[3] = 0; 
  }

  void set(int n0,int n1,int n2,int n3)
  {
    n[0] = n0;
    n[1] = n1;
    n[2] = n2;
    n[3] = n3; 
  }

  bool operator<(qrtType const& qrt) const
  {
    int i;
    for(i=0;i<4;i++)
      {
	if(n[i] < qrt.n[i]) return (false);
	if(n[i] > qrt.n[i]) return (true);
	}
    
    return(false);
  }
};

typedef std::map<qrtType, qrtWeightType> qrtArrayType;
typedef qrtArrayType::iterator qrtArrayIter ;

void printQrt(const char *header, qrtType &qrt);
void printWeight(const char *header,qrtWeightType &weight);

void sortIndex(int intArray[], int indexArray[],int size);

float getMaxDist(float *distMat, int ntaxa);

int comp(const void * in_a,const void * in_b);

int getCharArg(char *argv[], const char *argName, char **argVal, int argNum);

/* Compute the qrt weight according to the four point method */

float computeWeight(float high, float med, float low);

/* Init a new node with taxa and weight values */
void initNode(qrtNode &node,qrtType &qrt,qrtConfigType qrtType, float weight);

/* compute type and weight of quartet */
/* Add quartet weight to the qrt weights 4D array */
/* Return the new qrt and its weight */

void addNewQuartet(float *distMat, int *spcArray,
		   qrtArrayType &qrtWeights,
		   int ntaxa, float maxDist, 
		   int t0,int t1, int t2, int t3,bool addToQrtArray,
		   qrtType &currentQrt,qrtWeightType &qrtWeight);

 /* Add weights to the qrt 4D arry */
qrtNode * addQuartetsWeights(float *distMat, int *spcArray, qrtArrayType &qrtWeights, int ntaxa, 
			     int pcorrect,  bool weightedPcorrect, 
			     bool withWeights,bool addToQrtArray);

void printQrt(qrtNode &node, FILE * qrtFile, bool withWeights);

void addQrt2list(qrtNode **q_anchor, qrtNode &node);

void handelQrt(qrtNode **q_anchor, qrtNode &node,unsigned long &qnum,
	       FILE *qrtFile, int genQrtList, bool withWeights);


/* Get quartet from the weighted array and add it to the qrt list */

void handelWeightedQrt(int t0, int t1, int t2, int t3, qrtArrayType &qrtWeights,  
		    qrtNode **q_anchor, int ntaxa, FILE *qrtFile, int genQrtList, 
		       bool withWeights, unsigned long &qnum);

void handelWeightedQrt(qrtType &qrt, qrtWeightType &qrtWeight,  
		       qrtNode **q_anchor, int ntaxa, FILE *qrtFile, int genQrtList, 
		       bool withWeights, unsigned long &qnum);

unsigned long handleAllQrts(qrtArrayType &qrtWeights,  qrtNode **q_anchor, 
			int ntaxa, char *qrtFname, int genQrtList, 
		   bool withWeights);

unsigned long genAllMatQrts(qrtArrayType &qrtWeights, float *distMat,int *spcArray, 
		   qrtNode **q_anchor, char *qrtFname, int genQrtList, 
		  bool withWeights, int ntaxa, int pcorrect,  
			    bool weightedPcorrect, bool AddToQrtArray);

void genRandomQrt(int ntaxa, int &t0,int &t1,int &t2,int &t3);
void flipQrts(qrtArrayType &qrtWeights, int pcorrect, bool weightedPcorrect);

unsigned long genRandomMatQrts(qrtArrayType &qrtWeights, float *distMat,int *spcArray, 
			       qrtNode **q_anchor, char *qrtFname, int genQrtList, 
			       bool withWeights, int ntaxa, int pcorrect, 
			       bool weightedPcorrect, unsigned long qnum, 
			       bool addToQrtArray);


qrtNode * addRandomQuartetsWeights(float *distMat, int *spcArray, qrtArrayType &qrtWeights,
				   int ntaxa, int pcorrect,  
				   bool weightedPcorrect, unsigned long qnum, 
				   bool withWeights,bool addToQrtArray);


unsigned long readQuartets(char * qrtFname, qrtNode ** q_anchor, eleNode ** e_anchor, int *maxEle, bool withWeights);

int   getQrt(qrtNode *q, FILE * f, int *maxEle, bool withWeights);

eleNode * buildEleList(qrtNode * q_anchor, int ntaxa, int *nelem, int *maxSp);


#endif
