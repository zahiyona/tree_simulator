import os
import re


class ClannGenerator:
    def __init__(self, output_tree_prefix="data/clann_trees/clann_tree", clann_binary_path="externals/clann/clann"):
        self._output_tree_prefix = output_tree_prefix
        self._clann_binary_path = clann_binary_path
        self._clan_input_prefix = "{}_input/clann_input".format(output_tree_prefix)
        self._tmp_clann_path = 'tmp_clann'
        pass

    def _extract_quartets(self, quartet_path):
        with open(quartet_path,'r') as quartets_fh:
            quartets_text = quartets_fh.read()
            quartets = re.findall('(\d),(\d)\|(\d),(\d)', quartets_text)
            return quartets


    def _generate_input_file(self, quartet_path, ntaxa):
        clann_input_path = "{}_n{}.cln".format(self._clan_input_prefix, str(ntaxa))
        os.makedirs(os.path.dirname(clann_input_path), exist_ok=True)
        with open(clann_input_path, 'w') as input_fh:
            input_fh.write('#NEXUS\n')
            input_fh.write('Begin trees; [Test Tree file in nexus format]\n')
            quartets = self._extract_quartets(quartet_path)
            for quartet_index, quartet in enumerate(quartets):
                input_fh.write('tree t{} = [&U] (({},{}),({},{}));\n'.format(quartet_index, quartet[0], quartet[1], quartet[2], quartet[3]))
            input_fh.write('End;\n\n')
            input_fh.write('Begin Clann;\n')
            input_fh.write('set criterion=dfit;\n')
            input_fh.write('hs nreps=3 sample=5 swap=spr maxswaps=1000 savetrees='+self._tmp_clann_path+';\n')
            input_fh.write('quit;\n')
            input_fh.write('endblock;')
        return clann_input_path

    def _run_clann(self):
        pass

    def generate(self, quartet_path, ntaxa):
        clann_input_path = self._generate_input_file(quartet_path, ntaxa)
        output_tree_path = "{}_n{}.tre".format(self._output_tree_prefix, str(ntaxa))

        command = "{} {}".format(self._clann_binary_path, clann_input_path)
        print("### Running system command: {} ###".format(command))
        os.system(command)
        print("### System command finished###")

        with open(self._tmp_clann_path,'r') as tmp_clann_fh:
            tmp_tree = tmp_clann_fh.readlines()[0]
            clann_tree = tmp_tree.split(';')[0]
        os.remove(self._tmp_clann_path)
        os.remove('supertree.ps')
        os.makedirs(os.path.dirname(output_tree_path), exist_ok=True)
        with open(output_tree_path, 'w') as tree_fh:
            tree_fh.write(clann_tree)

        return output_tree_path