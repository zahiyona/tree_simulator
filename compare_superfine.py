import os
from statistics import mean
from externals.qfit_between_trees import compare_quartet_trees
from externals.phylo import compute_RF_between_tree_files
import matplotlib.pyplot as plt

QUARTETS_FACTOR = 1.2
SIMULATED_FOLDER_PATH = 'data/SF/'

def plot_SF_results(maxcut_means, paup_means, scaffolds, y_label, ntaxa):
    plt.title("nTaxa: {}".format(str(ntaxa)))
    plt.xlabel("scaffold")
    plt.ylabel(y_label)
    maxcut_plt, = plt.plot(scaffolds, maxcut_means, linewidth=1, marker='+', ms=10, color='r', linestyle='--')
    paup_plt, = plt.plot(scaffolds, paup_means, linewidth=1, marker='o', ms=10, color='y', linestyle='--')
    result_path = "data/SF_results/result-{}-{}.png".format(y_label, ntaxa)
    plt.legend([maxcut_plt, paup_plt], ['maxcut', 'paup'])
    # os.makedirs(os.path.dirname(result_path), exist_ok=True)
    plt.savefig(result_path)
    plt.clf()


def compare_trees(method_code, ntaxa):
    simulated_folder_path = SIMULATED_FOLDER_PATH + str(ntaxa) + "/"
    rf_means = []
    qfit_means = []
    time_means = []
    scaffolds = os.listdir(simulated_folder_path)
    scaffolds = [float(scaffold) for scaffold in scaffolds]
    scaffolds.sort()
    for scaffold in scaffolds:
        scaffold_path = simulated_folder_path + "/" + str(scaffold)
        rf_similarities = []
        qfit_similarities = []
        times = []
        for simulated_tree in os.listdir(scaffold_path):
            if not simulated_tree.endswith(method_code):
                continue
            result_tree_path = scaffold_path + "/" + simulated_tree
            tree_with_time_path_prefix = result_tree_path[:result_tree_path.rindex(".")]
            tree_path_prefix, time_str = tree_with_time_path_prefix.split("+")
            time = float(time_str)
            times.append(time)
            model_tree_path = tree_path_prefix + ".model_tree"

            rf_distance, _, _ = compute_RF_between_tree_files(result_tree_path, model_tree_path)
            rf_similarity = 1 - (float(rf_distance) / ((ntaxa - 3)*2))
            rf_similarities.append(rf_similarity)

            qrt_cnt, cnt_agree, qfit_similarity = compare_quartet_trees(result_tree_path, model_tree_path, int(ntaxa ** QUARTETS_FACTOR))
            qfit_similarities.append(qfit_similarity)
        rf_means.append(mean(rf_similarities))
        qfit_means.append(mean(qfit_similarities))
        time_means.append(mean(times))
    return qfit_means, rf_means, time_means, scaffolds


def main():
    for ntaxa in os.listdir(SIMULATED_FOLDER_PATH):
        ntaxa = int(ntaxa)
        maxcut_qfit_means, maxcut_rf_means, maxcut_times_mean, scaffolds = compare_trees('SFwQMC', ntaxa=ntaxa)
        paup_qfit_means, paup_rf_means, paup_times_mean, scaffolds = compare_trees('SFpaup', ntaxa=ntaxa)
        plot_SF_results(maxcut_qfit_means, paup_qfit_means, scaffolds, y_label="qfit-similarity", ntaxa=ntaxa)
        plot_SF_results(maxcut_rf_means, paup_rf_means, scaffolds, y_label="rf-similarity", ntaxa=ntaxa)
        plot_SF_results(maxcut_times_mean, paup_times_mean, scaffolds, y_label="time(seconds)", ntaxa=ntaxa)


if __name__ == '__main__':
    main()