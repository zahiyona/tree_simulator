import re


class TreeDistance:
    def __init__(self, rf_distance, qfit_distance):
        self.rf_distance = rf_distance
        self.qfit_distance = qfit_distance

    def __str__(self):
        return "RF distance:{}\nqfit distance: {}".format(self.rf_distance, self.qfit_distance)


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
        self.tnt_distance = None  # type: TreeDistance
        self.maxcut_distance = None  # type: TreeDistance
        self.clann_distance = None  # type: TreeDistance
        self.spr_distance = None  # type: TreeDistance
        self.weights = True

    def __str__(self):
        return "Iteration results:\ntaxa: {}\nqnum factor: {}\n#tnt distance#\n{}\n#maxcut distance#\n{}\n#clann " \
               "distance#\n{}\n#spr supertree distance#\n{}\n".format(
            self.ntaxa, self.qnum_factor, self.tnt_distance,
            self.maxcut_distance, self.clann_distance, self.spr_distance
        )


class GeneratorBase:
    def generate(self, simulation_iteration: SimulationIteration):
        pass

    def _extract_quartets(self, quartet_path):
        with open(quartet_path, 'r') as quartets_fh:
            quartets_text = quartets_fh.read()
            quartets = re.findall('(\d*),(\d*)\|(\d*),(\d*)', quartets_text)
            return quartets
