import os
import re

class QuartetGenerator:
    def __init__(self, quartet_generator_path="externals/tree_quartets", output_quartet_prefix="data/quartets/quartet"):
        self._quartet_generator_path = quartet_generator_path
        self._output_quartet_prefix = output_quartet_prefix

    @staticmethod
    def extract_quartets(quartet_path):
        with open(quartet_path,'r') as quartets_fh:
            quartets_text = quartets_fh.read()
            quartets = re.findall('(\d*),(\d*)\|(\d*),(\d*)', quartets_text)
            return quartets

    def generate(self, tree_file_path, ntaxa, weights):
        output_quartet_path = "{}_n{}.{}qrt".format(self._output_quartet_prefix, str(ntaxa),
                                                    "w" if weights == "on" else "")
        os.makedirs(os.path.dirname(output_quartet_path), exist_ok=True)
        command = "{} tree_file={} out_qrt_file={} ntaxa={} weights={}".format(
            self._quartet_generator_path, tree_file_path, output_quartet_path, ntaxa, weights
        )
        print("### Running system command: {} ###".format(command))
        os.system(command)
        print("### System command finished###")
        return output_quartet_path
