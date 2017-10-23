import re


class SimulationIteration:
    def __init__(self, ntaxa, qnum_factor):
        self.ntaxa = ntaxa
        self.qnum_factor = qnum_factor
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
        self.weights = True


class GeneratorBase:
    def generate(self, simulation_iteration: SimulationIteration):
        pass

    def _extract_quartets(self, quartet_path):
        with open(quartet_path,'r') as quartets_fh:
            quartets_text = quartets_fh.read()
            quartets = re.findall('(\d*),(\d*)\|(\d*),(\d*)', quartets_text)
            return quartets