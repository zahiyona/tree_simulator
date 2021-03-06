#!/usr/bin/python

import string, sys, re, copy, os, traceback
#from numpy import *
#from math import *
from subprocess import call
from random import choice
from time import *
#import numpy as np
from .phylo import *


#-----------------------------------------------------------------------------------_#

class TreeNode:
    count = 0
    def __init__(self):
        self.id = TreeNode.count
        TreeNode.count +=1
        print('(Initialized node: %d)' % self.id)
	
    def tell(self):
        '''Tell my details.'''
        print('Name:"%s" Age:"%s"' % (self.name, self.age), end=' ')


#-----------------------------------------------------------------------------------_#
def LCA(n1, n2):
    # First stage: delete attr
    print("now starting from ", n1.id)
    n = n1
    while hasattr(n,'father'): 
#        print "Now at node ",n.id
        if hasattr(n,'lca'):
            delattr(n,'lca')
        n = n.father

#    print "stopped at node ", n.id
    if hasattr(n,'lca'):
        delattr(n,'lca')
        
#    print "now starting from ", n2.id
    n = n2
    while hasattr(n,'father'): 
        setattr(n,'lca',1)
        n = n.father

#    print "stopped at node ", n.id
    setattr(n,'lca',1)

#    print " starting again from ", n1.id
    n = n1
    while hasattr(n,'father'): 
#        print "Now at node ",n.id
        if hasattr(n,'lca'):
#            print "stopped at ", n.id
            return n
        n = n.father

#    print "stopped at node ", n.id
    if not hasattr(n,'lca'):
        ERR("At LCA. Arrived at a node w/o father (root?) but with no lca-mark")
    return n



#------------------------------------------------------------------------------------#

def get_root(n):
    print("At get root from node %d" % n.id)
    if hasattr(n,'name'):
        print("Node is a leaf %s" %n.name)
    while  hasattr(n,'father'):
        print("Node %d has father %d" % (n.id, n.father.id))
        n = n.father
    
#    print "Ended at node %d", n.id
    if hasattr(n,'father'):
        ERR("Ended get_root at a node %d that has a father %d" %  (n.id, n.father.id))
            
    return n

#------------------------------------------------------------------------------------#
def get_quartet_from_leaves(leaves, q):
    [s1, s2, s3, s4] = q
    print("In get_quartet_from_leaves for %s, %s, %s, %s\n" % \
        ( s1, s2, s3, s4))
    if  s1 in leaves:
        n1 = leaves[s1]
    else:
        ERR("name %s does not exist in leaves\n" %(s1))

    if  s2 in leaves:
        n2 = leaves[s2]
    else:
        ERR("name %s does not exist in leaves\n" %(s2))

    if  s3 in leaves:
        n3 = leaves[s3]
    else:
        ERR("name %s does not exist in leaves\n" %(s3))

    if  s4 in leaves:
        n4 = leaves[s4]
    else:
        ERR("name %s does not exist in leaves\n" %(s4))


    out1 = triplet_topology(n1, n2, n3)
    print("out1:", out1)
    if out1 == None:
        print(("Unresolved triplet for %s, %s, %s\n" % (s1, s2, s3)))
        out2 = triplet_topology(n2, n3, n4)
        if out2 == None:
            print(("Unresolved triplet for %s, %s, %s\n" % (s4, s2, s3)))
            # We can still have two options
            out3 = triplet_topology(n1, n2, n4)
            if out3 == None:
                print(("Unresolved triplet for %s, %s, %s\n" % (s4, s2, s3)))
                print(("Unresolved quartet for %s, %s, %s, %s\n" % (s1, s4, s2, s3)))
                r = get_root(n1)
                det_tre = printDetailedTree(r, "")
                print("Unresolved quartet for %s, %s, %s, %s on tree\n%s" % \
                    (s1, s2, s3, s4, det_tre))
                return 
                ERR( "Unresolved quartet for %s, %s, %s, %s on tree\n%s" % \
                         (s1, s2, s3, s4, det_tre))
                return

            if out3 == n2:
                print("Third triplet is %s,%s|%s\n" % (s1,s4,s2))
                print("quartet is %s,%s|%s,%s\n" % (s4,s1, s2,s3))
                return [s1,s4,s3,s2]
            else:
                print("\n\n\nINTERNAL ERROR!!!!!!!!!!!!!!!!!")
                print("Third triplet is either %s,%s|%s or %s,%s|%s\n\n" % (s1,s2,s4, s1,s2,s4))
                print("This case is IMPOSSIBLE!!!!\n\n")
                err("iNTERNAL ERROR IN TRIPLET ALGORITHM!!!!!\n\n\n")
 

        else:
            if (out2 != n4 and out2 != n2 and out2 != n3):
                ERR ("ERROR ERROR. invalid out2 %d\n" % (out2.id))

            if (out2 == n2):
                print("second triplet is %s,%s|%s\n" % (s3,s4,s2))
                print("quartet is %s,%s|%s,%s\n" % (s4,s3, s2,s1))
                return [s1,s2,s3,s4]

            if (out2 == n3):
                print("second triplet is %s,%s|%s\n" % (s2,s4,s3))
                print("quartet is %s,%s|%s,%s\n" % (s1,s3, s2,s4))
                return [s1,s3,s2,s4]
 
            if (out2 == n4):
                print("second triplet is %s,%s|%s\n" % (s2,s3,s4))
                print(("Unresolved quartet for %s, %s, %s, %s\n" % (s1, s4, s2, s3)))
                r = get_root(n1)
                det_tre = printDetailedTree(r, "")
                print("Unresolved quartet for %s, %s, %s, %s on tree\n%s" % \
                    (s1, s2, s3, s4, det_tre))
                return
                ERR( "Unresolved quartet for %s, %s, %s, %s on tree\n%s" % \
                         (s1, s2, s3, s4, det_tre))
                return
            ERR("We should not get here!!!!!")



    if (out1 != n1 and out1 != n2 and out1 != n3):
        print("In get_quartet_from_leaves for %s, %s, %s, %s\n" % \
            ( s1, s2, s3, s4))
        print(out1)
        ERR ("ERROR ERROR. for quartet %s. Invalid out1 %d\n" % \
        (str(q), out1.id)) 


    print("out1 is %d\n" %(out1.id))

    if (out1 == n1):
        print("first triplet is %s,%s|%s\n" % (s2,s3,s1))
        out2 = triplet_topology(n2, n3, n4)
        if out2 == None:
            print(("Second triplet is unresolved for %s, %s, %s\n" % (s4, s2, s3)))
            print(("Unresolved quartet for %s, %s, %s, %s\n" % (s1, s4, s2, s3)))
            r = get_root(n1)
            det_tre = printDetailedTree(r, "")
            print("Unresolved quartet for %s, %s, %s, %s on tree\n%s" % \
                (s1, s2, s3, s4, det_tre))
            return
            ERR( "Unresolved quartet for %s, %s, %s, %s on tree\n%s" % \
                     (s1, s2, s3, s4, det_tre))
            return


        if (out2 != n2 and out2 != n3 and out2 != n4):
            ERR ("ERROR ERROR. invalid out2 %ld\n" % out2.id);
            
        if (out2 == n2):
            print("second triplet is %s,%s|%s\n" % (s3,s4,s2))
            print("quartet is %s,%s|%s,%s\n" % (s4,s3, s2,s1))
            return [s1,s2,s3,s4]

        if (out2 == n3):
            print("second triplet is %s,%s|%s\n" % (s2, s4, s3))
            print("quartet is %s,%s|%s,%s\n"% (s2, s4, s3, s1)) 
            return [s2, s4, s3, s1]

        if (out2 == n4):
            print("second triplet is %s,%s|%s\n" % (s2, s3, s4))
            print("quartet is %s,%s|%s,%s\n"% (s2, s3, s4, s1))
            return [s2, s3, s4, s1]

    if (out1 == n2):
        print("first triplet is %s,%s|%s\n"% (s3, s1, s2))
        out2 = triplet_topology (n1, n3, n4)
        if out2 == None:
            print(("Second triplet is unresolved for %s, %s, %s\n" % (s4, s1, s3)))
            print(("Unresolved quartet for %s, %s, %s, %s\n" % (s1, s4, s2, s3)))
            r = get_root(n1)
            det_tre = printDetailedTree(r, "")
            print("Unresolved quartet for %s, %s, %s, %s on tree\n%s" % \
                (s1, s2, s3, s4, det_tre))
            return
            ERR( "Unresolved quartet for %s, %s, %s, %s on tree\n%s" % \
                     (s1, s2, s3, s4, det_tre))
            return


        if (out2 != n1 and out2 != n3 and out2 != n4):
            ERR ("ERROR ERROR. invalid out2 %d\n" %  out2.id)

        if (out2 == n1):
            print("second triplet is %s,%s|%s\n" % (s3, s4, s1))
            print("quartet is %s,%s|%s,%s\n" % (s3, s4, s2, s1))
            return [s3, s4, s2, s1]

        if (out2 == n3):
            print("second triplet is %s,%s|%s\n" % (s1, s4, s3))
            print("quartet is %s,%s|%s,%s\n"% (s1, s4, s2, s3))
            return [s1, s4, s2, s3]
        
        if (out2 == n4):
            print(("second triplet is %s,%s|%s\n" % (s1, s3, s4)))
            print(("quartet is %s,%s|%s,%s\n" % (s1, s3, s4, s2)))
            return [s1, s3, s4, s2]

    if (out1 == n3):
        print(("first triplet is %s,%s|%s\n" % (s1, s2, s3)))
        out2 = triplet_topology (n1, n2, n4)
        if out2 == None:
            print(("Second triplet is unresolved for %s, %s, %s\n" % (s4, s2, s1)))
            print(("Unresolved quartet for %s, %s, %s, %s\n\n" % (s1, s4, s2, s3)))
            r = get_root(n1)
            det_tre = printDetailedTree(r, "")
            print("Unresolved quartet for %s, %s, %s, %s on tree\n\n%s" % \
                (s1, s2, s3, s4, det_tre))
            return

            ERR( "Unresolved quartet for %s, %s, %s, %s on tree\n\n%s" % \
                     (s1, s2, s3, s4, det_tre))
            return


        if (out2 != n1 and out2 != n2 and out2 != n4):
            ERR ("ERROR ERROR. invalid out2 %ld\n"%  out2.id)

        if (out2 == n2):
            print(("second triplet is %s,%s|%s\n" % (s1, s4, s2)))
            print(("quartet is %s,%s|%s,%s\n"% (s4, s1, s2, s3)))
            return [s4, s1, s2, s3]

        if (out2 == n1):
            print(("second triplet is %s,%s|%s\n" % (s2, s4, s1)))
            print(("quartet is %s,%s|%s,%s\n" % (s2, s4, s1, s3)))
            return [s2, s4, s1, s3]
        
        if (out2 == n4):
            print(("second triplet is %s,%s|%s\n" % (s1, s2, s4)))
            print(("quartet is %s,%s|%s,%s\n" % (s1, s2, s3, s4)))
            return [s1, s2, s3, s4]

#-----------------------------------------------------------------------------------_#

def obtainMatchedParen(tree, ind):
    if tree[ind] != '(':
        ERR ("In matched paren, first char is NOT \"(\"");
    balance = 1
    ind += 1
    while ind < len(tree):
        if tree[ind] == '(':
            balance += 1
        if tree[ind] == ')':
            balance -= 1
        if balance == 0:
            return ind
        ind +=1

    ERR ("Could not a match at "+tree[ind:])
            

#-----------------------------------------------------------------------------------_#
def get_descendant(n):
    print("At get descendant for node ",n.id)
    if hasattr(n,'name'):
        print("starting node", n.id, " is a leaf", n.name)
    while hasattr(n,'son'):
        print("descending at ",n.id)
        if hasattr(n,'name'):
            ERR("Node "+str(n.id)+" has son "+str(n.son.id)+" and a name "+n.name)
        n = n.son
    
    if not hasattr(n,'name'):
        ERR("Node "+str(n.id)+" has NO son and also NO name ")
    print("ended descending at ", n.id)
    return n

#-----------------------------------------------------------------------------------_#
def ck_quartet(n1, n2, n3, n4):

    if n1 == None or n2 == None or n3 == None or n3 == None:
        ERR("One of the nodes is empty\n")
    if not hasattr(n1,'name') or  not hasattr(n2,'name') or  \
            not hasattr(n3,'name') or  not hasattr(n4,'name'):
        ERR("One of the nodes has no name\n")
    print("\n\nAt ck quartet for leaves %s, %s, %s, %s"%\
        (n1.name, n2.name, n3.name, n4.name,))

    l12 =  LCA(n1, n2)
    print("LCA of "+n1.name+" and "+n2.name+" is", l12.id)
   

    l13 =  LCA(n1, n3)
    print("LCA of "+n1.name+" and "+n3.name+" is", l13.id)
 
    l23 =  LCA(n2, n3)
    print("LCA of "+n2.name+" and "+n3.name+" is", l23.id)

    l14 =  LCA(n1, n4)
    print("LCA of "+n1.name+" and "+n4.name+" is", l14.id)

    l24 =  LCA(n2, n4)
    print("LCA of "+n2.name+" and "+n4.name+" is", l24.id)

    l34 =  LCA(n3, n4)
    print("LCA of "+n3.name+" and "+n4.name+" is", l34.id)

    if not hasattr(l12,'father'):
        ERR("LCA12 %d of "+n1.name+" and of "+n2.name+" MUST ave a father" %l12.id)

    if l23 != l13 :
        ERR("LCA13 %d is not LCA23 %d \n" %(l13.id, l23.id))

    if l24 != l14 :
        ERR("LCA14 %d is not LCA24 %d \n" %(l14.id, l24.id))

    if hasattr(l12.father,'father'):
        if l34 != l14 :
            ERR("LCA34 %d is not LCA14 %d \n" %(l34.id, l14.id))
        if l13 != l12.father:
            ERR("LCA12 %d has father %d but is not LCA13 %d\n" % (l12.id, l12.father.id, l13.id))
    else:
        if l12.father != l34.father:
            ERR("FORK topology and LCA12 father %d is NOT LCA34 father %d" % \
                    (l12.father, l34.father))
        
#-----------------------------------------------------------------------------------_#
def triplet_topology(n1,n2,n3):

    if n1 == None or n2 == None or n3 == None:
        ERR("One of the nodes is empty\n")
    if not hasattr(n1,'name') or  not hasattr(n2,'name') or  not hasattr(n3,'name') :
        ERR("One of the nodes has no name\n")

    print("\n\nAt  triplet topology for %s, %s, %s" % \
        (n1.name, n2.name, n3.name))

    L12 = LCA(n1, n2)
    print("LCA of "+n1.name+" and "+n2.name+" is", L12.id)
    if L12 != n1.father:
#        ERR("LCA of "+n1.name+" and "+n2.name+" MUST be the father of "+n1.name)
        pass

    L23 =  LCA(n2, n3)
    print("LCA of "+n3.name+" and "+n2.name+" is", L23.id)
    if L23 == n1.father:
#        ERR("LCA of "+n2.name+" and "+n3.name+" CANNOT be the father of "+n1.name)
        pass

    L13 =  LCA(n1, n3)
    print("LCA of "+n1.name+" and "+n3.name+" is", L13.id)

    if L13 == L23 and L23 == L12:
        print("\\unresolved triplet ", n1.name, n2.name, n3.name)
        return 
    
    if L13 == L23:
         print("LCA of (", n1.name, n3.name,") and (", n2.name, n3.name,") are the same.",\
             "outgroup is",  n3.name)
         return n3

    if L13 == L12:
         print("LCA of (", n1.name, n3.name,") and (", n2.name, n1.name,") are the same.",\
             "outgroup is",  n1.name)
         return n1


    if L12 == L23:
         print("LCA of (", n1.name, n2.name,") and (", n2.name, n3.name,") are the same.",\
             "outgroup is",  n2.name)
         return n2
     
    ERR("No outgroup was found for "+n1.name+", "+n2.name+", "+n3.name)


#-----------------------------------------------------------------------------------_#
def ck_triplet(n1,n2,n3):
    print("\n\nAt ck triplet for leaves %s, %s, %s" % \
        (n1.name, n2.name, n3.name))
    if n1 == None or n2 == None or n3 == None:
        ERR("One of the nodes is empty\n")
    if not hasattr(n1,'name') or  not hasattr(n2,'name') or  not hasattr(n3,'name') :
        ERR("One of the nodes has no name\n")

    L12 = LCA(n1, n2)
    print("LCA of "+n1.name+" and "+n2.name+" is", L12.id)
    if L12 != n1.father:
        ERR("LCA of "+n1.name+" and "+n2.name+" MUST be the father of "+n1.name)
    

    L23 =  LCA(n2, n3)
    print("LCA of "+n3.name+" and "+n2.name+" is", L23.id)

    L13 =  LCA(n1, n3)
    print("LCA of "+n1.name+" and "+n3.name+" is", L13.id)

    if L12 == L13 and L12 == L23:
        print("Father of %s is a multifurcating node" % n1.name)
        return

    if L23 == n1.father:
        ERR("LCA of "+n2.name+" and "+n3.name+" CANNOT be the father of "+n1.name)


    if hasattr(n1.father,'father'):
        if L23 != n1.father.father:
            ERR("Grandfather of "+n1.name+" is not LCA of "+n2.name+" and "+n3.name)
        if L23 != L13 :
            ERR(n1.name +" has a grandfather but LCA of  "+n2.name+" and "+n3.name+ \
                    " is not the same")
    else:
        if L23.father != n1.father:
            ERR("father of "+n1.name+" is not LCA of "+n2.name+" and "+n3.name)
        if L12 != L13:
            ERR(n1.name +" has NO grandfather but LCA of  "+n2.name+" and "+n3.name+ \
                    " is not the same")




#-----------------------------------------------------------------------------------_#
def get_defining_triplet(n):
    print("\nAt get_defining_triplet for leaf %s\n" % n.name)
    if not hasattr(n,'father'):
        ERR("Node id "+str(n.id)+" has no father. Can't obtain triplet")
    f = n.father
# We first check if father has three sons        
    if not hasattr(f.son,'brother'):
        ERR("Father %d has a single son %d" % (f.id, f.son.id))
    if hasattr(f.son.brother,'brother'):
        print("Three sons at node %d (possibly the root)\n" % f.id)
#        exit(8)
        brothers = []
        b = f.son
        print("searching for brothers")
        while  True:
            if b != n:
                print("found a son %d that is not me. Adding to brothers" % \
                    b.id)
                brothers.append(b)
                if len(brothers) == 2:
                    print("Two brother %s found. leaving" % str(brothers))
                    break
            else:
                print("Found myself: %d" %b.id)
            if hasattr(b,'brother'):
                b = b.brother
            else:
                break
        if len(brothers) < 2:
            ERR("Did not find two bothers. brothers: %s" %str(brothers))
                
        print("getting descendants of both brothers %d, %d" % \
            (brothers[0].id, brothers[1].id ))
        d1 = get_descendant(brothers[0])
        d2 = get_descendant(brothers[1])
        return d1, d2
            

    print("Father %d has only two sons: %d, %d" %     \
        (f.id, f.son.id, f.son.brother.id))
    if f.son != n and  f.son.brother != n:
        ERR("Non of father sons %d and %d is me %d" %  \
                (f.son.id, f.son.brother.id, n.id))
    if f.son == n:
        b  = f.son.brother
    else :
        b = f.son

    if not hasattr(f,'father'):
        print("father ",f.id," has no father. Is a root")
        if not hasattr(b,'son'):
            ERR("No father to father (root?) %d and no son to brother %d", f.id, )
        g1 = b.son
        if not hasattr(g1,'brother'): 
            ERR("Grandson "+str(g1.id)+" must have a brother ")
        g2 = g1.brother
        print("Found two grandsons of root", g1.id, g2.id)
        d1 = get_descendant(g1)
        d2 = get_descendant(g2)
        return d1, d2
    else:
        u =  get_brother(f)
        if u == None:
            ERR("No bother to father "+str(f.id))
        
        d1 = get_descendant(b)
        d2 = get_descendant(u)
        return d1, d2


#-----------------------------------------------------------------------------------_#
def get_defining_quartet(n):
    print("\nAt get defining quartet for node %d" % n.id)
    # first get the two descendants under the node
    if not hasattr(n,'father'):
        ERR("Node id "+str(n.id)+" has no father. Can't obtain triplet")
    f = n.father
    if not hasattr(n,'son'):
            ERR("Internal starting node "+str(n.id)+" has no son")
    s1 = n.son
    s2 = get_brother(s1)
    if s2 == None:
        ERR("No brother to son "+str(s1.id)+" of base node "+str(n.id))
    d1 = get_descendant(s1)
    d2 = get_descendant(s2)

# We first check if father has three sons        
    if not hasattr(f.son,'brother'):
        ERR("Father %d has a single son %d" % (f.id, f.son.id))
    if hasattr(f.son.brother,'brother'):
        print("Three sons at node %d (possibly the root)\n" % f.id)
#        exit(8)
        brothers = []
        b = f.son
        print("searching for brothers")
        while  True:
            if b != n:
                print("found a son %d that is not me. Adding to brothers" % \
                    b.id)
                brothers.append(b)
                if len(brothers) == 2:
                    print("Two brother %s found. leaving" % str(brothers))
                    break
            else:
                print("Found myself: %d" %b.id)
            if hasattr(b,'brother'):
                b = b.brother
            else:
                break
        if len(brothers) < 2:
            ERR("Did not find two bothers. brothers: %s" %str(brothers))
                
        print("getting descendants of both brothers %d, %d" % \
            (brothers[0].id, brothers[1].id ))
        d3 = get_descendant(brothers[0])
        d4 = get_descendant(brothers[1])
        return d1, d2, d3, d4
            

    print("Father %d has only two sons: %d, %d" %     \
        (f.id, f.son.id, f.son.brother.id))
    if f.son != n and  f.son.brother != n:
        ERR("Non of father sons %d and %d is me %d" %  \
                (f.son.id, f.son.brother.id, n.id))
    if f.son == n:
        b  = f.son.brother
    else :
        b = f.son

    if not hasattr(f,'father'):
        print("father ",f.id," has no father. Is a root")
        if not hasattr(b,'son'):
            ERR("No father to father (root?) %d and no son to brother %d", f.id, )
        g1 = b.son
        if not hasattr(g1,'brother'): 
            ERR("Grandson "+str(g1.id)+" must have a brother ")
        g2 = g1.brother
        print("Found two grandsons of root", g1.id, g2.id)
        d3 = get_descendant(g1)
        d4 = get_descendant(g2)
        return d1, d2, d3, d4
    else:
        u =  get_brother(f)
        if u == None:
            ERR("No bother to father "+str(f.id))
        
        d3 = get_descendant(b)
        d4 = get_descendant(u)
        return d1, d2, d3, d4



 
#-----------------------------------------------------------------------------------_#
def get_brother(n):
    if not hasattr(n,'father'):
        ERR("No father to node "+str(n.id)+" and hence also no brother")
    f=n.father
    if not hasattr(f,'son'):
        ERR("No son to father node "+str(f.id))
    s = f.son
    while True:
        print("at son ", s.id)
        if s == n:
            print("this son is me ", n.id)
        else:
            print("arrived at a son ",s.id ," that is not me. returning")
            return s
        if not hasattr(s,'brother'):
            return   
        s = s.brother

#-----------------------------------------------------------------------------------_#
def contract_tree(n):
    print("at contract tree in node ",n.id)
    if hasattr(n,'son'):
        print("Node ", n.id, "has son ",n.son.id," and is internal.")
        if hasattr(n,'name'):
            print("ERROR ERROR ERROR. Node ", n.id, "has son AND a name.")
            exit(8)

        s = n.son
        if not hasattr(s,'brother'):
            print("\nSingle Son, Single Son. node %d has only one son %d" %(n.id, n.son.id))
            if hasattr(n,'edge_len'):
                if hasattr(s,'edge_len'):
                    s.edge_len += n.edge_len
                else:
                    setattr(s,'edge_len', n.edge_len)
            if hasattr(n,'father'):
                setattr(s,'father', n.father)
            else:
                if hasattr(s,'father'):
                    delattr(s,'father')
                else:
                    ERR("Son %d has no father" % (s.id))
            
            if hasattr(n,'brother'):
                setattr(s,'brother',n.brother)

            return contract_tree(s)
        else:
            print("More than one son to node %d" %n.id)
            n.son = contract_tree(s)
    
    else:
        if not hasattr(n,'name'):
            ERR("Node %d has no son and also no name" %n.id)
        print("At a leaf %d with name %s" %(n.id, n.name))

    if hasattr(n,'brother'):
        n.brother = contract_tree(n.brother)
    print("returning from %d"% n.id)
    return n

#-----------------------------------------------------------------------------------_#

def printTree(n, outS):
#    print "in printTree, node id", n.id
    if hasattr(n,'son'):
#        print "Node ", n.id, "has son ",n.son.id,". continue recursing"
        if hasattr(n,'name'):
            print("ERROR ERROR ERROR. Node ", n.id, "has son AND a name.")
            exit(8)
        print('(', end=' ')
        outS += '('
        outS = printTree(n.son, outS)
        outS += ')'
#        outS += ')'+"[%d]"%n.id
        if hasattr(n,'edge_len'):
            outS += ":%5.4f"% getattr(n,'edge_len')
        print(')', end=' ')
    else:
#        print "Node ", n.id, "has NO son. Must be a leaf"
        if not hasattr(n,'name'):
            print("ERROR ERROR ERROR. Node ", n.id, "has NO son AND NO name.")
            exit(8)
#        print "\nAt a leaf", n.name
        outS += n.name
#        outS += n.name+"[%d]"%n.id
        if hasattr(n,'edge_len'):
            outS += ":%5.4f"% getattr(n,'edge_len')
        print(n.name, end=' ')
            
    if hasattr(n,'brother'):
        outS += ','
        print(',', end=' ')
#        print "Node ", n.id, "has brother. ",n.brother.id,".continue recursing. "
        outS = printTree(n.brother, outS)
    
    return outS

#-----------------------------------------------------------------------------------_#


#-----------------------------------------------------------------------------------_#


def printDetailedTree(n, outS):
#    print "in printTree, node id", n.id
    if hasattr(n,'son'):
#        print "Node ", n.id, "has son ",n.son.id,". continue recursing"
        if hasattr(n,'name'):
            print("ERROR ERROR ERROR. Node ", n.id, "has son AND a name.")
            exit(8)
        print('(', end=' ')
        outS += '('
        outS = printDetailedTree(n.son, outS)
#        outS += ')'
        outS += ')'+"[%d]"%n.id
        if hasattr(n,'edge_len'):
            outS += ":%5.4f"% getattr(n,'edge_len')
        print(')', end=' ')
    else:
#        print "Node ", n.id, "has NO son. Must be a leaf"
        if not hasattr(n,'name'):
            print("ERROR ERROR ERROR. Node ", n.id, "has NO son AND NO name.")
            exit(8)
#        print "\nAt a leaf", n.name
#        outS += n.name
        outS += n.name+"[%d]"%n.id
        if hasattr(n,'edge_len'):
            outS += ":%5.4f"% getattr(n,'edge_len')
        print(n.name, end=' ')
            
    if hasattr(n,'brother'):
        outS += ','
        print(',', end=' ')
#        print "Node ", n.id, "has brother. ",n.brother.id,".continue recursing. "
        outS = printDetailedTree(n.brother, outS)
    
    return outS

#-----------------------------------------------------------------------------------_#



#-----------------------------------------------------------------------------------_#

def addSon(father, son):
    print("in addSon for father %d and a new son %d\n" % \
        (father.id,  son.id))
    if hasattr(father,'name'):
        ERR("ERROR ERROR. Trying to insert a son to a leaf\n")
    if hasattr(father,'son'):
        temp = father.son
        son.brother = temp
    father.son = son
    son.father = father


#-----------------------------------------------------------------------------------_#

def find_dist_between_leaves(x,y,leaves):
    if not x in list(leaves.keys()):
        ERR ("Leaf "+x+" is not in the leaves of the tree")

    if not y in list(leaves.keys()):
        ERR ("Leaf "+y+" is not in the leaves of the tree")


    print("\n\n\nfinding dist between "+x+" and "+y)


    n = leaves[x]
    print("now starting from "+x)
    while hasattr(n,'father'): 
        print("Now at node ",n.id)
        if hasattr(n,'val'):
            delattr(n,'val')
        n = n.father

    print("stopped at node ", n.id)
    if hasattr(n,'val'):
        delattr(n,'val')
        
    print("now starting from "+y)
    n = leaves[y]
    while hasattr(n,'father'): 
        if hasattr(n,'val'):
            delattr(n,'val')
        n = n.father

    print("stopped at node ", n.id)

    n = leaves[x]

    print("calculating path len, starting from "+x)
    path_len=0
    while hasattr(n,'father'): 
        setattr(n,'val',path_len)    # Set len up to the node
        print("\nat node id %d,  path len %f" % (n.id, path_len), end=' ')
        if hasattr(n,'edge_len'):
            print("Found edge len %f" %( n.edge_len), end=' ')
            elen = n.edge_len
            if elen <= 0:
                print(("Found a non positive edge len %f at node %d\n" %(elen, n.id)))
#                ERR("Found a non positive edge len %f at node %d\n" %(elen, n.id))
            path_len += elen          # add to the total len
        n = n.father
        
    setattr(n,'val',path_len) # Set len to the root at the root

    print("\n\ncalculating path len, starting from "+y)
    n = leaves[y]
    path_len=0
    while not hasattr(n,'val'): 
        print("\nat node id %d,  path len %f" % (n.id, path_len), end=' ')
        if hasattr(n,'edge_len'):
            print("Found edge len %f" %( n.edge_len), end=' ')
            elen = n.edge_len
            if elen <= 0:
                print(("Found a non positive edge len %f at node %d\n" %(elen, n.id)))
#                ERR("Found a non positive edge len %f at node %d\n" %(elen, n.id))
            path_len += elen 
        n = n.father
    print("\n\nstopped at node %d, path len %f" % (n.id, path_len))
    lca = n
    pair_dist = n.val+path_len
    print("reached LCA %d. Len of path1 to LCA %f. len path2 to LCA %f. Total %f" % \
        (lca.id, n.val,path_len, pair_dist))
    return pair_dist

#-----------------------------------------------------------------------------------_#



def insertSubTree(tree, leaves):
    father =  TreeNode()
    if tree[0] != '(' or tree[-1] != ')' :
        ERR("tree does not start and end with ( and ):"+tree)
    ind = 1
    print("entered insert subtree. Allocated a father with id ", father.id)
    print("subtree is ", tree)
    while tree[ind] != ')':
        print("looking at char %d: %c\n" % (ind,  tree[ind] ))
        if tree[ind] == '(':
            print("Found a ( at %d, inserting a new son \n"  % ind)
            matched = obtainMatchedParen(tree, ind)
            print("the matched \')\' is at ", matched)
            print("resulted subtree:"+tree[ind:matched+1])
            son =  insertSubTree(tree[ind : matched+1], leaves)
            ind = matched +1
            print("returnd to subtree loop after finding son %d." \
                " char now is %c\n" % (son.id, tree[ind] ))
            if tree[ind] == ':':
                if not re.match(r'[-+]?(\d+(\.\d*)?|\.\d+)',tree[ind+1:]):
                    ERR("Found \':\' after "+name+" but could not parse len at "+ \
                            tree[ind:])   
                edge_len = (re.match(r'[-+]?(\d+(\.\d*)?|\.\d+)',tree[ind+1:])).group(0)
                print("Found edge len ", edge_len)
                ind += len(edge_len) +1
                setattr(son,'edge_len',float(edge_len))
        elif tree[ind]  == ',':
            print("found , inserting son with root id %d " \
	     " under father %d\n" % (son.id,  father.id))
            ind +=1
            addSon(father, son)
        elif re.match(r'(\w+)',tree[ind:]):
            name = re.match(r'(\w+)',tree[ind:]).group(0)
            print("found a name ", name)
            son = TreeNode()
            setattr(son,'name', name)
            if name in list(leaves.keys()):
                ERR ("name "+name + " appears twice in the leaves")
            leaves[name] = son
            ind += len(name)
            if tree[ind] == ':':
                if not re.match(r'[-+]?(\d+(\.\d*)?|\.\d+)',tree[ind+1:]):
                    ERR("Found \':\' after "+name+" but could not parse len at "+ \
                            tree[ind:])   
                edge_len = (re.match(r'[-+]?(\d+(\.\d*)?|\.\d+)',tree[ind+1:])).group(0)
                print("Found edge len ", edge_len)
                ind += len(edge_len) +1
                setattr(son,'edge_len',float(edge_len))
        else:
            ERR ("Invalid char \" "+tree[ind]+ "\" at "+str(ind))
    
    print("exited from insertSubtree loop. char is now %c\n" % tree[ind])
 
            
    addSon(father, son)
    return father


#-----------------------------------------------------------------------------------_#

# TreeNode.count =0

# tree = "((x1,x2),(P4,k5:0.004,3):-8,9:+.67,x3:0.99)"

# leaves = {};
# root = insertSubTree(tree, leaves)

# treen = printTree(root, "")
# print "\n\n\n", treen

# print "Leaves:", leaves.keys()

# find_dist_between_leaves('x3','k5',leaves)

