import os

from input_generator.generator_base import SimulationIteration, GeneratorBase


class TntMatrixGenerator(GeneratorBase):
    def __init__(self, quartet_to_matrix_converter_path="externals/tnt-mat-from-wquartets.pl",
                 output_tnt_prefix="data/tnt_matrix/tnt_matrix"):
        self._quartet_to_matrix_converter_path = quartet_to_matrix_converter_path
        self._output_tnt_prefix = output_tnt_prefix

    def generate(self, simulation_iteration: SimulationIteration):

        output_tnt_matrix_path = "{}_n{}_q{}.tnt".format(self._output_tnt_prefix, str(simulation_iteration.ntaxa),
                                                         str(simulation_iteration.qnum_factor))
        os.makedirs(os.path.dirname(output_tnt_matrix_path), exist_ok=True)
        command = "{} {} {}".format(
            self._quartet_to_matrix_converter_path, simulation_iteration.quartets_path, simulation_iteration.qnum_factor)
        print("### Running system command: {} ###".format(command))
        os.system(command)
        print("### System command finished###")
        simulation_iteration.tnt_matrix_path = output_tnt_matrix_path