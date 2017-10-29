import os

from externals.phylo import compute_RF_between_tree_files
from input_generator.generator_base import TreeDistance


class TreeComparator:
    def __init__(self, output_distance_prefix="data/distance/distance", qfit_binary_path="externals/wqrt_agree"):
        self.output_distance_prefix = output_distance_prefix
        self._qfit_binary_path = qfit_binary_path

    def _compare_with_qfit(self, simulation_iteration, ref_tree_path):
        temp_qfit_path = "tmp.qfit"
        command = "{} src_tree={} qrt_file={} out={} qnum_factor={} ntaxa={} weights=ref select=all".format(
            self._qfit_binary_path, ref_tree_path, simulation_iteration.quartets_path, temp_qfit_path,
            simulation_iteration.qnum_factor, simulation_iteration.ntaxa
        )

        print("### Running system command: {} ###".format(command))
        os.system(command)
        print("### System command finished###")

        with open(temp_qfit_path, 'r') as qfit_fh:
            line = qfit_fh.readline()
        os.remove(temp_qfit_path)
        line = line.replace("  ", " ")
        qfit_distance = line.split(" ")[1]
        return qfit_distance

    def compare(self, simulation_iteration, ref_tree_path):
        output_distance_path = "{}_n{}_q{}.txt".format(
            self.output_distance_prefix, str(simulation_iteration.ntaxa), str(simulation_iteration.qnum_factor)
        )
        rf_distance, _, _ = compute_RF_between_tree_files(simulation_iteration.simulated_tree_path, ref_tree_path)

        qfit_distance = self._compare_with_qfit(simulation_iteration, ref_tree_path)

        tree_distance = TreeDistance(rf_distance, qfit_distance)

        os.makedirs(os.path.dirname(output_distance_path), exist_ok=True)
        with open(output_distance_path, 'w') as tree_fh:
            tree_fh.write(str(tree_distance))

        return tree_distance
