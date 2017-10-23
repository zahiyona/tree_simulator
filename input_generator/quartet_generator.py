import os
from input_generator.generator_base import GeneratorBase, SimulationIteration


class QuartetGenerator(GeneratorBase):
    def __init__(self, quartet_generator_path="externals/tree_quartets", output_quartet_prefix="data/quartets/quartet"):
        self._quartet_generator_path = quartet_generator_path
        self._output_quartet_prefix = output_quartet_prefix

    def generate(self, simulation_iteration: SimulationIteration):
        output_quartet_path = "{}_n{}_q{}.{}qrt".format(self._output_quartet_prefix, str(simulation_iteration.ntaxa),
                                                        simulation_iteration.qnum_factor,
                                                        "w" if simulation_iteration.weights else "")
        os.makedirs(os.path.dirname(output_quartet_path), exist_ok=True)
        command = "{} tree_file={} out_qrt_file={} ntaxa={} weights={} qnum_factor={} select=all".format(
            self._quartet_generator_path, simulation_iteration.simulated_tree_w_path, output_quartet_path,
            simulation_iteration.ntaxa, "on" if simulation_iteration.weights else "off", simulation_iteration.qnum_factor
        )
        print("### Running system command: {} ###".format(command))
        os.system(command)
        print("### System command finished###")
        simulation_iteration.quartets_path = output_quartet_path
