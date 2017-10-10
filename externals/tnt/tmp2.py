import subprocess
p = subprocess.Popen(['./tnt'], stdout=subprocess.PIPE, stdin=subprocess.PIPE, stderr=subprocess.PIPE)
p.stdin.write(b'proc quartet.tnt;')
p.stdin.write(b'aquickie;')
res = p.communicate()
print(res)
