import json
from externals import jsonpickle
import os

from input_generator.clann_generator import ClannGenerator
from input_generator.paup_generator import PaupGenerator
from input_generator.tree_comparator import TreeComparator
from input_generator.generator_base import SimulationIteration
from input_generator.maxcut_generator import MaxcutGenerator
from input_generator.quartet_generator import QuartetGenerator
from input_generator.spr_generator import SprGenerator
from input_generator.tnt_matrix_generator import TntMatrixGenerator
from input_generator.tnt_tree_genertor import TntTreeGenerator
from input_generator.tree_generator import TreeGenerator


class TreeSimulator:
    def run(self):
        qnum_factors = [1.2, 1.4, 1.6, 1.8, 2, 2.2, 2.4, 2.6, 2.8]
        min_taxa = 10
        max_taxa = 101
        taxa_jump = 20
        simulation_iterations = [SimulationIteration(taxa, round(q, 1)) for q in qnum_factors for taxa in range(min_taxa, max_taxa, taxa_jump)]
        generators = [
            TreeGenerator(),
            QuartetGenerator(),
            TntMatrixGenerator(),
            TntTreeGenerator(),
            PaupGenerator(),
            MaxcutGenerator(),
            #ClannGenerator(),
            #SprGenerator()
            ]

        paup_tree_comparator = TreeComparator("data/paup_distance/paup_distance")
        tnt_tree_comparator = TreeComparator("data/tnt_distance/tnt_distance")
        maxcut_tree_comparator = TreeComparator("data/maxcut_distance/maxcut_distance")
        clann_tree_comparator = TreeComparator("data/clann_distance/clann_distance")
        spr_tree_comparator = TreeComparator("data/spr_distance/spr_distance")

        for simulation_iteration in simulation_iterations:
            for generator in generators:
                generator.set_start_time()
                generator.generate(simulation_iteration)

            simulation_iteration.paup_data.distance = paup_tree_comparator.compare(
                simulation_iteration, simulation_iteration.paup_data.tree_path
            )

            simulation_iteration.tnt_data.distance = tnt_tree_comparator.compare(
                simulation_iteration, simulation_iteration.tnt_data.tree_path
            )

            simulation_iteration.maxcut_data.distance = maxcut_tree_comparator.compare(
                simulation_iteration, simulation_iteration.maxcut_data.tree_path
            )

            self._save_results(max_taxa, min_taxa, simulation_iterations, taxa_jump)
            #
            # simulation_iteration.clann_data.distance = clann_tree_comparator.compare(
            #     simulation_iteration, simulation_iteration.clann_data.tree_path
            # )
            #
            # simulation_iteration.spr_data.distance = spr_tree_comparator.compare(
            #     simulation_iteration, simulation_iteration.spr_data.tree_path
            # )

        for simulation_iteration in simulation_iterations:
            print(simulation_iteration)


    def _save_results(self, max_taxa, min_taxa, simulation_iterations, taxa_jump):
        iterations_dump_dir = "data/simulation_iterations_dump"
        iterations_dump_path = "{}/simulation_iteration_{}_{}_{}.json".format(iterations_dump_dir, min_taxa, max_taxa,
                                                                              taxa_jump)
        os.makedirs(os.path.dirname(iterations_dump_path), exist_ok=True)
        with open(iterations_dump_path, 'w') as iterations_fh:
            json.dump(jsonpickle.encode(simulation_iterations), iterations_fh, indent=4)
