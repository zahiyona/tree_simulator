import json
from typing import Iterable, Dict, List
import numpy as np
import matplotlib.pyplot as plt
import os

from externals import jsonpickle
from input_generator.generator_base import SimulationIteration, TreeData


def main():
    iterations_base_dump_path = "../data/simulation_iterations_dump"
    #iterations_base_dump_path = iterations_base_dump_path + "/simulation_iteration_200_501_50.json"
    iterations_dump_path = iterations_base_dump_path + "/simulation_iteration_300_401_100.json"
    iterations_dump_path2 = iterations_base_dump_path + "/simulation_iteration_300_401_100.json"

    taxa_iteration_map = get_taxa_map(iterations_dump_path)
    taxa_iteration_map2 = get_taxa_map(iterations_dump_path2)

    for ntaxa, iterations in taxa_iteration_map.items():

        # qnum_factors = [iteration.qnum_factor for iteration in iterations]
        # distances = [iteration.tnt_distance.qfit_distance for iteration in iterations]
        qnum_factors = []
        maxcut_distances = []
        tnt_distances = []
        clann_distances = []
        paup_distances = []

        maxcut_distances_rf = []
        tnt_distances_rf = []
        clann_distances_rf = []
        paup_distances_rf = []

        maxcut_times = []
        tnt_times = []
        clann_times = []
        paup_times = []

        iterations2 = taxa_iteration_map2[ntaxa]
        for ind, iteration in enumerate(iterations):
            if iteration.qnum_factor == 2.8:
                continue
            iteration2 = iterations2[ind]
            qnum_factors.append(iteration.qnum_factor)
            update_data_arrays(iteration2.tnt_data, iteration.tnt_data, tnt_distances_rf, tnt_distances, tnt_times)
            update_data_arrays(iteration2.maxcut_data, iteration.maxcut_data, maxcut_distances_rf, maxcut_distances, maxcut_times)
            update_data_arrays(iteration2.clann_data, iteration.clann_data, clann_distances_rf, clann_distances, clann_times)
            update_data_arrays(iteration2.paup_data, iteration.paup_data, paup_distances_rf, paup_distances, paup_times)


        plot_results(paup_distances, clann_distances, maxcut_distances, ntaxa, qnum_factors, tnt_distances, 'results/result_qfit_n{}.png')
        plot_results(paup_distances_rf, clann_distances_rf, maxcut_distances_rf, ntaxa, qnum_factors, tnt_distances_rf, 'results/result_rf_n{}.png')
        plot_results(paup_times, clann_times, maxcut_times, ntaxa, qnum_factors, tnt_times, 'results/result_time_n{}.png')


def get_taxa_map(iterations_dump_path):
    taxa_iteration_map = {}  # type: Dict[float, List[SimulationIteration]]
    with open(iterations_dump_path, 'r') as iterations_fh:
        simulation_iterations = jsonpickle.decode(json.load(iterations_fh))  # type: List[SimulationIteration]
    for simulation_iteration in simulation_iterations:
        print(simulation_iteration)
        if simulation_iteration.ntaxa not in taxa_iteration_map.keys():
            taxa_iteration_map[simulation_iteration.ntaxa] = [simulation_iteration]
        else:
            taxa_iteration_map[simulation_iteration.ntaxa].append(simulation_iteration)
    return taxa_iteration_map


def update_data_arrays(tree_data2, tree_data: TreeData, distances_rf, distances, times):
    
    if tree_data.distance is not None and tree_data2.distance is not None:
        distances_rf.append(np.mean([tree_data.distance.rf_distance, tree_data2.distance.rf_distance]))
        distances.append(np.mean([float(tree_data.distance.qfit_distance), float(tree_data2.distance.qfit_distance)]))
        times.append(round(np.mean([float(tree_data.running_time), float(tree_data2.running_time)]),3))
    else:
        distances_rf.append(0)
        distances.append(0)
        times.append(0)
    
    return distances_rf, distances, times

def plot_results(paup, clann, maxcut, ntaxa, qnum_factors, tnt, plot_file_prefix):
    plt.title("nTaxa: " + str(ntaxa))
    plt.xlabel("qrt-num-factor")
    plt.ylabel("time")
    # tnt_plt, = plt.plot(qnum_factors, tnt, linewidth=1, marker='^', ms=10, color='b', linestyle='--')
    maxcut_plt, = plt.plot(qnum_factors, maxcut, linewidth=1, marker='+', ms=10, color='r', linestyle='--')
    # clann_plt, = plt.plot(qnum_factors, clann, linewidth=1, marker='o', ms=10, color='g', linestyle='--')
    paup_plt, = plt.plot(qnum_factors, paup, linewidth=1, marker='o', ms=10, color='y', linestyle='--')
    # plt.legend([tnt_plt, maxcut_plt, clann_plt, paup_plt], ['tnt', 'maxcut', 'clann', 'paup'])
    plt.legend([maxcut_plt, paup_plt], ['maxcut', 'paup'])
    result_path = plot_file_prefix.format(ntaxa)
    os.makedirs(os.path.dirname(result_path), exist_ok=True)
    plt.savefig(result_path)
    plt.clf()


if __name__ == '__main__':
    main()

