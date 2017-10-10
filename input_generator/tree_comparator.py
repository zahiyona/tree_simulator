import os

from externals.phylo import compute_RF_between_tree_files


class TreeComparator:
    def __init__(self, output_distance_prefix="data/distance/distance"):
        self.output_distance_prefix = output_distance_prefix

    def compare(self, first_tree_path, second_tree_path, ntaxa):
        output_distance_path = "{}_n{}.txt".format(self.output_distance_prefix, str(ntaxa))
        distance, _, _ = compute_RF_between_tree_files(first_tree_path, second_tree_path)
        os.makedirs(os.path.dirname(output_distance_path), exist_ok=True)
        with open(output_distance_path, 'w') as tree_fh:
            tree_fh.write(str(distance))

        print("distance: {} saved in: {}".format(distance, output_distance_path))
        return distance
