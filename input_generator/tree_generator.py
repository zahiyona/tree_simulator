import os

from input_generator import building_a_Yule_tree_2


class TreeGenerator:
    def __init__(self, output_tree_prefix="data/simulated_trees/simulated_tree"):
        self._output_tree_prefix = output_tree_prefix
        pass

    def generate(self, ntaxa):
        simulated_tree, simulated_tree_with_time = building_a_Yule_tree_2.building_the_tree(ntaxa)
        output_tree_path = "{}_n{}.tre".format(self._output_tree_prefix, str(ntaxa))
        os.makedirs(os.path.dirname(output_tree_path), exist_ok=True)
        with open(output_tree_path, 'w') as tree_fh:
            tree_fh.write(simulated_tree)

        output_tree_with_time_path = "{}_n{}_w.tre".format(self._output_tree_prefix, str(ntaxa))
        with open(output_tree_with_time_path, 'w') as tree_fh:
            tree_fh.write(simulated_tree_with_time)

        return output_tree_path, output_tree_with_time_path