#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
int ntaxa;
int ntriplets;

#define my_printf // printf


#define ERR(...) {printf("ERROR RROR ERROR: "__VA_ARGS__); exit(8);}

typedef struct qrtStruct {
  int species[4];
  struct qrtStruct * next;
}qrtNode;

typedef struct tree_node {int val;
  int id;
struct tree_node *father;
  struct tree_node *brother;
  struct tree_node  *son;} treeNode;
  
int mark_parent(treeNode *s,int i, int level);

long  treeInd = 0, cnt = 0, visited =0, n_quartets=0, n_species = 0;
long best_score = 0, max_taxa = 0;
qrtNode *anchor;
int *taxa_exists;
treeNode **leaves; 
/*---------------------------------------------------------------------*/

char *  getStr( char * from, int tokLen){
	char * to;
	to = (char *) malloc(tokLen+1);
	memcpy(to, from, tokLen);
	*(to+tokLen)='\0';
	return to;
}
/*---------------------------------------------------------------------*/

treeNode * lca(int leaf1, int leaf2){
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
  
  printf("\nfather of leaf %d with id %d, and of leaf %d with id %d"
	 " is node with id %d\n", 
	 leaf1,leaves[leaf1]->id,leaf2,leaves[leaf2]->id,node->id);
    return node;
}
/*---------------------------------------------------------------------*/


treeNode * newNode(treeNode * father, int newId){
   treeNode  *node = calloc (sizeof(treeNode),1);
   node -> father = father;
   node -> val = -1;
   node -> id = newId;
   node -> son = node -> brother = NULL;
   return node;
}

/*---------------------------------------------------------------------*/

treeNode *getLeaf(char *tree){
  char *delim = "(),";
  int i= treeInd ,num;
  char * temp;

  treeNode  *leaf = newNode(NULL,cnt++);
  //  printf("in get leaf, starting at %d, char %c\n",
  //	 treeInd, *(tree+treeInd));
  for (; strchr(delim,*(tree+treeInd)) == NULL; treeInd++);
  //  printf ("found a delimiter %c at %d\n",
  //	  *(tree+treeInd), treeInd);
  temp =(char *) getStr(tree+i, treeInd - i);
  //  printf("found leaf %s\n",temp);
  leaf->val = atoi(temp);
  leaves[leaf->val] = leaf;
  taxa_exists[leaf->val] = 1;
  free(temp);
  //  printf("returning a leaf with id %d and val %d \n",
  //	 leaf->id, leaf->val);
  
  return leaf;
    
}





/*---------------------------------------------------------------------*/
void addSon(treeNode *father, treeNode *son){
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

treeNode *insertSubtree(char *tree){
  treeNode *father = newNode (NULL,cnt++);
  treeNode *son;
  //  printf ("entered insert subtree. Allocated a father with id %d\n", cnt-1);
  while (*(tree  + treeInd) != ')'){
    //    printf("looking at char %d: %c\n",treeInd,*(tree  + treeInd));
    if (*(tree  + treeInd) == '('){
      treeInd++;
      son =  insertSubtree(tree);
      //      printf ("returnd to subtree loop after finding son %d."
      //	      " char now is %c\n",son->id, *(tree  + treeInd));
    }
    else if (*(tree  + treeInd) == ','){
      //      printf("found , inserting son with root id %d and val %d"
      //	     " under father %d\n",   son->id, son->val, father->id);
      treeInd++;
      addSon(father, son);
      son=NULL;
      if (son != NULL){
      printf("ERROR ERROR: son is not NULL after insert %p\n", son);
      exit(4);
      }
    }
    else if ((*(tree  + treeInd) >= '0') && (*(tree  + treeInd) <= '9')){
      //      printf ("found a start of a name. inserting a leaf\n");
      son = getLeaf(tree);
    }
    else{
      printf("ERROR ERROR: invalid character %c\n",*(tree  + treeInd));
      exit(4);
    }
  }
  //  printf ("exited from insertSubtree loop. char is now %c\n",
  //	  *(tree  + treeInd));
  addSon(father, son);
  treeInd++;
  return father; 
}

/*---------------------------------------------------------------------*/

void get_max_taxa(char *tree){
  int ind, taxa;
  int lastInd=0;
  char  tok [100];
  max_taxa = 0;
  for (ind = 0; ind < strlen(tree); ind++){
    if ((*(tree+ind) == '(') ){
      lastInd = ind+1;
    }
    else if ((*(tree+ind) == ',') || (*(tree+ind) == ')')){
      int tokLen = ind - lastInd; 
      if (tokLen  > 0){
	strncpy(tok,(tree+lastInd), tokLen);	
	*(tok+tokLen) = '\0';	
	//	printf ("found a token at position %d: %s\n",ind,tok);
	taxa = atoi(tok);
	if (taxa > max_taxa){
	  //	  printf ("found a bigger taxa than current %d: %d\n",max_taxa,taxa);
	  max_taxa = taxa;
	}
      }
      lastInd = ind+1;
    }
  }
  printf("max taxa is %d\n\n",max_taxa);
}


/*---------------------------------------------------------------------*/

treeNode * parseTree(char * tree){
  treeNode * root;
  int i=0, vec_size;
  cnt=0;
  
  treeInd = 0;
  if (leaves != NULL){
    free (leaves);
  }
  get_max_taxa(tree);
  vec_size = 2*(max_taxa > strlen(tree)? max_taxa: strlen(tree));
  printf("vec sizes is %d\n",vec_size);
  taxa_exists = (int *) calloc (sizeof (int),vec_size);
  for (i=0; i < vec_size; i++){
    taxa_exists[i] = 0;
  }
  leaves = (treeNode **) calloc (sizeof (treeNode *),vec_size);
  printf ("\n\nin parse Tree. Tree is %s\n\n",tree);
  if (*tree == '('){
    treeInd++;
    root = insertSubtree(tree);
  }
  else {
    printf ("ERROR ERROR: invalid tree %s\n",tree);
    exit (8);
  }
  printf("\n\nthe tree read:\n");
  print_tree(root);
  return root;
}

/*---------------------------------------------------------------------*/



/*---------------------------------------------------------------------*/
int randInt(int above)
{
  int mid;
  mid = rand();
  mid = mid/(RAND_MAX/above);
  if (mid <= 0)
    {
      mid = mid+1;
    }
  else if (mid ==above)
    {
      mid = mid -1;
    }
    
  return mid;
}


/*---------------------------------------------------------------------*/

/*---------------------------------------------------------------------*/

print_tree(treeNode *t)
{
  //  printf ("IN PRINT TREE. node id :%d, val %d\n",t->id, t-> val);
  if (t->val >= 0)
    {
      printf("%d", t->val);
    }
  else {

    if (t->son != NULL)
      printf("(");
      print_tree(t->son);
      t = t->son->brother;
      while (t != NULL)
      {
	printf(",");
	print_tree(t);;
	t = t->brother;
      }
      printf(")");
  }
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

int find_triplet_outgroup(int i, int j, int k)
{
  int counti,countk,countj;
  //  printf ("i %d, j %d, k %d\n", i,j,k);

  //  printf ("flip %d\n", flip);
  if (i == j || j == k || i == k) exit ( -1);
  if ( !taxa_exists[i] || !taxa_exists[j] || !taxa_exists[k]) {
    printf ("One of the taxa %d, %d or %d does not exists\n", i,j,k);
    exit (-1);
  }

  //  printf ("All the taxa %d, %d or %d  exist\n", i,j,k);
 

  mark_parent(leaves[i]->father,i,0);
  counti = mark_parent(leaves[j]->father,j,0);

  mark_parent(leaves[i]->father,-1,0);
  mark_parent(leaves[j]->father,-1,0);
  
  mark_parent(leaves[k]->father,k,0);
  countk = mark_parent(leaves[j]->father,j,0);

  mark_parent(leaves[k]->father,-1,0);
  mark_parent(leaves[j]->father,-1,0);
  //  printf ("i %d, j %d, k %d, counti %d, count k %d\n",i,j,k,counti, countk);

  if (counti == countk)
    {
      mark_parent(leaves[i]->father,i,0);
      counti = mark_parent(leaves[k]->father,k,0);
      
      mark_parent(leaves[k]->father,-1,0);
      mark_parent(leaves[i]->father,-1,0);
      
      mark_parent(leaves[j]->father,j,0);
      countj = mark_parent(leaves[k]->father,k,0);
      
      mark_parent(leaves[k]->father,-1,0);
      mark_parent(leaves[j]->father,-1,0);
      if (counti == countj){
	printf ("(%d,%d,%d)\n", i,k,j);
	printf ("ERROR ERROR. such a rtiplet should NOT occur\n");
	return -1;
	//	exit (6);
      }
      else {
	//      printf("((%d,%d),%d)\n", i,k,j);
	//      trp[0] = i; trp[1] = k; trp[2] = j;
      return j;
      //      fprintf(trpF, "%d,%d|%d\n", i,k,j);
      }
    }
  else if (counti < countk)
    {
      //      printf("((%d,%d),%d)\n", i,j,k);
      //      trp[0] = i; trp[1] = j; trp[2] = k;
      return k;
      //      fprintf(trpF, "%d,%d|%d\n", i,j,k);
      }
  else if (counti > countk)
    {
      //      printf("((%d,%d),%d)\n", j,k,i);
      //      trp[0] = j; trp[1] = k; trp[2] = i;
      return i;
      //      fprintf(trpF, "%d,%d|%d\n", j,k,i);
    }
      
}



/*---------------------------------------------------------------------*/
int check_quartet(int i, int j, int k, int l){
  int out1, out2, out3;
  
  my_printf("\nin check quartet for %d, %d, %d, %d\n",i,j,k,l);
  out1 = find_triplet_outgroup(i,j,k);
  if (out1 != i && out1 != j && out1 != k){
    if (out1 == -1){
      my_printf("A polytomy for %d, %d, %d\n",i,j,k);
      out1 = find_triplet_outgroup(l,j,k);
      if (out1 == -1){
	my_printf("A polytomy for %d, %d, %d\n",l,j,k);
	out1 = find_triplet_outgroup(l,j,i);
	if (out1 == -1){
	  printf("A polytomy for %d, %d, %d. no quartet exists.\n",l,j,i);
	  printf("A polytomy for %d, %d, %d, %d. no quartet exists.\n",i,j,k,l);
	  return (0);      
	}
	else{ // out1 MUST BE j
	  if (out1 == j){
	    my_printf ("quartet is %d,%d|%d,%d\n",l,i,j,k);
	    return 0;
	  }
	  else{
	    ERR("Illegel quartet for  %d,%d|%d,%d\n",i,j,l,k);
	  }
	}
      }
      else{ // out1 is either k,j,l
	if (out1 == k){
	  my_printf ("quartet is %d,%d|%d,%d\n",l,j,i,k);
	  return 0;
	}
	else{
	  if (out1 == l){
	    my_printf ("No quartet for  %d,%d,%d,%d\n",k,j,l,i);
	    return 0;
	  }
	  else{
	    if (out1 == j){
	      my_printf ("quartet is %d,%d|%d,%d\n",k,l,j,i);
	      return 1;
	    }
	    else {
	      printf ("ERROR ERROR. invalid out1 %d\n", out1);
	      exit (8);
	    }
	  }
	}
      }
    }
    printf ("ERROR ERROR. invalid out1 %d\n", out1);
    exit (8);
  }
  my_printf ("out1 is %d\n",out1);
  if (out1 == i){
    //    printf ("first triplet is %d,%d|%d\n",j,k,i);
    out2 = find_triplet_outgroup(k,j,l);
    if (out2 != l && out2 != j && out2 != k){
      if (out2 == -1) { return 0;}
      printf ("ERROR ERROR. invalid out2 %d\n", out2);
      exit (8);
    }
    if (out2 == j){
      //      printf ("second triplet is %d,%d|%d\n",k,l,j);
      my_printf ("quartet is %d,%d|%d,%d\n",l,k,j,i);
      return 1;
    }
    if (out2 == k){
      //      printf ("second triplet is %d,%d|%d\n",j,l,k);
      my_printf ("quartet is %d,%d|%d,%d\n",j,l,k,i);
      return 0;
    }
    if (out2 == l){
      //      printf ("second triplet is %d,%d|%d\n",j,k,l);
      my_printf ("quartet is %d,%d|%d,%d\n",j,k,l,i);
      return 0;
    }
  }
  if (out1 == j){
    //    printf ("first triplet is %d,%d|%d\n",k,i,j);
    out2 = find_triplet_outgroup(k,i,l);
    if (out2 != l && out2 != i && out2 != k){
      if (out2 == -1) { return 0;}
      printf ("ERROR ERROR. invalid out2 %d\n", out2);
      exit (8);
    }
    if (out2 == i){
      //      printf ("second triplet is %d,%d|%d\n",k,l,i);
      my_printf ("quartet is %d,%d|%d,%d\n",l,k,i,j);
      return 1;
    }
    if (out2 == k){
      //      printf ("second triplet is %d,%d|%d\n",i,l,k);
      my_printf ("quartet is %d,%d|%d,%d\n",i,l,k,j);
      return 0;
    }
    if (out2 == l){
      //      printf ("second triplet is %d,%d|%d\n",i,k,l);
      my_printf ("quartet is %d,%d|%d,%d\n",i,k,l,j);
      return 0;
    }
  }
  if (out1 == k){
    //    printf ("first triplet is %d,%d|%d\n",j,i,k);
    out2 = find_triplet_outgroup(i,j,l);
    if (out2 != l && out2 != j && out2 != i){
      if (out2 == -1) { return 0;}
      printf ("ERROR ERROR. invalid out2 %d\n", out2);
      exit (8);
    }
    if (out2 == j){
      //      printf ("second triplet is %d,%d|%d\n",i,l,j);
      my_printf ("quartet is %d,%d|%d,%d\n",l,i,j,k);
      return 0;
    }
    if (out2 == i){
      //      printf ("second triplet is %d,%d|%d\n",j,l,i);
      my_printf ("quartet is %d,%d|%d,%d\n",j,l,i,k);
      return 0;
    }
    if (out2 == l){
      //      printf ("second triplet is %d,%d|%d\n",j,i,l);
      my_printf ("quartet is %d,%d|%d,%d\n",j,i,l,k);
      return 1;
    }
  }

}

/*---------------------------------------------------------------------*/


long  countAgree (){
  long  i=0;
  struct qrtStruct * current;
  long agree =0, disagree=0, res;

  //  printf("tree: %s\n",tree);
  printf ("At count agree\n");
  //  exit(7);

  for (i=1, current = anchor; current != NULL;i++, current = current -> next){
    if((i%100000) == 0){
      printf("After %ld quartets: agree %ld, disagree %ld\n",i,agree,disagree);
      //      exit(8);
    }
    my_printf ("quartet %d: %d,%d|%d,%d\n",i,
	    current->species[0],
	    current->species[1],
	    current->species[2],
	    current->species[3]);    
    if ((res = check_quartet(current->species[0],
			    current->species[1],
			    current->species[2], 
			    current->species[3])) != -1){
      //      printf("response from check quartet %ld: %d. agree %ld\n",i,res, agree, disagree);
      if (res){
	agree++;
      }
      else {
	disagree++;
      }
    }
    else {
      my_printf("response %d from check_triplet, for triplet number %d: %d,%d|%d\n",res,i,   current->species[0],
	     current->species[1],  current->species[2]);
      return -1;
    }

    //    printf ("quartet %ld: %d,%d|%d,%d.  agree: %ld,  disagree %ld\n",i, 
    //	    current->species[0],  current->species[1],current->species[2],current->species[3],agree, disagree);
  } 
  printf("Score for currentTree: agree %d,  disagree %d\n", agree, disagree);
  return agree;
  
}

/*---------------------------------------------------------------------*/


int check_triplet(struct qrtStruct *trpl){
  int i = trpl->species[0],
    j = trpl->species[1],
    k = trpl->species[2];    
  int counti,countk,countj;
  
  if ((leaves[i] == NULL ) ||  (leaves[j] == NULL ) ||  (leaves[k] == NULL ) ) {
    printf("\n\none of leaves %d, %d, %d is missing\n",i,j,k);
    return -1;
  }
  
  if (mark_parent(leaves[i]->father,i,0) == -1) {
    printf ("invalid tree\n");
    return -1;
  }
  if ((counti = mark_parent(leaves[j]->father,j,0)) == -1) {
    printf ("invalid tree\n");
    return -1;
  }
  
  mark_parent(leaves[i]->father,-1,0);
  mark_parent(leaves[j]->father,-1,0);
  
  if (mark_parent(leaves[k]->father,k,0) == -1) {
    printf ("invalid tree\n");
    return -1;
  }
  if ((countk = mark_parent(leaves[j]->father,j,0)) == -1) {
    printf ("invalid tree\n");
    return -1;
  }
  
  mark_parent(leaves[k]->father,-1,0);
  mark_parent(leaves[j]->father,-1,0);
  //  printf ("i %d, j %d, k %d, counti %d, count k %d\n",i,j,k,counti, countk);
  
  if (counti < countk)      {
    //    printf("triplet ((%d,%d),%d) agrees\n", i,j,k);
    return  1;
  }
  else {
    //    printf("triplet ((%d,%d),%d) does NOT agree\n", i,j,k);
    return  0;
  }
  
}


/*---------------------------------------------------------------------*/

void checkTree(treeNode * root){
  if (root->son !=NULL){
    treeNode * node = root->son;
    if  (root->val != -1){
      printf("ERROR ERROR. Node % is not a leaf but has value %d\n",
	     node->id, node->val);
      exit(6);
    }

    if (node->brother == NULL && root->father == NULL){
      printf("the root node %d has a single son\n",root->id);
    }
    if (node->brother == NULL && root->father != NULL){
      printf("ERROR ERROR. Node %d has a single son\nsubtree is",root->id);
      print_tree(root);
      exit(6);
    }
    while (node != NULL){
      if (node->father != root){
	printf("ERROR ERROR. Node % does not point to its father %p\n",
	       node->id,root);
	exit(6);
      }
      checkTree(node);
      node = node->brother;
    }
  }
  else{
    //    printf("\narrived at a leaf %d\n",root->val);
  }
}
/*---------------------------------------------------------------------*/

/*---------------------------------------------------------------------*/

void	parseParms(int argc, char *argv[], char ** treeFname, char **qrtFname){
	int i;
	*qrtFname = *treeFname = NULL;
	printf ("at parse parm. argc %d\n",argc);
	for (i=1; i< argc; i++){
		printf ("parm %d is %s\n",i,argv[i]);
		//	 return;
		if (! strncmp(argv[i],"qrtt=",5)){
			printf("found qrtt=%s\n", (argv[i]+5));
			*qrtFname = argv[i]+5;
			//strcpy(trpFname, argv[i]+5);
		}
		if (! strncmp(argv[i],"tree=",5)){
			printf("found tree=%s\n", (argv[i]+5));
			*treeFname = argv[i]+5;
			//strcpy(treeFname, argv[i]+5);
		}
	}
	if (*qrtFname == NULL || *treeFname == NULL){
		printf("ERROR !!!!!  Invalid input");
		printf("\nUsage: %s   tree=<tree-file> qrtt=<quartets-file>\n", argv[0]);
		exit(2);
	}

}
/*---------------------------------------------------------------------*/

int get_n_species (char * currentTree){
  int ind=0, lastInd =0; 
  int currentTreeLen = strlen(currentTree);
  char tok [100];
  char *delim="(),";
  
  //  printf ("checking fro triplet %s,%s|%s\n",
  //	  tr->species[0],
  //	  tr->species[1],
  //	  tr->species[2]);
  for (ind = 0;ind < currentTreeLen ; ind++){
    //    printf("now at pos %d, char %c\n", ind, *(currentTree+ind));
    if (strchr(delim,*(currentTree+ind)) != NULL){
      int tokLen = ind - lastInd;
      //      printf("found a delimiter\n"); 
      if (tokLen > 0){
	strncpy(tok,(currentTree+lastInd), tokLen);	
	*(tok+tokLen) = '\0';
	//	printf ("found species is %s\n",tok);
	n_species++;
      }
      lastInd = ind +1;
    }
    
  }
  return n_species;
}



/*---------------------------------------------------------------------*/

int  getQuartet(struct qrtStruct *tr, FILE * f){
  int c; int i =0, state =0;
  char line [100];
  //	printf ("\n\nfound ");
  //	exit (0);
  while ((c = fgetc(f)) != EOF){
    //    printf ("c=%c",c);
    if ((c == ' ') || (c == '\n')){
      //      printf ("found space\n");
      if (state == 0){
	continue;
      }
      if (state == 4){
	*(line+i) = '\0';
	tr -> species[3] = atoi(line);
	//	printf ("found a space. sp3 is %d. End of triplet\n",tr -> species[3]);
	//	printf ("quartet IS %d,%d|%d,%d\n", 
	//		tr -> species[0],tr -> species[1],tr ->species[2],tr ->species[3]);
	return 1;
      }
      else {printf ("a space is permitted only before or after a quartet\n");
      exit(7);
      }
    }
    else
      if (c == ','){
	if ((state != 1) && (state != 3)){
	  printf("comma is allowed only after first species\n");
	  exit(7);
	}
	else{
	  *(line+i) = '\0';
	  if (state == 1){
	    tr -> species[0] = atoi(line);
	  }
	  else{
	    tr -> species[2] = atoi(line);
	  }	    
	  //	  printf ("found a comma. sp1 is %d\n",tr -> species[0]);
	  if (state == 1){
	    state = 2;
	  }
	  else{
	    state = 4;
	  }
	  i = 0;
	}
      }
      else
	if (c == '|'){
	  if (state != 2){
	    printf("bar is allowed only after second species\n");
	    exit(7);
	  }
	  else{
	    *(line+i) = '\0';
	    tr -> species[1] = atoi(line);
	    //	    printf ("found a bar. sp2 is %d\n",tr -> species[1]);
	    state = 3;
	    i = 0;
	  }
	}
	else{
	  //	  printf ("moving %c to line\n",c);
	  *(line+i) = c;
	  i++;
	  if (state == 0){
	    state = 1;
	  }
	}
  }
  if (c == EOF){
    if (state != 0 && state != 4){
      printf ("\n\n ERROR ERROReof reached but state = %d\n", state);
      exit (8);
    }
    if (state == 4){
      *(line+i) = '\0';
      tr -> species[3] = atoi(line);
      //      printf ("EOF reached but there ia a quartet. sp4 is %d. End of triplet\n",tr -> species[3]);
      //      printf ("quartet IS %d,%d|%d,%d\n", 
      //	      tr -> species[0],tr -> species[1],tr ->species[2],tr ->species[3]);
      return 1;
    }
  }
  printf ("eof reached\n");
  return 0;
  //  return EOF;
}

/*---------------------------------------------------------------------*/

/*---------------------------------------------------------------------*/

treeNode *readQuartetsAndTree(char * treeFname, char * qrtFname){
  FILE *qrtFile = fopen(qrtFname, "r");
  FILE *treeFile;
  char * tree;
  struct qrtStruct * prev, * current;
  treeNode * t;
  int i=0, max_sp =0;
  
  
  if (qrtFile == NULL){
    printf ("open for %s failed\n",qrtFname);
    exit (0);
  }
  //  exit (5);
  prev  = anchor = (struct qrtStruct *) malloc (sizeof(struct qrtStruct));
  prev -> next = NULL;
  getQuartet(anchor, qrtFile);
  current = (struct qrtStruct *) malloc (sizeof(struct qrtStruct));
  while (	getQuartet(current, qrtFile)){
    prev -> next = current;
    prev = current;
    prev -> next = NULL;
    current = (struct qrtStruct *) malloc (sizeof(struct qrtStruct));
    i++;
  }
  free (current);
  for (i=0, current = anchor; current != NULL; current = current -> next){
    my_printf ("quartet %d: %d,%d|%d,%d\n",i++,
		    current->species[0],
		    current->species[1],
		    current->species[2],
		    current->species[3]);
    
  }
  n_quartets = i;
  //  exit (6);
  treeFile = fopen(treeFname, "r");
  tree= (char *) malloc(200000);
  if (fgets(tree,200000,treeFile) == NULL) {
    printf ("could not read the tree file %s\n",treeFname);
    exit (3);
  }
  if (feof(treeFile)) {
    printf ("EOF REACHED. OK\n");
  }
  else {
    printf("not all the tree read\n");
  }
  //  exit (7);
  printf ("tree: %s\n",tree);
  if (*(tree+strlen(tree)-1) == '\n'){
    //    printf ("tree %s ends ends with newline\n",tree);
    *(tree+strlen(tree)-1) = '\0';
    //    printf ("newtree %s does not.\n",tree);
  }
  t = parseTree(tree);
  get_n_species(tree);
  printf ("\n\n\nNumber of species is %d and number of quartets is %d\n\n",n_species, n_quartets);
  //	exit (3);
  return t;
}

/*---------------------------------------------------------------------*/



/*---------------------------------------------------------------------*/


main(int argc, char *argv[])
{
  int i;
  float frac = 3.0/2;
  int flip;
  int correct = 90;
  char * tree1 = "(((11,12,13,14),8,(9,20,21,22)),1,(2,3,(5,6,7)),4)";
  char * tree2 = "((1,2),3)";
  char * tree4 = "((1,2),(3,4))";
  char * tree3 = "(4)";
  char * tree5 = "(((((0,(1,2)),(3,(4,5))),((6,(7,8)),(9,(10,11)))),(((12,(13,14)),(15,(16,17))),((18,(19,20)),((21,22),(23,24))))),((((25,(26,27)),(28,(29,30))),((31,(32,33)),(34,(35,36)))),(((37,(38,39)),(40,(41,42))),((43,(44,45)),((46,47),(48,49))))))";
  char * tree6 = "((((((0,(1,2)),(3,(4,5))),((6,(7,8)),(9,(10,11)))),(((12,(13,14)),(15,(16,17))),((18,(19,20)),((21,22),(23,24))))),((((25,(26,27)),(28,(29,30))),((31,(32,33)),(34,(35,36)))),(((37,(38,39)),(40,(41,42))),((43,(44,45)),((46,47),(48,49)))))),(((((50,(51,52)),(53,(54,55))),((56,(57,58)),(59,(60,61)))),(((62,(63,64)),(65,(66,67))),((68,(69,70)),((71,72),(73,74))))),((((75,(76,77)),(78,(79,80))),((81,(82,83)),(84,(85,86)))),(((87,(88,89)),(90,(91,92))),((93,(94,95)),((96,97),(98,99)))))))";
  treeNode *t, *t1, *t2, *t3;
  char *treeFname,  *qrtFname;
  int improve;
  FILE *outFile = fopen("qrt-score.dat","w");

  /*
  
  srandom(50000);
  t2 = parseTree(tree2);
  t1 = parseTree(tree6);
  printf("\n\nHere's the tree.\n");
  print_tree(t1);
  checkTree(t1);
  traverseTree(t1,t1);
  exit(3);

  t = lca(2,7);
  printf ("lca is %d\n", t->id);
  printf("the subtree is \n");
  print_tree(t);
  removeSubtree(t);
  printf("\n\ntree after removing subtree is:\n");
  print_tree(t1);
  checkTree(t1);
  split_edge(leaves[8],t);
  printf("\n\ntree after inserting subtree is:\n");
  print_tree(t1);
  checkTree(t1);

  t = lca(11,14);
  printf ("lca is %d\n", t->id);
  printf("the subtree is \n");
  print_tree(t);
  removeSubtree(t);
  printf("\n\ntree after removing subtree is:\n");
  print_tree(t1);
  checkTree(t1);
  split_edge(lca(20,21),t);
  printf("\n\ntree after inserting subtree is:\n");
  print_tree(t1);
  checkTree(t1);

    t=leaves[8];
  removeSubtree(t);
  printf("\n\ntree after removing subtree is:\n");
  print_tree(t1);
  checkTree(t1);
  

 exit(3);
  //  t2 = parseTree(tree2);
  //  t3 = parseTree(tree3);
  //  insert_tree(t2,t3,t2);
  t1 = parseTree(tree4);
  printf("Here's the tree.\n");
  lca(1,2);
  t = lca(7,2);
  
  exit(3);
  
  printf("Here's the tree.\n");
  print_tree(t1);
  printf("\n\nnoe searching\n");
  search_tree(t1);
  exit (5);  

  */

  parseParms(argc, argv, &treeFname, &qrtFname);
  printf("quartet file is %s, tree file %s\n",qrtFname, treeFname);
  
  t1 = readQuartetsAndTree(treeFname, qrtFname);
  printf ("after read quartets. Quartst read %ld\n", n_quartets);
  //  exit(7);
  best_score = countAgree();
  printf("\n\nscore is %ld, percent %f\n",best_score, ((float)best_score/n_quartets));
  //  exit(8);
  fprintf (outFile,"%s\t%s\t%ld\t%ld\t%3.1f\n",qrtFname, treeFname,  n_species, n_quartets,  100* ((float)best_score/n_quartets));
  close (outFile);
  return 0;
}

