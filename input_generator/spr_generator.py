import os

from input_generator.quartet_generator import QuartetGenerator


class SprGenerator:
    def __init__(self, output_tree_prefix="data/spr_trees/spr_tree", spr_binary_path="externals/spr_supertrees/spr_supertree"):
        self._output_tree_prefix = output_tree_prefix
        self._spr_binary_path = spr_binary_path
        self._clan_input_prefix = "{}_input/spr_input".format(output_tree_prefix)
        self._log_dir_prefix = "{}_log/spr_input".format(output_tree_prefix)
        self._tmp_spr_path = 'tmp_spr'
        pass

    def _generate_input_file(self, quartet_path, ntaxa):
        spr_input_path = "{}_n{}.cln".format(self._clan_input_prefix, str(ntaxa))
        os.makedirs(os.path.dirname(spr_input_path), exist_ok=True)
        with open(spr_input_path, 'w') as input_fh:
            quartets = QuartetGenerator.extract_quartets(quartet_path)
            for quartet in quartets:
                input_fh.write('(({},{}),({},{}));\n'.format(quartet[0], quartet[1], quartet[2], quartet[3]))
        return spr_input_path

    def _extract_tree_from_log(self, log_dir_path):
        with open(log_dir_path, 'r') as log_fh:
            tree_line = log_fh.readlines()[-2]
            tree_start_index = tree_line.index("(")
            tree = tree_line[tree_start_index:]
        return tree

    def generate(self, quartet_path, ntaxa):
        spr_input_path = self._generate_input_file(quartet_path, ntaxa)
        output_tree_path = "{}_n{}.tre".format(self._output_tree_prefix, str(ntaxa))
        log_dir_path = "{}_n{}.log".format(self._log_dir_prefix, str(ntaxa))
        os.makedirs(os.path.dirname(log_dir_path), exist_ok=True)
        command = "{} -i 30 < {} > {}".format(self._spr_binary_path, spr_input_path, log_dir_path)
        print("### Running system command: {} ###".format(command))
        os.system(command)
        print("### System command finished###")
        spr_tree = self._extract_tree_from_log(log_dir_path)

        os.makedirs(os.path.dirname(output_tree_path), exist_ok=True)
        with open(output_tree_path, 'w') as tree_fh:
            tree_fh.write(spr_tree)

        return output_tree_path