import re

import time


class TreeData:
    def __init__(self):
        self.distance = None  # type: TreeDistance
        self.tree_path = None
        self.running_time = None

    def __str__(self):
        return "distance:{}\nrunning time: {}\npath:{}\n".format(self.distance, self.running_time, self.tree_path)


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
        self.tnt_matrix_path = None
        self.tnt_data = TreeData()  # type: TreeData
        self.maxcut_data = TreeData()  # type: TreeData
        self.clann_data = TreeData()  # type: TreeData
        self.spr_data = TreeData()  # type: TreeData
        self.paup_data = TreeData()  # type: TreeData

        self.weights = True

    def __str__(self):
        return "Iteration results:\ntaxa: {}\nqnum factor: {}\n#tnt#\n{}\n#maxcut#\n{}\n#clann" \
               "#\n{}\n#spr supertree#\n{}\n".format(
            self.ntaxa, self.qnum_factor, self.tnt_data,
            self.maxcut_data, self.clann_data, self.spr_data
        )


class GeneratorBase:
    def __init__(self):
        self._start_time = None

    def generate(self, simulation_iteration: SimulationIteration):
        pass

    def set_start_time(self):
        self._start_time = time.time()

    def _extract_quartets(self, quartet_path):
        with open(quartet_path, 'r') as quartets_fh:
            quartets_text = quartets_fh.read()
            quartets = re.findall('(\d*),(\d*)\|(\d*),(\d*)', quartets_text)
            return quartets
