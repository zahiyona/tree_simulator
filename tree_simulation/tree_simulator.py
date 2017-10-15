from input_generator.quartet_generator import QuartetGenerator
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
        self.tnt_matrix_path = None
        self.tnt_tree_path = None
        self.distance = None


class TreeSimulator:

    def run(self):
        simulation_iterations = [SimulationIterationData(ntaxa) for ntaxa in range(70, 100, 10)]
        tree_generator = TreeGenerator()
        quartet_generator = QuartetGenerator()
        tnt_matrix_generator = TntMatrixGenerator()
        tnt_tree_generator = TntTreeGenerator()
        tree_comparator = TreeComparator()

        for simulation_iteration in simulation_iterations:
            simulation_iteration.simulated_tree_path, simulation_iteration.simulated_tree_w_path = \
                tree_generator.generate(simulation_iteration.ntaxa)

            simulation_iteration.quartets_path = quartet_generator.generate(
                simulation_iteration.simulated_tree_w_path, simulation_iteration.ntaxa, "on"
            )

            simulation_iteration.tnt_matrix_path = tnt_matrix_generator.generate(
                simulation_iteration.quartets_path, simulation_iteration.ntaxa
            )

            simulation_iteration.tnt_tree_path = tnt_tree_generator.generate(
                simulation_iteration.tnt_matrix_path, simulation_iteration.ntaxa
            )

            simulation_iteration.distance = tree_comparator.compare(
                simulation_iteration.simulated_tree_path, simulation_iteration.tnt_tree_path, simulation_iteration.ntaxa
            )

        for simulation_iteration in simulation_iterations:
            print("taxa: {}, distance: {}".format(simulation_iteration.ntaxa, simulation_iteration.distance))

