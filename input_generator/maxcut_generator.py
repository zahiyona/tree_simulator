import os


class MaxcutGenerator:
    def __init__(self, maxcut_path="externals/find-cut",
                 output_maxcut_prefix="data/maxcut_tree/maxcut_tree"):
        self._maxcut_path = maxcut_path
        self._output_maxcut_prefix = output_maxcut_prefix

    def generate(self, quartet_path,  ntaxa):

        output_maxcut_path = "{}_n{}.tnt".format(self._output_maxcut_prefix, str(ntaxa))
        os.makedirs(os.path.dirname(output_maxcut_path), exist_ok=True)
        log_path = "{}_logs/{}_n{}.log".format(self._output_maxcut_prefix, str(ntaxa))
        os.makedirs(os.path.dirname(log_path), exist_ok=True)
        command = "{} qrtt={} otre={} > {}".format(
            self._maxcut_path, quartet_path, output_maxcut_path, log_path)
        print("### Running system command for maxcut: {} ###".format(command))
        os.system(command)
        print("### System command finished###")
        return output_maxcut_path