import sys

import re

from .phylo import ERR, ck_exists, read_tree, random, same_quartet
from .tree import TreeNode, insertSubTree, get_quartet_from_leaves

def compare_quartet_trees(tree1_fn, tree2_fn, qrt_cnt):

    print("Parameters: tree1: %s, tree2: %s, qrt: %d\n\n" %(tree1_fn, tree2_fn, qrt_cnt))

    #exit(8)
    if not ck_exists(tree1_fn):
        ERR ("file "+tree1_fn+" does not exist")

    if not ck_exists(tree2_fn):
        ERR ("file "+tree2_fn+" does not exist")

    t1 = read_tree(tree1_fn)
    t2 = read_tree(tree2_fn)



    TreeNode.count =0
    t1_leaves = {}
    t1 =  re.sub(r';',r'',t1) #remove ;
    t1_root = insertSubTree(t1,t1_leaves)

    temp =  re.sub(r':[-+]?\d*(.\d+)?',r'',t1) #remove length
    temp =  re.sub(r'\[\d+\]',r'',temp) #remove internal  node names
    temp =  re.sub(r';',r'',temp) #remove ;
    sp_list1 = re.findall('[^,()]+',temp)


    TreeNode.count =0
    t2_leaves = {}
    t2 =  re.sub(r';',r'',t2) #remove ;
    t2_root = insertSubTree(t2,t2_leaves)

    temp =  re.sub(r':[-+]?\d*(.\d+)?',r'',t2) #remove length
    temp =  re.sub(r'\[\d+\]',r'',temp) #remove internal  node names
    temp =  re.sub(r';',r'',temp) #remove ;
    sp_list2 = re.findall('[^,()]+',temp)

    if set(sp_list2) != set(sp_list1):
        ERR("Different set of species:\n"+str(sorted(sp_list1))+"\nand\n"+str(sorted(sp_list2)))

    n= len(sp_list2)

    cnt_agree = 0;


    print("\n\n\nn = %d" %n)

    for i in range(qrt_cnt):
        q = [random.randint(n) for i in range(4)]
        while True:
            if q[0] != q[1] and q[0] != q[2] and q[0] != q[3] and q[1] != q[2] and q[1] != q[3] and  q[2] != q[3]:
                break
            q = [random.randint(n)for i in range(4)]

        print("found distinct q ", q)
        print("leaves are ", [t1_leaves[sp_list1[i]].name for i in q])
    #    exit(8)

        q1 = get_quartet_from_leaves(t1_leaves, [t1_leaves[sp_list1[i]].name for i in q])

        print("returned quartet: ",  q1)
        q2 = get_quartet_from_leaves(t2_leaves, [t2_leaves[sp_list1[i]].name for i in q])

        if q1 == None or q2 == None:
            if q1 == None and q2 == None:
                cnt_agree +=1
            continue

        if same_quartet(q1, q2):
            print("\n\n\nSame quartet ", q1, q2)
            cnt_agree +=1
        else:
            print("\n\n\nNOT same quartet ", q1, q2)

    #    exit(8)

    print("quartets sampled: %d, agree %d, %%agree %4.2f\n" %(qrt_cnt,cnt_agree, float(cnt_agree)/qrt_cnt))

    return qrt_cnt, cnt_agree, float(cnt_agree)/qrt_cnt


if __name__ == '__main__':

    if len( sys.argv) != 4:
        ERR("Must suply two tree files and qrt count")

    tree1_fn  = sys.argv[1]

    tree2_fn  = sys.argv[2]

    qrt_cnt = int (sys.argv[3])

    _,_,_ = compare_quartet_trees(tree1_fn, tree2_fn, qrt_cnt)
