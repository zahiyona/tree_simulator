import os

from input_generator.generator_base import GeneratorBase, SimulationIteration


class MaxcutGenerator(GeneratorBase):
    def __init__(self, maxcut_path="externals/find-cut",
                 output_maxcut_prefix="data/maxcut_tree/maxcut_tree"):
        self._maxcut_path = maxcut_path
        self._output_maxcut_prefix = output_maxcut_prefix

    def generate(self, simulation_iteration: SimulationIteration):
        output_maxcut_path = "{}_n{}_q{}.tnt".format(self._output_maxcut_prefix, str(simulation_iteration.ntaxa),
                                                     str(simulation_iteration.qnum_factor))
        os.makedirs(os.path.dirname(output_maxcut_path), exist_ok=True)
        log_path = "{}_logs/maxcut_tree_n{}.log".format(self._output_maxcut_prefix, str(simulation_iteration.ntaxa))
        os.makedirs(os.path.dirname(log_path), exist_ok=True)
        command = "{} qrtt={} otre={} > {}".format(
            self._maxcut_path, simulation_iteration.quartets_path, output_maxcut_path, log_path)
        print("### Running system command for maxcut: {} ###".format(command))
        os.system(command)
        print("### System command finished###")
        simulation_iteration.maxcut_tree_path = output_maxcut_path