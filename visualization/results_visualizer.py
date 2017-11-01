import json
from typing import Iterable, Dict, List
import numpy as np
import matplotlib.pyplot as plt
import os

from externals import jsonpickle
from input_generator.generator_base import SimulationIteration


def main():
    iterations_dump_path = "../data/simulation_iterations_dump"
    iterations_dump_path = iterations_dump_path + "/simulation_iteration_100_201_20.json"
    # iterations_dump_path = iterations_dump_path + "/simulation_iteration_7_15_4.json"

    taxa_iteration_map = {} #  type: Dict[float, List[SimulationIteration]]

    with open(iterations_dump_path, 'r') as iterations_fh:
        simulation_iterations = jsonpickle.decode(json.load(iterations_fh)) #  type: List[SimulationIteration]

    for simulation_iteration in simulation_iterations:
        print(simulation_iteration)
        if simulation_iteration.ntaxa not in taxa_iteration_map.keys():
            taxa_iteration_map[simulation_iteration.ntaxa] = [simulation_iteration]
        else:
            taxa_iteration_map[simulation_iteration.ntaxa].append(simulation_iteration)

    for ntaxa, iterations in taxa_iteration_map.items():

        # qnum_factors = [iteration.qnum_factor for iteration in iterations]
        # distances = [iteration.tnt_distance.qfit_distance for iteration in iterations]
        qnum_factors = []
        maxcut_distances = []
        tnt_distances = []
        clann_distances = []

        maxcut_distances_rf = []
        tnt_distances_rf = []
        clann_distances_rf = []

        maxcut_times = []
        tnt_times = []
        clann_times = []

        for iteration in iterations:
            if iteration.qnum_factor is not None and iteration.tnt_data.distance is not None \
                    and iteration.maxcut_data.distance is not None and iteration.clann_data.distance is not None:
                qnum_factors.append(iteration.qnum_factor)
                tnt_distances.append(float(iteration.tnt_data.distance.qfit_distance))
                maxcut_distances.append(float(iteration.maxcut_data.distance.qfit_distance))
                clann_distances.append(float(iteration.clann_data.distance.qfit_distance))
                tnt_distances_rf.append(float(iteration.tnt_data.distance.rf_distance))
                maxcut_distances_rf.append(float(iteration.maxcut_data.distance.rf_distance))
                clann_distances_rf.append(float(iteration.clann_data.distance.rf_distance))
                tnt_times.append(round(float(iteration.tnt_data.running_time) / 60, 3))
                maxcut_times.append(round(float(iteration.maxcut_data.running_time) / 60, 3))
                clann_times.append(round(float(iteration.clann_data.running_time) / 60, 3))

        plot_results(clann_distances, maxcut_distances, ntaxa, qnum_factors, tnt_distances, 'results/result_qfit_n{}.png')
        plot_results(clann_distances_rf, maxcut_distances_rf, ntaxa, qnum_factors, tnt_distances_rf, 'results/result_rf_n{}.png')
        plot_results(clann_times, maxcut_times, ntaxa, qnum_factors, tnt_times, 'results/result_time_n{}.png')


def plot_results(clann, maxcut, ntaxa, qnum_factors, tnt, plot_file_prefix):
    plt.title("nTaxa: " + str(ntaxa))
    tnt_plt, = plt.plot(qnum_factors, tnt, linewidth=1, marker='^', ms=10, color='b', linestyle='--')
    maxcut_plt, = plt.plot(qnum_factors, maxcut, linewidth=1, marker='+', ms=10, color='r', linestyle='--')
    clann_plt, = plt.plot(qnum_factors, clann, linewidth=1, marker='o', ms=10, color='g', linestyle='--')
    plt.legend([tnt_plt, maxcut_plt, clann_plt], ['tnt', 'maxcut', 'clann'])
    result_path = plot_file_prefix.format(ntaxa)
    os.makedirs(os.path.dirname(result_path), exist_ok=True)
    plt.savefig(result_path)
    plt.clf()


if __name__ == '__main__':
    main()

