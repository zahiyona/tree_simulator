import os


class TntMatrixGenerator:
    def __init__(self, quartet_to_matrix_converter_path="externals/tnt-mat-from-wquartets.pl",
                 output_tnt_prefix="data/tnt_matrix/tnt_matrix"):
        self._quartet_to_matrix_converter_path = quartet_to_matrix_converter_path
        self._output_tnt_prefix = output_tnt_prefix

    def generate(self, quartet_path,  ntaxa):

        output_tnt_matrix_path = "{}_n{}.tnt".format(self._output_tnt_prefix, str(ntaxa))
        # todo: after converting tnt generator perl script to python, use this var to save
        #os.makedirs(os.path.dirname(output_tnt_matrix_path), exist_ok=True)
        command = "{} {}".format(
            self._quartet_to_matrix_converter_path, quartet_path)
        print("### Running system command: {} ###".format(command))
        os.system(command)
        print("### System command finished###")
        return output_tnt_matrix_path