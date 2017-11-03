import os
import re
import time
import fileinput

from input_generator.generator_base import GeneratorBase, SimulationIteration


class PaupGenerator(GeneratorBase):
    def __init__(self, output_tree_prefix="data/paup_trees/paup_tree", paup_dir_path="externals/paup"):
        super().__init__()
        self._output_tree_prefix = output_tree_prefix
        self._paup_dir_path = paup_dir_path
        self._paup_input_prefix = "{}_input/paup_input".format(output_tree_prefix)
        self._tmp_clann_path = 'tmp_clann'

    def _generate_input_file(self, simulation_iteration):
        paup_input_path = "{}_n{}_q{}.dat".format(self._paup_input_prefix, str(simulation_iteration.ntaxa),
                                                  str(simulation_iteration.qnum_factor))
        perl_script_path = "{}/mrp-mat-from-quartets.pl".format(self._paup_dir_path)
        os.makedirs(os.path.dirname(paup_input_path), exist_ok=True)
        command = "perl {} {} {} {}".format(
            perl_script_path, simulation_iteration.quartets_path, 9999, paup_input_path
        )
        print("### Running system command: {} ###".format(command))
        os.system(command)
        print("### System command finished###")
        return paup_input_path

    def generate(self, simulation_iteration: SimulationIteration):
        paup_input_path = self._generate_input_file(simulation_iteration)
        output_tree_path = "{}_n{}_q{}.tre".format(self._output_tree_prefix, str(simulation_iteration.ntaxa),
                                                   str(simulation_iteration.qnum_factor))

        command = "perl {}/run_mrp.pl {} {} {}".format(self._paup_dir_path, paup_input_path, output_tree_path, self._paup_dir_path)
        print("### Running system command: {} ###".format(command))
        os.system(command)
        print("### System command finished###")

        with fileinput.FileInput(output_tree_path, inplace=True) as file:
            for line in file:
                print(re.sub(r'\)\d*\.?\d*', ')', line[:-2]), end='')

        os.remove('PAUP.out')

        simulation_iteration.paup_data.running_time = time.time() - self._start_time
        simulation_iteration.paup_data.tree_path = output_tree_path