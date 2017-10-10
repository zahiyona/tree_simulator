#!/usr/bin/python

import string, sys, re, copy, os, traceback
from numpy import *
from math import *
from subprocess import call
from random import choice
from time import *
#import numpy as np



#-----------------------------------------------------------------------------------_#
def getBase():
    pid = os.getpid()
    progname = os.path.basename(sys.argv[0])
    if re.search(r'\.py$', progname):
        progname = re.sub(r'\.py$',r'', progname)
    else:
        ERR("Invalid progname %s" %progname)

    base = "tmp-"+progname+"-"+str(pid) 

    print("base: %s\n" % base)
    return base



#-----------------------------------------------------------------------------------_#
def revComp (s):
#    print str(s)
    revs = {'A':'T', 'C': 'G', 'G': 'C', 'T': 'A'}
    outS = "".join([{'A':'T', 'C': 'G', 'G': 'C', 'T': 'A'}[s[len(s)-1-i]] for i in range(len (s))])

    
    return outS

#-----------------------------------------------------------------------------------_#

def rmfile(f):
    if  os.access(f, os.F_OK):
        print(f,"exists\n")
        line = "rm " + f
        os.system(line)
    else:
        print(f,"NOT exists\n\n")
                
#-----------------------------------------------------------------------------------_#


def ck_exists(f):
    if  os.access(f, os.F_OK):
       return True 
    else:
        print(f,"NOT exists\n\n")
        return False


#-----------------------------------------------------------------------------------_#
def WARN(msg):
    print("\n\n\nWARNING WARNING WARNING: " + msg +"\n\n\n")


#-----------------------------------------------------------------------------------_#
def ERR(msg):
    print("\n\n\nERROR ERROR ERROR: " + msg +"\n\n\n")
    raise
    traceback.print_exc()
    exit(7)

#-----------------------------------------------------------------------------------_#

def cmd (cmd):
    print(strftime("%a, %d %b %Y %H:%M:%S +0000", localtime()))

    print("UNIX COMMAND: ", cmd)
    sts = os.system(cmd)

    print("After command, rc is ", sts, ". time is ", \
        strftime("%a, %d %b %Y %H:%M:%S +0000", localtime()))
    if (sts != 0 ):
        print("rc ", sts, " after system call\n")
        exit(8)
    



#-----------------------------------------------------------------------------------_#

def find_quartet_topology(tree, q):
    if len(q) !=4:
        ERR("quartet has "+str(len(q))+" elements")
    sp_list = re.findall('[^,()]+',tree)
    for x in q:
        if not x in sp_list:
            ERR("Taxon "+x+" not in tree "+tree)
    induced = str(obtain_subtree(tree,q))
    print("induce", induced)
    if not re.search(r'\(+([^,()]+)[,(]+([^,()]+)[,)(]+\(([^,()]+),([^,()]+)\)',induced):
        print("not1")
        if not re.search(r'\(+([^,()]+)[,(]+\(([^,()]+),([^,()]+)\)[,()]+([^,()]+)',induced):
            print("not2")
            if not re.search(r'\([,(]+\(([^,()]+),([^,()]+)\)[,()]+([^,()]+)[,()]+([^,()]+)',induced):
                print("not3")
                if not re.search(r'([^,()]+),([^,()]+),([,()]+),([,()]+)',induced):
                    ERR("Invalid topology "+induced)
                else:
                    print("unresolved")
                    return 0
            else:
                cherry1 = re.search(r'\([,(]+\(([^,()]+),([^,()]+)\)[,()]+([^,()]+)[,()]+([^,()]+)',induced).\
                    groups()[0:2]
        else:
            cherry1 = re.search(r'\(+([^,()]+)[,(]+\(([^,()]+),([^,()]+)\)[,()]+([^,()]+)',induced).groups()[1:3]
        
    else:
        cherry1 = re.search(r'\(+([^,()]+)[,(]+([^,()]+)[,)(]+\(([^,()]+),([^,()]+)\)',induced).groups()[2:4]
    cherry2 = list (set(q) - set(cherry1))

#    print "quartet is ",cherry1," versus\n",cherry2
    return cherry1[0]+","+cherry1[1]+"|"+cherry2[0]+","+cherry2[1]
                
        

        
    
#-----------------------------------------------------------------------------------_#
def same_quartet(q1, q2):
    if set(q1) != set(q2):
        print("quartets are not over the same taxa:\n",sorted(q1),"\nversus:\n",sorted(q2))
        return -1
    if len(set(q1[:2]) & set(q2[:2])) == 1:
        print("the intersection is not 2 or 0:",(set(q1[:2]) & set(q2[:2])))
        return 0
    else:
        return 1
#-----------------------------------------------------------------------------------_#

def taxa_exists(tree, x):
    sp_list = re.findall('[^,()]+',tree)
    if x in sp_list:
        return 1
    else:
        return 0
    
#-----------------------------------------------------------------------------------_#


def remove_double(tree):
    print("in remove double. Tree:",tree)
    add_paren = 0
    while 1:
        if re.match(r'^[^(),]+$',tree):
            print("arrived at a single taxon", tree, "  returning.")
            return tree
        else:
#        print "not a single taxon \'" +tree +"\'"
            pass
        balance =0
        print("In loop, tree now", tree)
        for i in range(len(tree)):
            if tree[i] ==',' :
    #            print "found a \',\' balance ", balance
                if balance ==0:
    #                print "balance is zero. recursing for ", tree[:i], "and", tree[i+1:]
                    t1 = remove_double(tree[:i])
                    t2 = remove_double(tree[i+1:])
    #                print "back from split. returning ("+t1+","+t2+")"
                    if add_paren:
                        print("got to the end, must add paren")
                        return "("+t1+","+t2+")"
                    else:
                        print("did NOT get to the end, no paren")
                        return t1+","+t2
                else:
    #                print "balance is not zero. ignore"
                    pass
            if tree[i] == '(':
                balance +=1
    #            print "found \'(\', balance", balance
            if tree[i] == ')':
                balance -=1
    #            print "found \')\', balance", balance
                if balance < 0:
                    ERR("Balance = "+str(balance)+ " at "+str(i))
        else:
            if tree[i] !=')' or balance != 0:
                ERR("got to the end but found "+ tree[i])
    #        print "got to the end but found "+ tree[i]
    #        print "recursing with ", tree[1:-1]
            tree = tree[1:-1]
            add_paren = 1


#-----------------------------------------------------------------------------------_#
def count_edges(tree):
#    print "in count edges. Tree:",tree
    if tree[-1] == ';':
#        print "found ; at end of tree.",tree," len", len(tree)
        tree = tree[0:-1]
#        print "After remove ; at end of tree.",tree," len", len(tree)
#    exit(8)
    if tree[0] != '(' or tree[-1] != ')':
        ERR("Invalid tree. Is not bounded by parentheses. "+tree)

    count = tree.count("(")
#    print "number of (", count
    return count - 1 -  binary_root(tree)

#-----------------------------------------------------------------------------------_#


def binary_root(tree):
#    print "in binary root. Tree:",tree
    count = 0
    balance = 0
    if tree[-1] == ';':
#        print "found ; at end of tree.",tree," len", len(tree)
        tree = tree[0:-1]
#        print "After remove ; at end of tree.",tree," len", len(tree)
#    exit(8)
    if tree[0] != '(' or tree[-1] != ')':
        ERR("Invalid tree. Is not bounded by parentheses. "+tree)
    tree = tree[1:-1]
    for i in range(len(tree)):
        if tree[i] ==',' :
#            print "found a \',\' balance ", balance
            if balance ==0:
#                print "balance is zero. raising count to ",count+1
                count+= 1
            else:
#                print "balance is not zero. ignore"
                1
        if tree[i] == '(':
            balance +=1
#            print "found \'(\', balance", balance
        if tree[i] == ')':
            balance -=1
#            print "found \')\', balance", balance
            if balance < 0:
                ERR("Balance = "+str(balance)+ " at "+str(i))

#    print "At end of tree, count ", count
    if count == 0:
        ERR("count is zero at binary root for  "+ tree[i])
    elif count == 1:
        return True
    else: return False


#-----------------------------------------------------------------------------------_#

def obtain_subtree(tree, sub_list):
    print("in obtain_subtree.\ntree:",tree,"sp list:\n", sub_list)
#    exit(8)
    if tree[-1] == ';':
#        print" tree contains ;"
        tree = tree[:-1]
#        print" tree after removal ;", tree

    
    sp_list = re.split('[,()]',tree)
    while '' in sp_list:
        sp_list.remove('')

    for sp in sp_list:
#        print "loking for sp", sp
        if not sp in sub_list:
#            print sp, "is not in the sub list"
            sp = re.sub('([|*.\[\]\(\)])','\\\\\g<1>',sp)   # this to allow special characters in the name

            tree  = re.sub(r'([(),])'+sp+r'([(),])','\g<1>\g<2>',tree)
#            print "after removal", tree
        else:
#            print sp, "is in the sub list"
            1
#    print "\n\nclean tree", tree

    while 1:
#        print "tree now", tree, "len", len(tree)
        if re.search(r'\(,*([^,()]+)\)',tree):
#            print "found orphan taxon",  re.search(r'\(,*([^,()]+)\)',tree).group(1)
            tree = re.sub(r'\(,*([^,()]+)\)','\g<1>',tree)
            continue
        if re.search(r'\(,*\)',tree):
#            print "found empty subtree",  re.search(r'\(,*\)',tree).group(0)
            tree = re.sub(r'\(,*\)','',tree)
            continue
        if re.search(r',,+',tree):
#            print "found multiple commas",  re.search(r',,+',tree).group(0)
            tree = re.sub(r',,+',',',tree)
            continue
        if re.search(r',+\)',tree):
#            print "found  commas before )",  re.search(r',+\)',tree).group(0)
            tree = re.sub(r',+\)',')',tree)
            continue
        if re.search(r'\(,+',tree):
#            print "found  commas after (",  re.search(r'\(,+',tree).group(0)
            tree = re.sub(r'\(,+','(',tree)
            continue
#        print "No subsitution found", tree, "break"
        break
#    print "\n\nfinal tree", tree
    t = remove_double(tree)
#    print "\n\n\nAfter re4move double:\n", t
    return t

#--------------------------------------------------------------------------------------#

def obtain_subtree_with_length(tree, sub_list):
    print("\n\nin obtain_subtree with length.\ntree:",tree,"sp list:\n", sub_list)
    if tree[-1] == ';':
#        print" tree contains ;"
        tree = tree[:-1]
#        print" tree after removal ;", tree

    temp_t =  re.sub(r':[-+]?\d*(.\d+)?',r'',tree) #remove length
    sp_list = re.findall('[^,()]+',temp_t)

    for sp in sp_list:

        print("loking for sp", sp)
        if not sp in sub_list:
            print(sp, "is not in the sub list")
            sp = re.sub('([|*.\[\]\(\)])','\\\\\g<1>',sp)   # this to allow special characters in the name
            print("sp_ before removal", sp)
            print("before removal sp",sp,":", tree)
            tree = re.sub(r'([(),])'+sp+r'(:[-+]?\d*(.\d+)?)?'+r'([(),])','\g<1>\g<4>',tree)
            print("after removal", tree)
        else:
            print(sp, "is in the sub list")
            pass
    print("\n\nclean tree", tree)
#    exit(8)
    while 1:
        print("tree now", tree, "len", len(tree))
        if re.search(r'\(,+([^,()]+)(:[-+]?\d*(\.\d+)?)?\)',tree):
            print("found orphan taxon",  re.search(r'\(,*([^,()]+)(:[-+]?\d*(\.\d+)?)?\)',tree).group(0))
            tree = re.sub(r'\(,*([^,()]+)(:[-+]?\d*(\.\d+)?)?\)','(\g<1>)',tree)
            print("after substitute", tree)
#            exit(8)
            continue
        if re.search(r'\(,*\)(:[-+]?\d*(\.\d+)?)?',tree):
            print("found empty subtree",  re.search(r'\(,*\)(:[-+]?\d*(\.\d+)?)?',tree).group(0))
            tree = re.sub(r'\(,*\)(:[-+]?\d*(\.\d+)?)?','',tree)
            print("after substitute", tree)
#            exit(8)
            continue
        if re.search(r',,+',tree):
#            print "found multiple commas",  re.search(r',,+',tree).group(0)
            tree = re.sub(r',,+',',',tree)
            continue
        if re.search(r',+\)',tree):
#            print "found  commas before )",  re.search(r',+\)',tree).group(0)
            tree = re.sub(r',+\)',')',tree)
            continue
        if re.search(r'\(,+',tree):
#            print "found  commas after (",  re.search(r'\(,+',tree).group(0)
            tree = re.sub(r'\(,+','(',tree)
            continue
#        print "No subsitution found", tree, "break"
        if re.match(r'\(([^,()]+)(:[-+]?\d*(\.\d+)?)?\):[-+]?\d*(\.\d+)?',tree):
            print("found orphan taxonwith two lens", \
                re.match(r'\(([^,()]+):[-+]?\d*(\.\d+)?\):[-+]?\d*(\.\d+)?',tree).group(0))
            exit(8)
            continue
        break
    print("\n\nfinal tree", tree)
    return tree

#-----------------------------------------------------------------------------------_#

def compute_RF_between_tree_files(f_tree1, f_tree2):
    t1 = read_tree(f_tree1)
    t2 = read_tree(f_tree2)
    print("in compte RF .\ntree1:",t1,"\ntree2:", t2)
    RF, n_edges1, n_edges2 = compute_RF_between_trees(t1, t2)

    print(" RF, n_edges1, n_edges2" ,  RF, n_edges1, n_edges2)
    return  RF, n_edges1, n_edges2
    if (n_edges1+n_edges2) > 0:
        print("%RF", float(RF)/float (n_edges1+n_edges2)) 
    else: 1
#-----------------------------------------------------------------------------------_#

def compute_RF_between_trees(tree1, tree2):
    print("in compte RF .\ntree1:",tree1,"\n\ntree2:", tree2)
#    tree1 =  re.sub(r':[-+]?\d*(.\d+)?',r'',tree1) #remove length
#    tree2 =  re.sub(r':[-+]?\d*(.\d+)?',r'',tree2) #remove length
    temp_t = tree1
    temp_t = re.sub(r'\)[-+]?\d*(\.\d+)?\s*',r')', temp_t) #remove bootstrap value
    temp_t = re.sub(r':[-+]?\d*(\.\d+)?\s*$',r'',temp_t) #remove length at the end
    temp_t =  re.sub(r':[-+]?\d*(\.\d+)?(e-\d+)?',r'',temp_t) #remove length
    tree1 = temp_t
    print("tree1 after remove len and btp", tree1)

    temp_t = tree2
    temp_t = re.sub(r'\)[-+]?\d*(\.\d+)?\s*',r')', temp_t) #remove bootstrap value
    temp_t = re.sub(r':[-+]?\d*(\.\d+)?\s*$',r'',temp_t) #remove length at the end
    temp_t =  re.sub(r':[-+]?\d*(\.\d+)?(e-\d+)?',r'',temp_t) #remove length
    tree2 = temp_t
    print("tree2 after remove len and btp", tree2)

    tree1 =  re.sub(r';\s*$',r'',tree1) #remove ;

    tree2 =  re.sub(r';\s*$',r'',tree2) #remove ;

    sp_list1 = re.findall('[^,()]+',tree1)
    sp_list2 = re.findall('[^,()]+',tree2)

    print("\nsp_list1", sp_list1)
    print("\nsp_list2", sp_list2)
    common = set(sp_list1) & set(sp_list2)
    print("Common sps:", common)
#    exit(8)
    if not set(sp_list1) >= common:
        ERR("Problem with sp_list and common species:"+ ",".join(sp_list1)+" and common "+",".join(common))
    sub_tree1 = obtain_subtree(tree1, common)
    if sub_tree1[-1] != ';':
        sub_tree1 +=";"

    if not set(sp_list2) >= common:
        ERR("Problem with sp_list and common species:"+ ",".join(sp_list2)+" and common "+",".join(common))
    sub_tree2 = obtain_subtree(tree2, common)
    if sub_tree2[-1] != ';':
        sub_tree2 +=";"
    print("\nFinding dist between:\n",sub_tree1,"\nand:\n",sub_tree2)
    RF = obtain_RF (sub_tree1, sub_tree2)
    n_edges1 = count_edges(sub_tree1)
    n_edges2 = count_edges(sub_tree2)
#    print "#edges1 ", n_edges1, "#edges2 ", n_edges2
    common_edges = (n_edges1 + n_edges2 - int(RF))/2
#    print "common_edges between\n",sub_tree1,"\nand \n",sub_tree2,"\n", common_edges
    return  RF, n_edges1, n_edges2

#-----------------------------------------------------------------------------------_#


def obtain_RF(tree1, tree2):
    
    if not  re.match(r'([(),]+[\w:\.]+)+\)+;?',tree1):
        ERR ("Invalid tree1 \n"+tree1)

    if not  re.match(r'([(),]+[\w:\.]+)+\)+;?',tree2):
        ERR ("Invalid tree2 \n"+tree2)

    if not re.search(r';\s*$',tree1):
        tree1 += ';'
    if not re.search(r';\s*$',tree2):
        tree2 += ';'

    intree = open ("intree",'w')
    intree.write(tree1)
    intree.close()
    
    intree = open ("intree2",'w')
    intree.write(tree2)
    intree.close()
    
    fparms = open ("phylip-param.txt",'w')
    print("d", file=fparms)
    print("2", file=fparms)
    print("c", file=fparms)
    print("s", file=fparms)
    print("y", file=fparms)
    fparms.close()
    rmfile("outfile")   #remove the output file
    stat = os.system("phylip treedist < phylip-param.txt > dist.out");
    print("stat after treedist ", stat)
    if stat:
        ERR("Invalid status after treedist")
    if not ck_exists("outfile"):
        ERR("file outfile was not created\n\n")
        
    distf = open ("outfile") 
    res_line = distf.readline()
    print("phylip output: res_line\n");
    m1 = re.match(r'1\s+(\d+)', res_line)
    if not m1:
        ERR( "invalid line at phylip output: "+res_line)
      
    print("phylip output: ", res_line)
    RFdist = (int) (m1.group(1))
    return RFdist


#-----------------------------------------------------------------------------------_#
# This function reads a tree
def read_tree(tree_file):
    tf = open(tree_file)
    tree = ""
    for line in tf:
        tree += line.strip()
    return tree


#-----------------------------------------------------------------------------------_#

def read_dist_mat(f_mat):
    mf = open(f_mat)
    line = mf.readline()
    m = re.match(r'^\s*(\d+)\s*$',line)
    if not m:
        ERR("invalid header line in dist mat:"+line)
    sp_n = int(m.group(0))
    dist_mat=[]
    print("line ", line, "sp_n ", sp_n)
    line = mf.readline()
    while (line != ""):
        print("line ", line)
        m =  re.match(r'^\s*(\d+)\s*(([+-]?\d*\.\d+\s+)+)$',line)
        if not m:
            ERR ("Invalid dist line "+line)
        sp = m.group(1)
        print("sp ", sp, m.group(2))
        dist_array = []
        m1 = re.findall(r'\d*\.\d+',m.group(2))
        dist_array.extend(m1)
        print("dist arr", dist_array)
        while (len(dist_array) < sp_n):
            print("in loop")
            line = mf.readline()
            print("line ", line)
            m =  re.match(r'^\s*((\d*\.\d+\s+)+)$',line)
            if not m:
                ERR ("Invalid dist line "+line)
            m1 = re.findall(r'\d*\.\d+',line)
            dist_array.extend(m1)
            print("dist arr", len(dist_array),  dist_array)
        print("final dist arr", dist_array)
        dist_float = [float (dist_array[i]) for i in range(len(dist_array))]
        print("float ", dist_float)
        dist_mat.append(dist_float)
        line = mf.readline()
        
#    print "\n\n\nFINAL mat\n\n"
#    for i in range(sp_n):
#        print "line", i, dist_mat[i]
        
    return dist_mat
#-----------------------------------------------------------------------------------_#



def addLength(tree, std):
    target = ""
    for i in range(len(tree)):
        if tree[i] in[')',',']:
#            target += ":%3.4f" %(random.random())
            while True:
                e_len = np.random.normal(1,std)/2
                if  e_len > 0 and e_len < 1:
                    break
            target += ":%3.4f" %(  e_len)
        target += tree[i]
    return target

#-----------------------------------------------------------------------------------_#

def genAllTrees(tree, n, final, trees):
    chars = [str(i) for i in range(10)]
    chars.append(')')
    inserted = 0
    for i in range(len(tree)):
        print("in loop",i, tree[i])
        if  tree[i] in [',',')']:
            print("found delimit",  tree[i], "at ", i)
            print("inserting a taxon ", n, " before position", i, tree[i-1:i+2])
            balance = 0
            for  back in range(i-1,-1,-1):
                print("back", back, tree[back])
                if  tree[back] in [',','(']:
                    print("Found ",  tree[back], end=' ')
                    if balance == 0:
                        print("and also balance is zero. escaping...")
                        break
                    else:
                        print("but balance is ", balance, ". continuing")
                if  tree[back] == ')':
                    print("increasing balance to ", balance+1)
                    balance += 1
                if  tree[back] == '(':
                    print("decreasing balance to ", balance-1)
                    balance -= 1
            else:
                ERR("arrived at 0 while searching for a match from "+ \
                        str(back)+" in tree "+tree)
            print("faound a match at ", back, tree[back])
            tree_n = tree[:back+1]+'('+tree[back+1:]
            print("tree after inserting (", tree_n)
            tree_n = tree_n[:i+1]+')'+tree_n[i+1:]
            tree_n = tree_n[:i+1]+','+tree_n[i+1:]
            print("tree after inserting )", tree_n)
            tree_n = tree_n[:i+2]+str(n)+tree_n[i+2:]
            print("tree after inserting ",n, tree_n)
            inserted += 1
            if n < final:
                genAllTrees(tree_n, n+1, final, trees)
            else:
                print("Inserting a new tree ", tree_n)
                trees.append(tree_n)
                print("trees now ", trees)

    else:
        print("Inserted ", inserted, "times")

#
#-----------------------------------------------------------------------------------_#

def randTree(tree, n, final):
    places_left = (2*n-3)
    chars = [str(i) for i in range(10)]
    chars.append(')')
    while True:
        for i in range(len(tree)):
            print("in loop",i, tree[i])
            if  tree[i] in [',',')']:
                print("found delimit",  tree[i], "at ", i)
                if places_left == 0:
                    ERR("Arrived at zero at places left")
                p = 1./places_left
                if random.random()<p:
                    places_left -= 1
                    print("inserting a taxon ", n, " before position", i, tree[i-1:i+2])
                    balance = 0
                    for  back in range(i-1,-1,-1):
                        print("back", back, tree[back])
                        if  tree[back] in [',','(']:
                            print("Found ",  tree[back], end=' ')
                            if balance == 0:
                                print("and also balance is zero. escaping...")
                                break
                            else:
                                print("but balance is ", balance, ". continuing")
                        if  tree[back] == ')':
                            print("increasing balance to ", balance+1)
                            balance += 1
                        if  tree[back] == '(':
                            print("decreasing balance to ", balance-1)
                            balance -= 1
                    else:
                        ERR("arrived at 0 while searching for a match from "+ \
                                str(back)+" in tree "+tree)
                    print("faound a match at ", back, tree[back])
                    tree = tree[:back+1]+'('+tree[back+1:]
                    print("tree after inserting (", tree)
                    tree = tree[:i+1]+')'+tree[i+1:]
                    tree = tree[:i+1]+','+tree[i+1:]
                    print("tree after inserting )", tree)
                    tree = tree[:i+2]+str(n)+tree[i+2:]
                    print("tree after inserting ",n, tree)
                    if n < final-1:
                        return randTree(tree, n+1, final)
                    else:
                        return tree
                else:
                    print("random is bigger than ", p)
            else:
                1
#                print "NOT in char"
#

#-----------------------------------------------------------------------------------_#
def tran_names_to_numbers(tree):
    print("in tran_names_to_numbers. Tree ", tree)
    temp_t =  re.sub(r':[-+]?\d*(.\d+)?',r'',tree) #remove length
    temp_t =  re.sub(r';$',r'',temp_t) #remove ;
    sp_list = re.findall('[^,()]+',temp_t)
    sp_list = sorted(sp_list)
    print("sp_list:\n", "\n".join(sp_list))
    sub1 = tree
    
    for i in range(len(sp_list)):
        print("\n\n",i,"\t",sp_list[i])
        pat = '([,()])('+sp_list[i]+')([:,()])'
        if re.search(pat,sub1):
            print("matched \n", re.search(r'(.*)'+pat,sub1).group(1))
            print("matched ", re.search(pat,sub1).groups())
            print("before sub:\n",sub1)
            sub1,cnt = re.subn('([,()])('+sp_list[i]+')([:,()])','\g<1>'+str(i)+'\g<3>',sub1)
            if cnt != 1:
                ERR ("replaced "+str(cnt)+" times")
            print("Substituted:", cnt,"After sub:\n",sub1)
        else:
            ERR("could not find "+pat)

    temp_t =  re.sub(r':[-+]?\d*(.\d+)?',r'',sub1) #remove length
    temp_t =  re.sub(r';$',r'',temp_t) #remove ;
    temp_list = re.findall('[^,()]+',temp_t)
    temp_list.sort(lambda a,b: cmp(int(a), int(b)))
#    print "\n\nsorted :\n","\n".join(temp_list)
    s = [str(i) for i in range(len(temp_list))]
#    print "\n\nsorted 2:\n","\n".join(s)
    if temp_list != s:
        ERR("String differ:"+','.join(temp_list)+" and \n"+','.join(s))
    
    return sub1, sp_list


#-----------------------------------------------------------------------------------_#
def restore_names_from_numbers_partial(tree, sp_list):
    print("in restore_names_from_numbers_partial. \nTree\n ", tree)
    temp_t =  re.sub(r':[-+]?\d*(.\d+)?',r'',tree) #remove length
    temp_t =  re.sub(r';$',r'',temp_t) #remove ;
    sp_list_num = re.findall('[^,()]+',temp_t)

    for i in sp_list_num:
        print("\n\n",i)
        pat = '([,()])('+str(i)+')([:,()])'
        if re.search(pat,tree):
            print("matched \n", re.search(r'(.*)'+pat,tree).group(1))
            print("matched ", re.search(pat,tree).groups())
            print("before sub:\n",tree)
            tree,cnt = re.subn('([,()])('+str(i)+')([:,()])','\g<1>'+sp_list[int(i)]+'\g<3>',tree)
            if cnt != 1:
                ERR ("replaced "+str(cnt)+" times")
                print("Substituted:", cnt,"After sub:\n",tree)
        else:
            ERR("could not find "+pat)

    return tree

#-----------------------------------------------------------------------------------_#

#-----------------------------------------------------------------------------------_#
def restore_names_from_numbers(tree, sp_list):

    for i in range(len(sp_list)):
        print("\n\n",i,"\t",sp_list[i])
        pat = '([,()])('+str(i)+')([:,()])'
        if re.search(pat,tree):
            print("matched \n", re.search(r'(.*)'+pat,tree).group(1))
            print("matched ", re.search(pat,tree).groups())
            print("before sub:\n",tree)
            tree,cnt = re.subn('([,()])('+str(i)+')([:,()])','\g<1>'+sp_list[i]+'\g<3>',tree)
            if cnt != 1:
                ERR ("replaced "+str(cnt)+" times")
                print("Substituted:", cnt,"After sub:\n",tree)
        else:
            ERR("could not find "+pat)

    return tree

#-----------------------------------------------------------------------------------_#

def JC(hamming):
    return -3.0/4*log(1-4.*hamming/3)
 

#-----------------------------------------------------------------------------------_#

def invJC(dist):
    return 3.0/4*(1-exp(-4*dist/3))
 
