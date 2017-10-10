class TreeNode:
    def __init__(self):
        self.father=None
        self.son=None
        self.brother=None
        self.generation=None # do we need that here?
        self.value=None
        self.time=None
# -----------------------------------------------------------------------------
def FindMRCA(node1,node2):
# This function takes two nodes, and find their Most Recent Common Ancestor.
# The function assumes that that the nodes are distinct.
# node_a is the first chronologically, node_b is the latter.
#
# shouldn't I just get the generation and be done with it?
    j=node1.generation-node2.generation
    if j>=0: # this means node1 comes AFTER (or rather, not later from) node2
        node_b=node1
        node_a=node2
    if j<0: # this means node2 comes AFTER node1
        node_b=node2
        node_a=node1
    # setting node_b to a new value such that node_a and node_b have the same
    # generation in a way that doesn't change the MRCA.
    for i in range(abs(j)):
        node_b=node_b.father
    # now the two nodes belong to the same generation.
    if node_a==node_b:
        MRCA=node_a
    else:
        while node_a.father!=node_b.father:
            node_a=node_a.father
            node_b=node_b.father
        MRCA=node_a.father
    return MRCA
# -----------------------------------------------------------------------------
def ProduceQuartet(node1,node2,node3,node4):
# This function takes four tree nodes that are assumed to be distinct and
# returns their quartet topology as output.
# the different six ways to choose two nodes out of four give rise to six
# different variables that mark the generations of the MRCA's:
# g12, g13, g14, g23, g24, g34.
#
# It appears that stupid python can't handle nested sets. That is, the
# expression {{1,2},{3,4}} results in an error message. We therefore need to
# find another way to represent quartets.
# I can think of the following: Suppose we want to describe the quartet
# associated with a,b,c,d (where a<b<c<d). We can do that with a list:
# [(a,b,c,d),b] tells us that the four leaves we're looking at are a,b,c,d and
# that the quartet is a,b|c,d (that is, the last item in the list tells us
# which of the three greatest numbers - b,c or d - joins the smallest - a - in
# the first couple.
#
# the question remains: how do we go about rebuilding a tree from quartets when
# they are stored in this fashion? and how will we find the majority vote?
    MRCA12=FindMRCA(node1,node2)
    MRCA13=FindMRCA(node1,node3)
    MRCA14=FindMRCA(node1,node4)
    MRCA23=FindMRCA(node2,node3)
    MRCA24=FindMRCA(node2,node4)
    MRCA34=FindMRCA(node3,node4)
    g12=MRCA12.generation
    g13=MRCA13.generation
    g14=MRCA14.generation
    g23=MRCA23.generation
    g24=MRCA24.generation
    g34=MRCA34.generation
    List=[g12,g13,g14,g23,g24,g34]
    # It turns out that a quartet can be deduced only if max(List) appears in
    # the list no more than twice. Otherwise, the quartet in inconclusive and
    # the function returns {}.
    if List.count(max(List))<=2:
        if g12==max(List) or g34==max(List):
            a=node1.value
            b=node2.value
            c=node3.value
            d=node4.value
##            Q={node1,node2},{node3,node4} # I don't believe we're using it at all.
        if g13==max(List) or g24==max(List):
            a=node1.value
            b=node3.value
            c=node2.value
            d=node4.value
##            Q={node1,node3},{node2,node4} # I don't believe we're using it at all.
        if g14==max(List) or g23==max(List):
            a=node1.value
            b=node4.value
            c=node2.value
            d=node3.value
##            Q={node1,node4},{node2,node3} # I don't believe we're using it at all.
        to_print=[a,b],[c,d]
        output_file.write(str(to_print)+'\n')
# it appears that that's it.
# -----------------------------------------------------------------------------
def TreeToQuartets(root,leaves):
# this functions receives a tree and returns its quartets
    output_file.write('\n\nAnd here are the tree\'s quartets:\n')
    k=0
    for a in range(len(leaves)):
        for b in range(a):
            for c in range(b):
                for d in range(c):
                    ProduceQuartet(leaves[a],leaves[b],leaves[c],leaves[d])
                    k=k+1
                    if k==100:
                        print('doing '+str([a,b,c,d])+'\n')
                        k=0
# -----------------------------------------------------------------------------
# in the following function we take a tree (in the form of a conected list) and
# convert it to a string representing it.
# since the output of this file in supposed to be a tree that one can plug-in
# into an LGT simulator, we need to write the creation time of each node in a
# tree.
# this practice is unusual when dealing with trees. we're doing it by adding a
# '|'+time to each node.
# thus, the string (1,(2,3)) that represents a tree in the known way (where the
# leaves are 1,2 and 3, 2 and 3 form a cherry and their brother, and the leaf 1,
# are direct descendants of the root) can be replaced by
# (1|2.3,(2|4,3|1.5)|0.7), stating that leaf 1 was created at time 2.3, leaf 2
# was created at time 4, leaf 3 was created at 1.5, and the father of 2 and 3
# was created at time 0.7. we assume the root was created at time 0.
# of course, some sanity check is in order (since no node can be created after
# any one of its descendants). However, the construction process of the random
# tree should take care of that automatically.
def tree_to_string(root, with_time=True):
    if root.son==None:
        string=str(root.value)
        if with_time:
            string += ':' + str(root.time)
    else:
        string='('
        tmp_son=root.son
        while tmp_son.brother!=None: # why looking at the brother? to avoid something like '(1,2,)'.
            substring=tree_to_string(tmp_son, with_time)
            string=string+substring+','
            tmp_son=tmp_son.brother
        substring=tree_to_string(tmp_son, with_time)
        string=string+substring+')'
        if with_time:
            string += ':' + str(root.time)
    return string
# -----------------------------------------------------------------------------
def building_the_tree(num_of_leaves):
    import random
    node=TreeNode()
    node.generation=0
    node.time=0
    leaves_list=[(0,node)]
    leave_times=[0]
    ok_flag=False
    while not ok_flag:
        ok_flag=True
        for j in range(num_of_leaves-1):
            # now we're building the random binary rooted tree.
            # range(m-1) implies m leaves or something.
            time=leave_times.pop(0)
            # I don't mind the variable "time" to be overwritten.
            (time,node)=leaves_list.pop(0)
            T1=random.expovariate(1)
            T2=random.expovariate(1)
            #
            T1=T1*1000000
            T1=int(T1)
            T1=T1/float(1000000)
            T2=T2*1000000
            T2=int(T2)
            T2=T2/float(1000000)
            #
            if (time+min(T1,T2) in leave_times) or\
               (time+max(T1,T2) in leave_times) or\
               T1==T2:
                # in this case we have two leaves/nodes with excatly the same
                # time signature. leaves_list.sort() will lead to
                # an error. Can't continue. reset and redo.
                ok_flag=False
                node=TreeNode()
                node.generation=0
                node.time=0
                leaves_list=[(0,node)]
                leave_times=[0]
                break
            else:
                son_a=TreeNode()
                son_b=TreeNode()
                son_a.brother=son_b
                son_a.father=son_b.father=node
                son_a.generation=son_b.generation=node.generation+1
                node.son=son_a
                son_a.time=time+min(T1,T2)
                son_b.time=time+max(T1,T2)
                
                leaves_list.append((time+min(T1,T2),son_a))
                leaves_list.append((time+max(T1,T2),son_b))
        ##        print(leaves_list)
                leaves_list.sort()

                leave_times.append(time+min(T1,T2))
                leave_times.append(time+max(T1,T2))
                leave_times.sort()
    # now we're tagging the leaves and set the leaves times to make the tree ultra-
    # metric with respect to time.
    for j in range(len(leaves_list)):
        leaves_list[j][1].value=j
        # remember that the minimal time is now stored in leaves_list[0][0]
        leaves_list[j][1].time=leaves_list[0][0]
    # not sure I need this separate leaves list
    leaves=[]
    for j in range(len(leaves_list)):
        leaves.append(leaves_list[j][1])
    # and now it's time to write the tree that was created as a string:
    # first, we're getting hold of the root once again.
    # how? start at a random leaf and climb up.
    root=leaves_list[0][1]
    while not root.generation==0:
        root=root.father
    # and here we're determining what does the tree string looks like.
    tree_str_string_with_time = tree_to_string(root, with_time=True)
    tree_str = tree_to_string(root, with_time=False)
    return tree_str, tree_str_string_with_time
