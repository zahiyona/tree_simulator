#ifndef QMC_H
#define QMC_H

#define WARN(...) {					\
    char msg[1000];					\
    sprintf(msg, "WARNING: "__VA_ARGS__);	\
    log_msg(msg);					\
  }


#define ERR(...) {					\
    char msg[2000];					\
    sprintf(msg, "ERROR RROR ERROR: "__VA_ARGS__);	\
    log_msg(msg);					\
  }


#define EXIT(...) {					\
    char msg[2000];					\
    sprintf(msg, "ERROR RROR ERROR: "__VA_ARGS__);	\
    log_msg(msg);					\
    exit(4);						\
  }

#define my_free(a) { if (a) {free(a); a = NULL;}	\
	else {ERR("Freeing invalid NULL pointer\n");} 	\
}


#define MY_CALLOC(a, b, c) {					\
  if (a) {printf("try to allocate a non empty ptr \n");	\
    exit(4);							\
  }								\
  else{								\
    a = (void *) calloc (b, c);					\
    if (a == NULL){EXIT("Allocation for a failed\n");}		\
  }								\
}

void *my_malloc(int size);
void *my_calloc(int n, int size);

typedef struct tree_node {
  int val;                          // leaf number (-1 at internal nodes)
  float dist;                       // auxiliary field to compute distances between nodes
  int id;                          // Node id (serves for internal use only
  float len;                         // length to father
  struct tree_node *father;        // pointer to father
  struct tree_node *brother;       // pointer to right brother (NULL at last brother in chain)
  struct tree_node  *son;          // pointer to first son (NULL at leaves)
} treeNode;



#ifdef DEBUG_PRT
#define my_printf(format, args...) printf(format, ##args);
#else
#define my_printf(format, args...)
#endif


typedef struct qrtStruct {
  int species[4];
  float weight;
  struct qrtStruct * next;
}qrtNode;



typedef struct eleStruct {
  int species;
  struct eleStruct * next;
}eleNode;

typedef struct compsStruct {
  struct eleStruct *comp1;
  struct eleStruct *comp2;
}compsStruct;

void print_tree2file(treeNode *t, FILE *ftree);

int randInt(int max);

int getStr(char *dest, int destLen, char * source, int cpyLen);
 
treeNode * lca(int leaf1, int leaf2, treeNode **leaves);

treeNode * parseTree(char * tree, int max_taxa, treeNode ***p_leaves, int *cnt);

int mark_parent(treeNode *s,int i, int level);

void log_msg(char *msg);

int * cutGraph(int n, float * adjMat, int *minPart);

void print_tree(treeNode *t, int detail);

void check_tree(treeNode *t, treeNode *f);

void split_edge(treeNode *above, treeNode *subtree, int *pCnt);

void FiducciaMattheyses(int * bestPart, float *adjMat, int n);

treeNode **find_preceedance(treeNode * node);

void insert_subtree_allover(
			    treeNode *globalRoot,
			    treeNode *localRoot, 
			    treeNode *subtree,
			    int *pCnt);

void check_one_son(treeNode *father, int *pCnt);

treeNode * removeSubtree( treeNode *subtree, int *pCnt);

treeNode *make_tree(
		    int first, 
		    int after,
		    int random,
		    int *pCnt, 
		    treeNode **leaves);


treeNode *find_cut(
		   eleNode * e_anchor,
		   int ecnt, 
		   qrtNode * q_anchor, 
		   unsigned long qcnt, 
		   int maxSp, 
		   int *pCnt);



treeNode *join_trees(
		     treeNode *root0,
		     int outg0,
		     treeNode *root1,
		     int outg1,
		     int max_taxa,
		     int *pCnt);

// compsStruct * MaxCut(eleNode * e_anchor, int ecnt, qrtNode * q_anchor, int qcnt, int maxSp);
// char * recur (eleNode * e_anchor, int ecnt, qrtNode * q_anchor, int qcnt, int maxSp, int level);

treeNode * newNode(treeNode * father, int newId);

void addSon(treeNode *father, treeNode *son);

void addElement2list(eleNode **e_anchor, int eleId);

double real_pairwise_dist(treeNode * leaves[], int i, int j);

treeNode *readTree(char *treeFname, treeNode ***leaves, int ntaxa);

int getTreeTaxa(treeNode *t, int **spcArray,int nTreeTaxa);

float * genMatFromTree(treeNode **leaves, int spcArray[], int ntaxa, int realDist);

#endif
