from input_generator.clann_generator import ClannGenerator
from input_generator.maxcut_generator import MaxcutGenerator
from input_generator.quartet_generator import QuartetGenerator
from input_generator.spr_generator import SprGenerator
from input_generator.tnt_matrix_generator import TntMatrixGenerator
from input_generator.tnt_tree_genertor import TntTreeGenerator
from input_generator.tree_comparator import TreeComparator
from input_generator.tree_generator import TreeGenerator


class SimulationIterationData:
    def __init__(self, ntaxa):
        self.ntaxa = ntaxa
        self.simulated_tree_path = None
        self.simulated_tree_w_path = None
        self.quartets_path = None
        self.clann_path = None
        self.spr_path = None
        self.tnt_matrix_path = None
        self.tnt_tree_path = None
        self.maxcut_tree_path = None
        self.tnt_distance = None
        self.maxcut_distance = None
        self.clann_distance = None
        self.spr_distance = None



class TreeSimulator:

    def run(self):
        simulation_iterations = [SimulationIterationData(ntaxa) for ntaxa in range(10, 18, 4)]
        tree_generator = TreeGenerator()
        quartet_generator = QuartetGenerator()
        tnt_matrix_generator = TntMatrixGenerator()
        tnt_tree_generator = TntTreeGenerator()
        maxcut_generator = MaxcutGenerator()
        clann_generator = ClannGenerator()
        spr_generator = SprGenerator()

        tnt_tree_comparator = TreeComparator("data/tnt_distance/tnt_distance")
        maxcut_tree_comparator = TreeComparator("data/maxcut_distance/maxcut_distance")
        clann_tree_comparator = TreeComparator("data/clann_distance/clann_distance")
        spr_tree_comparator = TreeComparator("data/spr_distance/spr_distance")

        for simulation_iteration in simulation_iterations:
            simulation_iteration.simulated_tree_path, simulation_iteration.simulated_tree_w_path = \
                tree_generator.generate(simulation_iteration.ntaxa)

            simulation_iteration.quartets_path = quartet_generator.generate(
                simulation_iteration.simulated_tree_w_path, simulation_iteration.ntaxa, "on"
            )

            simulation_iteration.clann_path = clann_generator.generate(
                simulation_iteration.quartets_path, simulation_iteration.ntaxa
            )

            simulation_iteration.spr_path = spr_generator.generate(
                simulation_iteration.quartets_path, simulation_iteration.ntaxa
            )

            simulation_iteration.tnt_matrix_path = tnt_matrix_generator.generate(
                simulation_iteration.quartets_path, simulation_iteration.ntaxa
            )

            simulation_iteration.tnt_tree_path = tnt_tree_generator.generate(
                simulation_iteration.tnt_matrix_path, simulation_iteration.ntaxa
            )

            simulation_iteration.tnt_distance = tnt_tree_comparator.compare(
                simulation_iteration.simulated_tree_path, simulation_iteration.tnt_tree_path, simulation_iteration.ntaxa
            )

            simulation_iteration.maxcut_tree_path = maxcut_generator.generate(
                simulation_iteration.quartets_path, simulation_iteration.ntaxa
            )

            simulation_iteration.maxcut_distance = maxcut_tree_comparator.compare(
                simulation_iteration.simulated_tree_path, simulation_iteration.maxcut_tree_path,
                simulation_iteration.ntaxa
            )

            simulation_iteration.clann_distance = clann_tree_comparator.compare(
                simulation_iteration.simulated_tree_path, simulation_iteration.clann_path, simulation_iteration.ntaxa
            )

            simulation_iteration.spr_distance = spr_tree_comparator.compare(
                simulation_iteration.simulated_tree_path, simulation_iteration.spr_path, simulation_iteration.ntaxa
            )

        for simulation_iteration in simulation_iterations:
            print("taxa: {}, tnt distance: {}".format(simulation_iteration.ntaxa, simulation_iteration.tnt_distance))
            print("taxa: {}, maxcut distance: {}".format(
                simulation_iteration.ntaxa, simulation_iteration.maxcut_distance
            ))
            print("taxa: {}, clann distance: {}".format(
                simulation_iteration.ntaxa, simulation_iteration.clann_distance
            ))
            print("taxa: {}, spr distance: {}".format(
                simulation_iteration.ntaxa, simulation_iteration.spr_distance
            ))

