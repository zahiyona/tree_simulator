import subprocess
import shutil
import os

from input_generator.generator_base import GeneratorBase, SimulationIteration


class TntTreeGenerator(GeneratorBase):
    def __init__(self, tnt_binary_path="externals/tnt/tnt",
                 output_tnt_tree_prefix="data/tnt_tree/tnt_tree"):
        self._tnt_binary_path = tnt_binary_path
        self._output_tnt_tree_prefix = output_tnt_tree_prefix
        self._tnt_output_tree_path = "tmp.tre"

    def _move_tnt_logs(self, logs_dir_path):
        os.makedirs(os.path.dirname(logs_dir_path), exist_ok=True)
        shutil.move(self._tnt_output_tree_path, logs_dir_path + "tmp.tre")
        #shutil.move("aquickie.tre", logs_dir_path + "aquickie.tre")
        shutil.move("aquickie.out", logs_dir_path + "aquickie.out")

    def _convert_indices_to_taxas(self, tnt_matrix_path):
        with open(tnt_matrix_path, 'r') as tnt_matrix_fh:
            tnt_matrix_lines = tnt_matrix_fh.readlines()[3:-3]
            # print(tnt_matrix_lines)
            tnt_taxas = [line.split(' ')[0] for line in tnt_matrix_lines]

        with open(self._tnt_output_tree_path, 'r') as tmp_tree_fh:
            tree_line = tmp_tree_fh.readlines()[1]
            tree_line_iterator = iter(tree_line)
            converted_tree = self.parse_and_convert_tree_line(tnt_taxas, tree_line_iterator)
            if converted_tree[-1] == ',':
                converted_tree = converted_tree[:-1]
        return converted_tree

    def parse_and_convert_tree_line(self, tnt_taxas, tree_line_iterator):
        converted_tree = ""
        try:
            next_char = next(tree_line_iterator)  # type: str
            # will finish by stop iter exception
            while True:
                if next_char == '(':
                    converted_tree += next_char
                    next_char = next(tree_line_iterator)
                    continue

                if next_char == ' ':
                    next_char = next(tree_line_iterator)
                    continue

                if next_char == ')':
                    converted_tree += next_char
                    next_char = next(tree_line_iterator)
                    if next_char != ')':
                        converted_tree += ','
                    continue

                if not next_char.isdigit():
                    next_char = next(tree_line_iterator)
                    continue

                new_num = ""
                while next_char.isdigit():
                    new_num += next_char
                    next_char = next(tree_line_iterator)
                taxa = tnt_taxas[int(new_num)]
                converted_tree += taxa
                next_char = next(tree_line_iterator)
                if next_char != ')':
                    converted_tree += ','
                continue
        except StopIteration:
            return converted_tree

    def _exec_tnt(self, tnt_matrix_path):
        p = subprocess.Popen([self._tnt_binary_path], stdout=subprocess.PIPE, stdin=subprocess.PIPE,
                             stderr=subprocess.PIPE)
        process_cmd = 'proc {};'.format(tnt_matrix_path)
        p.stdin.write(bytes(process_cmd, "ascii"))
        p.stdin.write(b'aquickie;')
        res = p.communicate()
        print(res)

    def generate(self, simulation_iteration: SimulationIteration):
        output_tnt_path = "{}_n{}_q{}.tre".format(self._output_tnt_tree_prefix, str(simulation_iteration.ntaxa),
                                                  str(simulation_iteration.qnum_factor))
        print("### Running tnt binary###")

        self._exec_tnt(simulation_iteration.tnt_matrix_path)

        tnt_tree = self._convert_indices_to_taxas(simulation_iteration.tnt_matrix_path)
        output_tree_path = "{}_n{}_q{}.tre".format(self._output_tnt_tree_prefix, str(simulation_iteration.ntaxa),
                                                   str(simulation_iteration.qnum_factor))
        os.makedirs(os.path.dirname(output_tree_path), exist_ok=True)
        with open(output_tree_path, 'w') as tree_fh:
            tree_fh.write(tnt_tree)
        self._move_tnt_logs("{}_n{}_q{}_logs/".format(self._output_tnt_tree_prefix, str(simulation_iteration.ntaxa),
                                                      str(simulation_iteration.qnum_factor)))

        print("### tnt tree creation finished###")
        simulation_iteration.tnt_tree_path = output_tnt_path
