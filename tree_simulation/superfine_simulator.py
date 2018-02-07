import random
from typing import List

import os

import sys

from externals.phylo import obtain_subtree_with_length, obtain_subtree
from input_generator import building_a_Yule_tree_2
from contextlib import contextmanager


TAXA_FOR_SUBTREE = 15



@contextmanager
def suppress_stdout():
    with open(os.devnull, "w") as devnull:
        old_stdout = sys.stdout
        sys.stdout = devnull
        try:
            yield
        finally:
            sys.stdout = old_stdout

def random_choice_from_list(items: List, count):
    temp_items = items.copy()

    def choose_and_remove(temp_items: List):
        choice = random.choice(temp_items)
        temp_items.remove(choice)
        return choice

    choosen_taxa = [choose_and_remove(temp_items) for i in range(count)]
    return choosen_taxa


def save_tree(tree, path):
    with open(path, 'w') as tree_fh:
        tree_fh.write(tree)


def scm_verify(choosen_taxa_list:List[List[int]]):
    choosen_taxa_list = choosen_taxa_list.copy()
    taxa_set_count = len(choosen_taxa_list)
    if taxa_set_count == 1:
        return True, choosen_taxa_list[0]
    (index1, index2, currentMax) = (None, None, 1)

    for i in range(taxa_set_count - 1):
        for j in range(i + 1, taxa_set_count):
            #count overlap taxa
            overlap = [True if taxon in choosen_taxa_list[j] else False for taxon in choosen_taxa_list[i]].count(True)
            if (overlap > currentMax):
                (index1, index2, currentMax) = (i, j, overlap)
    choosen_taxa_list[index1].extend(choosen_taxa_list[index2])
    choosen_taxa_list[index1] = list(set(choosen_taxa_list[index1]))
    choosen_taxa_list.remove(choosen_taxa_list[index2])
    if currentMax < 4:
        return False, None
    else:
        return scm_verify(choosen_taxa_list)

def create_superfine_subtrees(simulated_tree, simulated_tree_with_time, ntaxa, subtrees_factor):
    good_for_scm = False

    while not good_for_scm:
        trees = ""
        trees_with_time = ""
        choosen_taxa_list = []
        taxa = [str(i) for i in range(ntaxa)]

        while not good_for_scm:
            for i in range(int(ntaxa * subtrees_factor)):
                with suppress_stdout():
                    choosen_taxa = random_choice_from_list(taxa, TAXA_FOR_SUBTREE)
                    choosen_taxa_list.append(choosen_taxa)
                    output_tree = obtain_subtree(simulated_tree, choosen_taxa)
                    output_tree_with_time = obtain_subtree_with_length(simulated_tree_with_time, choosen_taxa)
                    trees += output_tree + ";\n"
                    trees_with_time += output_tree_with_time + ";\n"

            good_for_scm, taxa_set = scm_verify(choosen_taxa_list)

            if not good_for_scm:
                print("tree creation failed because of not enough overlap taxa")
            elif len(taxa_set) != len(taxa):
                good_for_scm = False
                print("tree creation failed because of not full taxa in subtrees")
    # noinspection PyUnboundLocalVariable
    print("count subtrees: {}".format(len(choosen_taxa_list)))
    # noinspection PyUnboundLocalVariable
    return trees, trees_with_time, len(choosen_taxa_list)

def simulate_superfine_tree(simulated_tree, simulated_tree_with_time, ntaxa, subtrees_factor, tree_num=1):
    trees, trees_with_time, subtrees_count = create_superfine_subtrees(simulated_tree, simulated_tree_with_time, ntaxa, subtrees_factor)

    tree_dir = 'data/simulated/{}'.format(subtrees_count)
    tree_w_dir = 'data/simulated_w/{}'.format(subtrees_count)
    os.makedirs(tree_dir, exist_ok=True)
    os.makedirs(tree_w_dir, exist_ok=True)

    model_tree_path = '{}/tree.{}.model_tree'.format(tree_dir, tree_num)
    model_w_tree_path = '{}/tree_w.{}.model_tree'.format(tree_w_dir, tree_num)
    save_tree(simulated_tree, model_tree_path)
    save_tree(simulated_tree_with_time, model_w_tree_path)

    tree_path = '{}/tree.{}.model_source_trees'.format(tree_dir, tree_num)
    tree_w_path = '{}/tree_w.{}.model_source_trees'.format(tree_w_dir, tree_num)
    save_tree(trees, tree_path)
    save_tree(trees_with_time, tree_w_path)


def simulate_superfine_trees():
    ntaxa = 1000
    for subtrees_factor in [0.5,0.75,1.0,1.25,1.5,1.75]:
        for i in range(1):
            print("simulating tree #{}".format(i))
            simulated_tree, simulated_tree_with_time = building_a_Yule_tree_2.building_the_tree(ntaxa)
            simulate_superfine_tree(simulated_tree, simulated_tree_with_time, ntaxa, subtrees_factor, i)

if __name__ == '__main__':
    simulate_superfine_trees()