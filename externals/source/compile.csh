#/usr/bin/tcsh -xvf
set debug = $1
set debug_flag = ""

if ($debug == "debug") then
set debug_flag = " -DDEBUG_PRT"
endif

set command = "g++ max-cut-tree.c find-cut.c tree-operations.c QMC-general.c newCut.c quartets.c -o max-cut-tree -lm $debug_flag"
echo $command
$command

