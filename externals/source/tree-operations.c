#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "QMC.h"


#define MAXSTRLEN 1000
#define MAXTAXALEN 100
#define OVECCOUNT 30    /* should be a multiple of 3 */



/*---------------------------------------------------------------------*/

treeNode * lca(int leaf1, int leaf2, treeNode **leaves){
  int level,i;
  treeNode * node;

  mark_parent(leaves[leaf1]->father,leaf1,0);
  level = mark_parent(leaves[leaf2]->father,leaf2,0);
  mark_parent(leaves[leaf1]->father,-1,0);
  mark_parent(leaves[leaf2]->father,-1,0);
  node  = leaves[leaf2]->father;
  for (i=0; i < level; i ++){
    node = node -> father;
  }
  
  //  printf("\nfather of leaf %d with id %d, and of leaf %d with id %d"
  //	 " is node with id %d\n", 
  // 	 leaf1,leaves[leaf1]->id,leaf2,leaves[leaf2]->id,node->id);
    return node;
}

/*---------------------------------------------------------------------*/





/*---------------------------------------------------------------------*/


double getLen(char *tree, int *treeInd)
{
   int i;
   char temp[100];
   int lenlen;
    double len;

	//  char digits[20] = "0123456789";
    //  printf("\n\nAt getLen\n\n");

  if (*(tree + *treeInd) != ':'){
    ERR("getLen was called with no colon at column %d\n",*treeInd);
  }


  i = (*treeInd)+1;
  for (; (*(tree + i) >= '0') && (*(tree + i) <= '9');i++){
    //    printf ("digit %d is %c\n", i , *(tree + i));
  }
  if (*(tree + i) =='.'){
    // printf ("found a dot at %d\n",i);
    for (i=i+1; (*(tree + i) >= '0') && (*(tree + i) <= '9');i++){
      //  printf ("digit %d is %c, \n", i , *(tree + i)) ;
    }
  }

 
  lenlen = i - *treeInd -1;
  //   printf("len is %d\n", lenlen);
  strncpy(temp,(tree + *treeInd+1), lenlen);
  temp[lenlen] = '\0';
  // printf ("the string len is %s, len is %d\n",temp, lenlen);
  *treeInd = i;
 
  sscanf(temp,"%lg",&len);
  // printf ("len received is %f\n",len);
  return len;

}


/*---------------------------------------------------------------------*/

double real_dist(treeNode *s, double dist)
{
  //  printf ("at node %d, dist =%f\n",s-> id, dist);
  if ((s->dist != -1))
    {
      //      printf("found the lca with dist %f. Returning %f\n", 
      // 	     s-> dist,  dist + (s -> dist);
      return (dist + (s -> dist));
    }
  else
    {
      s -> dist = dist;
      if (s->father == NULL){
	  return 0;
	}
      else{
	double ret =  real_dist(s->father, dist + ( s -> len));
	//	printf("returned %f\n", ret);
	return ret;
      }
    }
}


/*---------------------------------------------------------------------*/

int topological_dist(treeNode *s, int level)
{
  //  printf ("at level %d, i=%d\n",level,i);
  if ((s->dist != -1))
    {
      return level + s -> dist;
    }
  else
    {
      s->dist = level;
      if (s->father == NULL){
	  return 0;
	}
      else
	return topological_dist(s->father, level+1);
    }
}


/*---------------------------------------------------------------------*/



void reset_dist(treeNode *s)
{
  //  printf ("at level %d, i=%d\n",level,i);
  if ((s -> dist != -1))
    {
      s -> dist = -1;
      if (s -> father) reset_dist (s -> father);
    }
}


/*---------------------------------------------------------------------*/

int topological_pairwise_dist(treeNode * leaves[], int i, int j){
  int dist;
 // printf ("in topo dist.i %d, j %d, \n", i,j);

  //  printf ("flip %d\n", flip);
  if (i == j ) return -1;


  //  printf ("All the taxa %d, %d or %d  exist\n", i,j,k);
 
  reset_dist(leaves[i]->father);
  reset_dist(leaves[j]->father);

  topological_dist(leaves[i]->father,1);
  dist = topological_dist(leaves[j]->father,1);
  
  return dist;
}

/*---------------------------------------------------------------------*/


double real_pairwise_dist(treeNode * leaves[], int i, int j){
  double dist;
 // printf ("in topo dist.i %d, j %d, \n", i,j);

  //  printf ("flip %d\n", flip);
  if (i == j ) return -1;


  //  printf ("All the taxa %d, %d or %d  exist\n", i,j,k);


  reset_dist(leaves[i]->father);

  reset_dist(leaves[j]->father);

  real_dist(leaves[i]->father, leaves[i]->len);
  
  dist = real_dist(leaves[j]->father, leaves[j]->len);
  //  printf("dist returned %f\n", dist);

  return dist;
}


/*---------------------------------------------------------------------*/





int mark_parent(treeNode *s,int i, int level)
{
  if (s == NULL) {
    printf ("invalid tree\n");
    return -1;
  }
  //  printf ("at level %d, i=%d\n",level,i);
  if ((s->val != -1) && (i != -1))
    {
      //      printf("found %d at val\n", s->val);
      return level;
    }
  else
    {
      s->val =i;
      if (s->father == NULL){
	  return level;
	}
      else
	return mark_parent(s->father,i,level+1);
    }
}


/*---------------------------------------------------------------------*/

treeNode **find_preceedance(treeNode * node){
  treeNode * father = node->father, *temp;
  //  printf("in find preceedence of node %d with father %d\n",
  //	 node->id, node->father->id);
  if (father->son == node){
    //    printf ("node %d is the first son of father %d\n",
    //   node->id,father->id);
    return (&(father->son));
  }
  temp=father->son;
  while ((temp->brother != NULL) && (temp->brother != node)){
    temp=temp->brother;
  }
  if (temp->brother == NULL){
    printf("\n\nERROR ERROR. the requested node %d "
	   "was not found in its brothers list\n",node->id);
    exit(6);
  }
  return (&(temp->brother));
}


/*---------------------------------------------------------------------*/

void insert_subtree_allover(treeNode *globalRoot, treeNode *localRoot, 
			    treeNode *subtree, int *pCnt){
  // printf("\nIn insert all over. node is %d, subtree is %d\n",
  // 	 localRoot->id,subtree->id);
  if (localRoot->son != NULL){
    treeNode *node = localRoot->son;
    while (node != NULL){
      // printf("splitting edge abone %d\n",node->id);
      split_edge(node, subtree, pCnt);
      // printf("\ntree after insertion:\n");
      // print_tree(globalRoot, 0);
      check_tree(globalRoot,NULL);      
      // printf("\nnow removing the subtree:\n");
      removeSubtree(subtree, pCnt);
      // print_tree(globalRoot, 0);
      insert_subtree_allover(globalRoot,node,subtree, pCnt);
      node = node->brother;
    }
    // printf("at allover, finished all brothers\n");
  }
  else{
    // printf ("arrived at a leaf %d\n",localRoot->val);
  }
  
}
/*---------------------------------------------------------------------*/

treeNode * removeSubtree( treeNode *subtree, int *pCnt){
  treeNode  *father, **ptr;
  
  //  printf ("at remove subtree. remove id %d \n", subtree->id);
  //  print_tree(subtree);
  father = subtree-> father;
  if (father -> son -> brother == NULL){
    printf ("ERROR ERROR, father of subtree %d, with id %d has "
	    "only one son %d\n", subtree->id, father->id, 
	    father->son->id);
    exit(3);
  }
  ptr = find_preceedance(subtree);
  *ptr = subtree->brother;
  //  free (subtree);
  check_one_son(father, pCnt);
  return father;
  
}
/*---------------------------------------------------------------------*/

void check_one_son(treeNode *father, int *pCnt){
  //  treeNode * temp, *temp1;
 treeNode  **ptr;
  //  printf("in check one son of father %d\n",father->id);
  if (father -> son-> brother == NULL){
    if (father -> father == NULL){
      if (father->id != 0){
	printf("ERROR ERROR. A node different than 0 has null father!!!");
	exit(3);
      }
      //      printf ("father %d has only one son %d.but is the root.\n",
      //	      father->id);
      return;
    }
    
    //    printf ("father %d has only one son %d. deleting father.\n",
    //	    father->id, father->son->id);
    ptr = find_preceedance(father);
    if ((*ptr ) != father){
      printf ("ERROR ERROR!! *prt %p != node %p\n",
	       *ptr,  father);
      exit (5);
    }
    //    printf("\n\n\nfound the father %d\n",(*ptr)->id);
    *ptr = (father -> son);
    (*ptr)->brother =  father -> brother;
    (*ptr)->father = father->father;
    if (father->id == (*pCnt) -1){
      //      printf("freeing the last node allocated %d."
      //" decrease cnt to %d\n",  father->id,cnt-1);
      (*pCnt)--;
    }
    free (father);
    return;
  }
  //  printf("father has more than one child\n");
}
     
     
/*---------------------------------------------------------------------*/


void split_edge(treeNode *above, treeNode *subtree, int *pCnt)
{
  treeNode **ptr = find_preceedance(above);
  treeNode *newInternal = newNode(above->father,(*pCnt)++);
  *ptr  = newInternal;
  subtree->father = above->father = newInternal;
  newInternal->brother = above->brother;
  newInternal->son = above;
  above->brother = subtree;
  subtree->brother = NULL;
}


/*---------------------------------------------------------------------*/

treeNode *make_tree(int first, int after, int random, int *pCnt, treeNode **leaves)
{
  treeNode *s,*s1,*s2;
  int mid;
  //  printf ("first %d after %d\n",first,after);

  s =  newNode (NULL,(*pCnt)++);

  if ((after - first) == 0)
    {
      //      printf ("a leaf %d\n",first);
      s->val = first;
      leaves[first] = s;
    }
  else if ((after - first) > 0)
    {
      if (random ==0)
	{
	  mid = first + ((after-first)/2);
	}
      else
	{
	  mid = first + randInt(after-first);
	}
      
      s1 = make_tree(first, mid, random, pCnt, leaves);
      s2 = make_tree(mid+1, after, random, pCnt, leaves);
      
      s->son = s1;
      s1->brother = s2;
      s1->father= s;
      s2->father= s;
      s->val = -1;
      
    }
  else{
    ERR("Invalid values for first %d and after %d\n",
	first, after);
    
  } 
  
  
  return s;
}

/*---------------------------------------------------------------------*/


int valid_tree(char *tree, int *max_taxa, double *max_len)
{
  int treeLen;
  int i, balance =0;
  char tempTree[200000];
  char *word;
  int taxon = -1;
  float edgeLen = -1;
  int wlen;
  char * sep;
  int taxonLen;
  int lenlen;
  int fieldsRead;

  printf("\n\nat  valid tree for tree: %s\n",tree);

  treeLen = strlen(tree);
  if (tree[treeLen-1] == ';'){
    printf ("tree %s ends with ';'", tree);
    tree[treeLen-1] = '\0';
    treeLen--;
  }

  if (tree[0] != '(' && tree[treeLen-1] != ')'){
    ERR("tree %s does not begin and end with '(' and ')'\n\n", tree);
  }
  
  for ( balance =0, i = 0; i < treeLen; i++){
    //    printf ("%c, bal %d\n",tree[i],balance);
    switch(tree[i])
      {
      case '(': balance++; break;
      case ')': balance--; 
	if (balance < 0){
	  printf("ERROR ERROR ERROR: invalid tree %s\n\nBalance %d at pos %d\n",
		 tree, balance, i);
	  exit(8);
	}
	break;
      }
  }
  if (balance != 0){
    printf("ERROR ERROR ERROR: invalid tree %s\n\nBalance %d at the end\n",tree, balance);
    exit(8);
  }


  strcpy(tempTree, tree);
  printf ("tree is %s, len %d\n", tempTree, strlen(tempTree));


  //  exit(8);
  word = strtok(tempTree,"(),");

  while (word != NULL)
    {

      wlen = strlen(word);
      //       printf("next match: %s\n", word);
      sep = (char *) memchr(word, ':', wlen);
      if (sep != NULL){
	//	printf ("a separator : was found\n");
	taxonLen = sep - word;
	lenlen = wlen - taxonLen;
	//	printf ("taxon len is %d, lenlen is %d\n", taxonLen, lenlen);
	fieldsRead = sscanf(word,"%d:%f",&taxon, &edgeLen);
	//	printf("Fields read %d, taxon %d\n",fieldsRead, taxon);

	if (taxonLen > 0){
	  if (sscanf(word,"%d:%f",&taxon, &edgeLen) != 2){
	    ERR ("Taxon with edge len is invalid: %s\n",word);
	  }
	}
	else{
	  if (sscanf(word,":%f", &edgeLen) != 1){
	    ERR ("Taxon with edge len is invalid: %s\n",word);
	  }
	}
      }
      else{
	printf("No length was supplied\n");
	if (strspn(word,"0123456789") != wlen){
	  ERR("taxa %s contains non numeric cars!!\n", word);
	}
	taxon = atoi(word);
      }
      
      if (edgeLen > *max_len){
	*max_len = edgeLen;
      }

      if (taxon > *max_taxa){
	*max_taxa = taxon;
      }
      //      printf ("word is %s, taxon %d, edgeLen %f, max_taxa %d\n", 
      //	      word, taxon, edgeLen, *max_taxa);
      word = strtok(NULL, "(),");
    }
  return 1;
}

/*---------------------------------------------------------------------*/



treeNode * newNode(treeNode * father, int newId){
  treeNode  *node = (treeNode *) calloc (sizeof(treeNode),1);
   node -> father = father;
   node -> val = -1;
   node -> len = 0;
   node -> id = newId;
   node -> son = node -> brother = NULL;
   return node;
}


/*---------------------------------------------------------------------*/

/*
  This procedure check that each leaf in leaves points at a real leaf in the tree

*/

void check_leaves(treeNode * leaves[], treeNode *root, int max_taxa){
  int cnt=0, i;

  for (i=0; i < max_taxa; i++){
    if (leaves[i]){
      treeNode *temp = NULL;
      
      for (cnt = 0, temp = leaves[i]; temp->father; temp = temp->father, cnt++){
	if (cnt > max_taxa){
	  ERR("Infinite loop at tree. Reached max_taxa %d while climbing from leaf %d\n",
	      max_taxa,(leaves[i])->val);
	}
      }
      if (temp != root){
	ERR("arrived at a node %d with no father that is not the root %d\n",temp->id, root->id);
      }
    }
  }
}


/*---------------------------------------------------------------------*/



/*---------------------------------------------------------------------*/


/*
  This procedure walks recursively on the tree and checks that every node is OK

*/

void check_tree(treeNode *t, treeNode *f)
{
  //  printf ("IN CHECK TREE. node id :%d, val %d\n",t->id, t-> val);
  if (t->father != f){
    ERR("Node %d does not point to its father %d\n",t->id, f->id);
  }
  if (t->val >= 0)
    {
      if (t->son){
	ERR("leaf %d with id %d has a son %d\n",t->val, t->id,t->son->id); 
      }
    }
  else {
    
    if (t->son){
      if (!(t->son->brother)){
	ERR("Node %d has only one sone %d\n",t->id, t->son->id)
	  }
      check_tree(t->son, t);
    }
    else{
      ERR("Node %d has value %d and no sons\n",t->id, t->val);
    }
  }
  if (t->brother != NULL)
    {
      //      printf("node %d has a brother %d\n",t->id, t->brother->id);
      check_tree(t->brother, f);
    }
  else{
    //    printf("node %d has a NO brother \n",t->id);
  }
}

/*---------------------------------------------------------------------*/


void print_tree_no_length(treeNode *t, int detail)
{
  //  printf ("IN PRINT TREE. node id :%d, val %d\n",t->id, t-> val);
  if (t->val >= 0)
    {
      printf("%d", t->val);
    }
  else {

    if (t->son != NULL){
      printf("(");
      print_tree(t->son, detail);
      //      printf("back from son %d\n",t->son->id);
      printf(")");
    }
    else{
      ERR("Node %d has no value and no sons\n",t->id);
    }
  }
  if (detail) {printf(":[%d]",t->id);}
  if (t->brother != NULL)
    {
      //      printf("node %d has a brother %d\n",t->id, t->brother->id);
      printf(",");
      print_tree(t->brother, detail);
    }
  else{
    //    printf("node %d has a NO brother \n",t->id);
  }
}

/*---------------------------------------------------------------------*/


void print_tree(treeNode *t, int detail)
{
  //  printf ("IN PRINT TREE. node id :%d, val %d\n",t->id, t-> val);
  if (t->val >= 0)
    {
      printf("%d:%2.4f", t->val, t->len);
    }
  else {

    if (t->son != NULL){
      printf("(");
      print_tree(t->son, detail);
      //      printf("back from son %d\n",t->son->id);
      printf("):%2.4f", t->len);
    }
    else{
      ERR("Node %d has no value and no sons\n",t->id);
    }
  }
  if (detail) {printf("[%d]", t->id);}
  if (t->brother != NULL)
    {
      //      printf("node %d has a brother %d\n",t->id, t->brother->id);
      printf(",");
      print_tree(t->brother, detail);
    }
  else{
    //    printf("node %d has a NO brother \n",t->id);
  }
}

/*---------------------------------------------------------------------*/



void print_tree2file(treeNode *t, FILE *ftree)
{
  //  printf ("IN PRINT TREE. node id :%d, val %d\n",t->id, t-> val);
  if (t->val >= 0)
    {
      fprintf(ftree,"%d", t->val);
    }
  else {

    if (t->son != NULL){
      fprintf(ftree, "(");
      print_tree2file(t->son, ftree);
      //      printf("back from son %d\n",t->son->id);
      fprintf(ftree, ")");
    }
    else{
      ERR("Node %d has no value and no sons\n",t->id);
    }
  }
  if (t->brother != NULL)
    {
      //      printf("node %d has a brother %d\n",t->id, t->brother->id);
      fprintf(ftree, ",");
      print_tree2file(t->brother, ftree);
    }
  else{
    //    printf("node %d has a NO brother \n",t->id);
  }
}

/*---------------------------------------------------------------------*/

/*---------------------------------------------------------------------*/


treeNode *getWeightedLeaf(char *tree, int *startInd, int *cnt, treeNode * leaves[])
{
  double len;
  char delim[20] = "(),:";
  int treeInd = *startInd;
  int i= treeInd;
  char temp[MAXTAXALEN];

  treeNode  *leaf = newNode(NULL,(*cnt)++);
  //   printf("in get leaf, starting at %d, char %c\n",
  // 	 treeInd, *(tree+treeInd));
  for (; strchr(delim,*(tree+treeInd)) == NULL; treeInd++);
  //   printf ("found a delimiter %c at %d\n",
  // 	  *(tree+treeInd), treeInd);
  if (getStr(temp, MAXTAXALEN, tree+i, treeInd - i)){
    ERR("Could not allocate space for taxa at offset %d\n",i);
  }
// printf("found leaf %s\n",temp);
  leaf->val = atoi(temp);
  leaves[leaf->val] = leaf;
  if (*(tree+treeInd) == ':'){
    double len = getLen(tree, &treeInd);
    leaf -> len = len;
  }
// printf("returning a leaf with id %d and val %d, len: %f \n",
//	 leaf->id, leaf->val, leaf->len);
  
  *startInd = treeInd;
  return leaf;
    
}



/*---------------------------------------------------------------------*/




treeNode *getLeaf(char *tree, int *startInd, int *cnt, treeNode * leaves[]){
  char *delim = (char *) "(),";
  int treeInd = *startInd;
  int i= treeInd;
  char temp[MAXTAXALEN];

  treeNode  *leaf = newNode(NULL,(*cnt)++);
  //  printf("in get leaf, starting at %d, char %c\n",
  // 	 treeInd, *(tree+treeInd));
  for (; strchr(delim,*(tree+treeInd)) == NULL; treeInd++);
  // printf ("found a delimiter %c at %d\n",
  // 	  *(tree+treeInd), treeInd);
  if (getStr(temp, MAXTAXALEN, tree+i, treeInd - i)){
    ERR("Could not allocate space for taxa at offset %d\n",i);
  }
  // printf("found leaf %s\n",temp);
  leaf->val = atoi(temp);
  leaves[leaf->val] = leaf;
  //  taxa_exists[leaf->val] = 1;
  // printf("returning a leaf with id %d and val %d \n",
  // 	 leaf->id, leaf->val);
  
  *startInd = treeInd;
  return leaf;
    
}

/*---------------------------------------------------------------------*/
int detachSon(treeNode *father, treeNode *son){ 
  //  printf ("In detachSon for father %d and  son %d\n",(father?father->id:-1), son->id);
  treeNode  **temp;
  treeNode *t1;

  if (!father){
    //    printf("Nothing to remove. Son %d has no father. Returning\n",son->id);
    return -1;
  }
  if (father->val != -1){
    printf("ERROR ERROR. Trying to insert a son to a leaf\n");
    exit(4);
  }
  temp = &(father->son);
  //  printf("first son of father %d is %d with val %d\n", father->id, (*temp)->id, (*temp)->val);
  while (*temp != NULL){
    //  printf("Now at son of father %d is %d with val %d\n", father->id, (*temp)->id, (*temp)->val);
    if ((*temp) == son){
      //      printf("Found the son to remove %d\n",(*temp)->id);

      treeNode *keep = *temp;
      *temp = (*temp)->brother;
      if (*temp){
	// printf ("brother of son to remove is %d with val %d\n",(*temp)->id, (*temp)->val);
      }
      else{
	//	printf ("no brother to son %d\n",keep->id);
      }
      //      printf ("bypassing the son to remove: %d\n",(keep)->id);
      keep->brother = keep->father = NULL;

      //      printf("now checking: Father %d has sons:\n",father->id);
      t1=father->son;
      while (t1){
	//	printf("%d, ",t1->id);
	t1 = t1->brother;
      }
      //      printf("\n\nreturning after removing son\n");
      return 0;
    }
    else{
      //      printf("this is not the right son yet\n");
    }
    temp = &((*temp)->brother);
  }
  ERR("son %d was not found among the sons of %d\n", son->id, father ->id);
  return 0;
}


/*---------------------------------------------------------------------*/
void addSon(treeNode *father, treeNode *son){
  //  printf ("in addSon for father %d with existing son %d and a new son %d\n",
  //  father->id, (father->son? father->son->id: -1), son->id);
  treeNode *temp = father->son;
  if (father->val != -1){
    printf("ERROR ERROR. Trying to insert a son to a leaf\n");
    exit(4);
  }
  son->brother = temp;
  son->father = father;
  father->son=son;
  son = NULL;
}




/*---------------------------------------------------------------------*/


treeNode *insertWeightedSubtree(char *tree, int *pTreeInd, int *pCnt, treeNode * leaves[]){
  treeNode *father = newNode (NULL,(*pCnt)++);
  treeNode *son;
  //  printf ("entered insert subtree. Allocated a father with id %d\n", (*pCnt)-1);
  while (*(tree  + *pTreeInd) != ')'){
    //    printf("looking at char %d: %c\n",*pTreeInd,*(tree  + *pTreeInd));
    if (*(tree  + *pTreeInd) == '('){
      (*pTreeInd)++;
      son =  insertWeightedSubtree(tree, pTreeInd, pCnt, leaves);
      //   printf ("returnd to subtree loop after finding son %d."
      //	      " char now is %c\n",son->id, *(tree  + *pTreeInd));
      if (*(tree  + *pTreeInd) == ':'){
	double len = getLen(tree, pTreeInd);
	son->len = len;
      }

    }
    else if (*(tree  + *pTreeInd) == ','){
      //  printf("found , inserting son with root id %d and val %d"
      //     " under father %d\n",   son->id, son->val, father->id);
      (*pTreeInd)++;
      addSon(father, son);
      son=NULL;
      if (son != NULL){
      printf("ERROR ERROR: son is not NULL after insert %d\n", son->id);
      exit(4);
      }
    }
    else if ((*(tree  + *pTreeInd) >= '0') && (*(tree  + *pTreeInd) <= '9')){
      // printf ("found a start of a name. inserting a leaf\n");
      son = getWeightedLeaf(tree, pTreeInd, pCnt, leaves);
    }
    else{
      printf("ERROR ERROR: invalid character %c\n",*(tree  + *pTreeInd));
      exit(4);
    }
  }
//printf ("exited from insertSubtree loop. char is now %c\n",
//	  *(tree  + *pTreeInd));
  addSon(father, son);
  (*pTreeInd)++;
  return father; 
}


/*---------------------------------------------------------------------*/

treeNode *insertSubtree(char *tree, int *pTreeInd, int *pCnt, treeNode * leaves[]){
  treeNode *father = newNode (NULL,(*pCnt)++);
  treeNode *son;
  // printf ("entered insert subtree. Allocated a father with id %d\n", (*pCnt)-1);
  while (*(tree  + *pTreeInd) != ')'){
    // printf("looking at char %d: %c\n",*pTreeInd,*(tree  + *pTreeInd));
    if (*(tree  + *pTreeInd) == '('){
      (*pTreeInd)++;
      son =  insertSubtree(tree, pTreeInd, pCnt, leaves);
      // printf ("returnd to subtree loop after finding son %d."
      //	      " char now is %c\n",son->id, *(tree  + *pTreeInd));
    }
    else if (*(tree  + *pTreeInd) == ','){
      //  printf("found , inserting son with root id %d and val %d"
      //     " under father %d\n",   son->id, son->val, father->id);
      (*pTreeInd)++;
      addSon(father, son);
      son=NULL;
      if (son != NULL){
	printf("ERROR ERROR: son is not NULL after insert %p\n", son);
      exit(4);
      }
    }
    else if ((*(tree  + *pTreeInd) >= '0') && (*(tree  + *pTreeInd) <= '9')){
      // printf ("found a start of a name. inserting a leaf\n");
      son = getLeaf(tree, pTreeInd, pCnt, leaves);
    }
    else{
      printf("ERROR ERROR: invalid character %c\n",*(tree  + *pTreeInd));
      exit(4);
    }
  }
// printf ("exited from insertSubtree loop. char is now %c\n",
//	  *(tree  + *pTreeInd));
  addSon(father, son);
  (*pTreeInd)++;
  return father; 
}

/*---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*/



treeNode * parseTree(char * tree, int max_taxa, treeNode ***p_leaves, int *cnt){
  treeNode * root;
  int i=0, vec_size;
  treeNode * *leaves = NULL;
  int *taxa_exists;
  int treeInd = 0;
  if (leaves != NULL){
    free (leaves);
  }

  vec_size = (max_taxa+4 > strlen(tree)? max_taxa+4: strlen(tree));
  // printf("vec sizes is %d\n",vec_size);
  taxa_exists = (int *) calloc (sizeof (int),vec_size);
  for (i=0; i < vec_size; i++){
    taxa_exists[i] = 0;
  }
  leaves = (treeNode **) calloc (sizeof (treeNode *),vec_size);
  *p_leaves = leaves;

  // printf ("\n\nin parse Tree. Tree is %s\n\n",tree);
  if (*tree == '('){
    treeInd++;
    root = insertWeightedSubtree(tree, &treeInd, cnt, leaves);
  }
  else {
    printf ("ERROR ERROR: invalid tree %s\n",tree);
    exit (8);
  }
  //  printf("\n\nthe tree read:\n");
  // print_tree(root,1);
  printf ("\n\nnumber of nodes created %d\n",*cnt);
  
  //  my_free(taxa_exists);
  //  my_free(leaves);

  return root;
}


/*---------------------------------------------------------------------*/


treeNode * parseTreeOld(char * tree, int max_taxa, treeNode ***p_leaves, int *cnt){
  treeNode * root;
  int i=0, vec_size;
  treeNode * *leaves = NULL;
  int *taxa_exists;
  int treeInd = 0;
  if (leaves != NULL){
    free (leaves);
  }

  vec_size = (max_taxa+4 > strlen(tree)? max_taxa+4: strlen(tree));
  // printf("vec sizes is %d\n",vec_size);
  taxa_exists = (int *) calloc (sizeof (int),vec_size);
  for (i=0; i < vec_size; i++){
    taxa_exists[i] = 0;
  }
  leaves = (treeNode **) calloc (sizeof (treeNode *),vec_size);
  *p_leaves = leaves;

  // printf ("\n\nin parse Tree. Tree is %s\n\n",tree);
  if (*tree == '('){
    treeInd++;
    root = insertSubtree(tree, &treeInd, cnt, leaves);
  }
  else {
    printf ("ERROR ERROR: invalid tree %s\n",tree);
    exit (8);
  }
  //printf("\n\nthe tree read:\n");
  //print_tree(root,1);
  printf ("\n\nnumber of nodes created %d\n",*cnt);
  
  //  my_free(taxa_exists);
  //  my_free(leaves);

  return root;
}



/*---------------------------------------------------------------------*/

int binary_root(char *tree){
  int i, balance =0, count;
  int bin = 1;
  // printf("at binary tree, tree %s\n", tree);
  
  for (count = 0, balance =1, i = 1; i < strlen(tree)-1; i++){
    // printf ("%c, bal %d\n",tree[i],balance);
    switch(tree[i])
      {
      case '(': balance++; break;
      case ')': balance--; 
	if (balance == 0){
	  printf("ERROR ERROR ERROR: invalid tree %s\n",tree);
	  exit(8);
	}
	break;
      case ',':
	if (balance==1){
	  count++;
	  if (count > 1){
	    printf("Tree is not binary");
	    bin= 0;
	  }
	}
	break;
      }
  }
  if (balance != 1){
	  printf("ERROR ERROR ERROR: invalid tree %s\n",tree);
	  exit(8);
  }
  return bin;
}



/*---------------------------------------------------------------------*/

int contract(treeNode *node){
  //  printf("\nIn contract for node id %d\n",node->id);
  treeNode *father;
  treeNode *son;
  if (!node->son){
    ERR(" Node has NO son and can't be contracted\n");
  }
  if (node->son->brother){
    //    printf(" Node %d has more than one son and can't be contracted\n",node->id);
    return 1;
  }

  father = node->father;
  son = node->son;
  detachSon(father, node);
  free(node);
  addSon(father,son);
  return 0;
}
/*---------------------------------------------------------------------*/
int merge_leaves(treeNode **leaves1, treeNode **leaves2, int max_taxa){
 
  int i=0;
  // printf("\nIn merge leaves\n");
  
  for (i =0; i < max_taxa +1; i++){
    if (leaves1[i] && leaves2[i]){
      ERR("Leaf %d appears in both trees\n",i);
    }
    if  (leaves2[i]){
      leaves1[i] =  leaves2[i];
    }
  }

  return 0;
}

/*---------------------------------------------------------------------*/


treeNode *find_leaf(treeNode *t, int leaf){
  //  printf ("IN FIND LEAF for leaf %d. node id :%d, val %d\n",leaf, t->id, t-> val);
  if (t->val >= 0)
    {
      if (leaf ==  t->val){
	//	printf("\nLeaf %d found!!!\n\n", t->val);
	return t;
      }
    }
  else {

    if (t->son != NULL){
      treeNode *temp =  find_leaf(t->son, leaf);
      if (temp){
	//	printf ("Leaf %d was found at son\n", leaf);
	return temp;
      }
      //      printf("back from son %d\n",t->son->id);
    }
    else{
      ERR("Node %d has no value and no sons\n",t->id);
    }
  }
  if (t->brother != NULL)
    {
      //      printf("node %d has a brother %d\n",t->id, t->brother->id);
      return find_leaf(t->brother, leaf);
    }
  else{
    //    printf("node %d has a NO brother \n",t->id);
    return NULL;
  }
}


/*---------------------------------------------------------------------*/

treeNode *reroot_tree( treeNode *outg_node, treeNode *root, int *pCnt){


  treeNode *new_root = newNode (NULL,(*pCnt)++);
  treeNode  *last_f;
  treeNode *father;
  int keep;

  if (!outg_node){
    ERR("No outg node supplied!!\n");
  }
  
  father = outg_node -> father;
  
  detachSon(father, outg_node);
  /*
  printf ("after removing son %d\n",outg_node->id);
  printf ("going to add the outg %d as a son of the new root %d\n",
	  outg_node->id , new_root->id);
  */
  addSon(new_root, outg_node);
  //  printf("son %d of new root %d was added\n",new_root->son->id,new_root->id);
  last_f = new_root;

  //  printf("\nEntering the loop with father %d, last_f %d\n",
  //father->id, last_f->id);
  while (father != NULL){
    treeNode *next_f = father -> father;
    /*
    printf("\nIn the loop with last_f %d, father %d and  his current father %d\n",
	   last_f->id,father->id,(next_f? next_f->id:-1));
    printf("going to remove father %d from his current father %d\n",
	   father->id,(next_f? next_f->id:-1));
    */
    detachSon(next_f,father);
    addSon(last_f,father);
    last_f = father;
    father = next_f;
  }
  //  printf("\n\nAFTER THE LOOP. Now checking if last inverted node %d (original root) should bbe contracted\n",
  // last_f->id);
  if (!last_f->son){
    ERR("Last node in the loop %d has no son\n",last_f->id);
  }
  if (last_f->son->brother){
    //    printf("Original root %d was NOT binary and now has more than one son. No need to contract it\n",last_f->id);
  }
  else{
    //    printf("Original root %d was binary. Going to contract it\n",last_f->id);
    keep = last_f->id;
    contract(last_f);
    //    printf("Reusing the contracted node id %d\n",keep);
    new_root->id = keep;
    (*pCnt)--;
  }
  return new_root;

}


/*---------------------------------------------------------------------*/

treeNode *join_trees(treeNode *root0, int outg0, treeNode *root1, int outg1, int max_taxa, int *pCnt){
  
  //  printf("\n\nAt join Trees with two outg1 %d and %d\n",
  // outg0, outg1);


  treeNode *outg_node0 = find_leaf(root0, outg0);
  treeNode *outg_node1 = find_leaf(root1, outg1);
  treeNode *f1;
  treeNode *f0;
  treeNode *new_root;
  int reuse_id;

  if (!outg_node0){
    ERR("outg0 %d does not appear in leaves of tree 0\n",outg0);
  }

  if (!outg_node1){
    ERR("outg1 %d does not appear in leaves of tree 0\n",outg1);
  }


  f0 = reroot_tree( outg_node0, root0, pCnt);
  if (f0 !=  (outg_node0)->father){
    ERR("Root returned from reroot for tree 0 is NOT outg0 %d father\n",outg0);
  }
  
  f1 = reroot_tree( outg_node1, root1, pCnt);
  if (f1 !=  (outg_node1)->father){
    ERR("Root returned from reroot for tree 1 is NOT outg1 %d father\n",outg1);
  }



  if (f0->father){
    ERR("father of outg0 %d is not a root\n", outg0);
  }

  if (f1->father){
    ERR("father of outg1 %d is not a root\n", outg1);
  }

  detachSon(f0, outg_node0);
  reuse_id = outg_node0-> id;
  free(outg_node0);


  detachSon(f1, outg_node1);
  free(outg_node1);


  new_root = newNode (NULL,reuse_id);
  addSon(new_root, f0);
  addSon(new_root, f1);

  contract(f0);
  contract(f1);

  //  int rc = merge_leaves(leaves1, leaves2, max_taxa );
  //  if (!rc){
  //    printf ("Leaves merged OK. Freeing leaves2\n");
  //    my_free(leaves2);
  //  }
  /* Now checking that the merged tree is OK */
  //  check_leaves(leaves1, new_root, max_taxa);
  check_tree(new_root, NULL);
  //  printf ("tree is OK\n\n");
  return new_root;
}


/*---------------------------------------------------------------------*/

treeNode *readTree(char *treeFname, treeNode ***leaves, int ntaxa){
  FILE *treeFile;
  char * tree;
  int cnt = 0;
  struct trpStruct * prev, * current;
  treeNode * t;
  long i;
  printf ("Reading tree %s \n",treeFname);
  treeFile = fopen(treeFname, "r");
  if (treeFile == NULL){
    printf ("open for %s failed\n",treeFname);
    exit (0);
  }
  
  tree= (char *) malloc(200000);
  if (fgets(tree,200000,treeFile) == NULL) {
    printf ("could not read the tree file %s\n",treeFname);
    exit (3);
  }
  fclose(treeFile);
  // printf ("tree: %s\n",tree);
  if (*(tree+strlen(tree)-1) == '\n'){
    // printf ("tree %s ends ends with newline\n",tree);
    *(tree+strlen(tree)-1) = '\0';
    //printf ("newtree %s does not.\n",tree);
  }
  t = parseTree(tree,ntaxa,leaves,&cnt);
  printf ("Finish reading tree %s \n",treeFname);
  free(tree);
  return t;
}

/*---------------------------------------------------------------------*/
/*
 The function gets a tree root and returns spcArry with the tree taxa,
 and the number of taxons in the tree
*/

int getTreeTaxa(treeNode *t, int **spcArray,int nTreeTaxa)
{
  //  printf ("in get tree taxa. node id :%d, val %d\n",t->id, t-> val);
  fflush(stdout);
  int nodeValue = t->val;
  
  if (nodeValue >= 0)
    {      
      (*spcArray)[nTreeTaxa] = nodeValue;
      nTreeTaxa++;
    }
  else {    
    if (t->son != NULL){
      nTreeTaxa = getTreeTaxa(t->son,spcArray,nTreeTaxa);
    }
    else{
      ERR("Node %d has no value and no sons\n",t->id);
    }
  }
  
  if (t->brother != NULL)
    {
      //      printf("node %d has a brother %d\n",t->id, t->brother->id);
      nTreeTaxa = getTreeTaxa(t->brother,spcArray,nTreeTaxa);
    }
  else{
    //    printf("node %d has a NO brother \n",t->id);
  }
  return(nTreeTaxa);
}


/*---------------------------------------------------------------------*/
float * genMatFromTree(treeNode **leaves, int spcArray[], int ntaxa, int realDist)
{
  float *distMat=  (float *) calloc (ntaxa * ntaxa, sizeof(float) ); 
  int i,j;
  printf("Running gen mat %d\n",realDist);
  for(i=0;i<ntaxa;i++)
    for(j=0;j<ntaxa;j++)
      {
	if(i == j) 
	  continue;
	float dist;
	
	if(realDist)
	  dist = (float) real_pairwise_dist(leaves,spcArray[i],spcArray[j]);
	else
	  {
	  dist = (float) topological_pairwise_dist(leaves,spcArray[i],spcArray[j]);
	  }
	distMat[i*ntaxa+j] = (float) dist;
      }
  return(distMat);
}

/*---------------------------------------------------------------------*/

int main1 (int argc, char *argv[]){
  
  int max_taxa=-1;
  int cnt = 0;
  double max_len =-1;
  treeNode * root0;
  treeNode *root1;
  treeNode * new_root;
  treeNode * new_root1;
  treeNode **leaves0 = NULL;
  treeNode **leaves1 = NULL;
  treeNode *outg_node;

  int keep;
  char tree1[300] = "(((10,11),(12,13),14),(15,16,17))";
  char tree0[300] = "((1,2),(3,(4,5),(6,7)))";
  int outg;

	if (argc != 2){
    printf ("ERROR ERROR: must supply one parameter - a Newick tree\n");
    printf ("Usage: %s <TREE>\n",argv[0]);
    exit(6);
  }
  
 
  if (binary_root(argv[1])){
    printf("Tree %s has binary root\n\n",argv[1]);
  }
  else{
        printf("Tree %s has NO binary root\n\n",argv[1]);
  }

  valid_tree(tree0, &max_taxa, &max_len);
  printf("tree %s is valid and max taxa is %d\n\n", tree0, max_taxa);
  keep = max_taxa;

  
  valid_tree(tree1, &max_taxa, &max_len);
  printf("tree %s is valid and max taxa is %d\n\n", tree1, max_taxa);
  if (keep > max_taxa){
    max_taxa = keep;
  }

  root0 = parseTree(tree0, max_taxa, &leaves0, &cnt);
  
  //  printf("id of leaf 5 is %d and his father %d\n", (leaves[5])->id,  (leaves[5])->father->id);
  //  exit(9);
  //  if (detachSon((leaves[5])->father, (leaves[9]))){
  //    ERR("Could not remove sone %d of father %d\n",  (leaves[5])->id,  (leaves[5])->father->id);
  //  }
  print_tree(root0,0);

  root1 = parseTree(tree1, max_taxa, &leaves1, &cnt);

  print_tree(root1,0);

  
  new_root = join_trees(root0,  5, root1, 16, max_taxa, &cnt);
  
  print_tree(new_root,0);
  printf("\n\n");
  exit (8);

  printf ("\n\nenter an outgroup\n");
  
  scanf("%d\n",&outg);
  printf ("outgroup leaf %d\n",outg);
  
  outg_node = find_leaf(root0, outg);
  if (!outg_node){
    ERR(" no leaf with val %d\n",outg);
  }
  
  new_root1 = reroot_tree(outg_node, root0, &cnt);
    
  printf ("after rerooting for leaf %d, tree:\n\n",outg);
  print_tree(new_root1,1);
  printf("\n\n");
  print_tree(new_root1,0);
  printf("\n\n");
 

  
}

