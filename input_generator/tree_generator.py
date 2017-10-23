import os

from input_generator import building_a_Yule_tree_2
from input_generator.generator_base import GeneratorBase, SimulationIteration


class TreeGenerator(GeneratorBase):
    def __init__(self, output_tree_prefix="data/simulated_trees/simulated_tree"):
        self._output_tree_prefix = output_tree_prefix
        pass

    def generate(self, simulation_iteration: SimulationIteration):
        simulated_tree, simulated_tree_with_time = building_a_Yule_tree_2.building_the_tree(simulation_iteration.ntaxa)
        output_tree_path = "{}_n{}.tre".format(self._output_tree_prefix, str(simulation_iteration.ntaxa))
        os.makedirs(os.path.dirname(output_tree_path), exist_ok=True)
        with open(output_tree_path, 'w') as tree_fh:
            tree_fh.write(simulated_tree)

        output_tree_with_time_path = "{}_n{}_w.tre".format(self._output_tree_prefix, str(simulation_iteration.ntaxa))
        with open(output_tree_with_time_path, 'w') as tree_fh:
            tree_fh.write(simulated_tree_with_time)

        simulation_iteration.simulated_tree_path = output_tree_path
        simulation_iteration.simulated_tree_w_path = output_tree_with_time_path