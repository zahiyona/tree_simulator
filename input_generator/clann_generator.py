import os

import time

from input_generator.generator_base import GeneratorBase, SimulationIteration

class ClannGenerator(GeneratorBase):
    def __init__(self, output_tree_prefix="data/clann_trees/clann_tree", clann_binary_path="externals/clann/clann"):
        self._output_tree_prefix = output_tree_prefix
        self._clann_binary_path = clann_binary_path
        self._clan_input_prefix = "{}_input/clann_input".format(output_tree_prefix)
        self._tmp_clann_path = 'tmp_clann'
        pass

    def _generate_input_file(self, quartets_path, ntaxa):
        clann_input_path = "{}_n{}.cln".format(self._clan_input_prefix, str(ntaxa))
        os.makedirs(os.path.dirname(clann_input_path), exist_ok=True)
        with open(clann_input_path, 'w') as input_fh:
            input_fh.write('#NEXUS\n')
            input_fh.write('Begin trees; [Test Tree file in nexus format]\n')
            quartets = self._extract_quartets(quartets_path)
            for quartet_index, quartet in enumerate(quartets):
                input_fh.write('tree t{} = [&U] (({},{}),({},{}));\n'.format(quartet_index, quartet[0], quartet[1], quartet[2], quartet[3]))
            input_fh.write('End;\n\n')
            input_fh.write('Begin Clann;\n')
            input_fh.write('set criterion=dfit;\n')
            input_fh.write('hs nreps=3 sample=5 swap=spr maxswaps=1000 savetrees='+self._tmp_clann_path+';\n')
            input_fh.write('quit;\n')
            input_fh.write('endblock;')
        return clann_input_path

    def generate(self, simulation_iteration: SimulationIteration):
        clann_input_path = self._generate_input_file(simulation_iteration.quartets_path, simulation_iteration.ntaxa)
        output_tree_path = "{}_n{}.tre".format(self._output_tree_prefix, str(simulation_iteration.ntaxa))

        command = "{} {}".format(self._clann_binary_path, clann_input_path)
        print("### Running system command: {} ###".format(command))
        os.system(command)
        print("### System command finished###")

        with open(self._tmp_clann_path,'r') as tmp_clann_fh:
            tmp_tree = tmp_clann_fh.readlines()[0]
            clann_tree = tmp_tree.split(';')[0]
        #os.remove(self._tmp_clann_path)
        os.remove('supertree.ps')
        os.makedirs(os.path.dirname(output_tree_path), exist_ok=True)
        with open(output_tree_path, 'w') as tree_fh:
            tree_fh.write(clann_tree)

        simulation_iteration.clann_data.running_time = time.time() - self._start_time
        simulation_iteration.clann_data.tree_path = output_tree_path