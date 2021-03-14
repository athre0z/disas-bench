import subprocess
import matplotlib.pyplot as plt
import numpy as np
import time
import os

file_code_len = 0x2460400
code_loop_count = 20

targets = [
    'bench/cs/bench-cs-fmt',
    'bench/zydis/bench-zydis-min-no-fmt',
    'bench/zydis/bench-zydis-full-no-fmt',
    'bench/zydis/bench-zydis-full-fmt',
    'bench/xed/bench-xed-fmt',
    'bench/xed/bench-xed-no-fmt',
    'bench/distorm/bench-distorm-fmt',
    'bench/distorm/bench-distorm-no-fmt',
    'bench/iced-x86/bench-iced-fmt',
    'bench/iced-x86/bench-iced-no-fmt',
    'bench/bddisasm/bench-bddisasm-fmt',
    'bench/bddisasm/bench-bddisasm-no-fmt',
    'bench/yaxpeax/bench-yaxpeax-fmt',
    'bench/yaxpeax/bench-yaxpeax-no-fmt',
]
timings = []

assert all(os.path.exists(x) for x in targets)

# Open & read file once before to make sure it's in OS cache.
with open('input/xul.dll', 'rb') as f:
    f.read()

print('[*] Performing benchmarks')

for cur_target in targets:
    print('[*] Benchmarking {} ...'.format(cur_target))    
    pwd = os.getcwd()
    os.chdir(os.path.dirname(cur_target))

    prev = time.time()
    subprocess.run(['./' + os.path.basename(cur_target)])
    diff = time.time() - prev
    timings.append(diff)

    os.chdir(pwd)
    print('[+] Completed in {:.2f} seconds'.format(diff))


print('[*] Generating chart')

code_len_mb = file_code_len / 1024 / 1024
mbs = [(code_len_mb * code_loop_count / x) for x in timings]
plt.rcdefaults()
fig = plt.figure(figsize=(10, 5))
ax = fig.add_subplot(1, 1, 1)

libs = [os.path.basename(x) for x in targets]
y_pos = np.arange(len(libs))
best = mbs.index(max(mbs))

ax.barh(
    y_pos, 
    mbs, 
    align='center',
    color='#9999ff'
)[best].set_color('lightgreen')
ax.set_yticks(y_pos)
ax.set_yticks(y_pos)
ax.set_yticklabels(libs)
ax.invert_yaxis()  # labels read top-to-bottom
ax.set_xlabel('MB/s')
ax.set_title('Throughput')
plt.subplots_adjust(left=0.2, right=0.95, top=0.9, bottom=0.1)
fig.savefig('bench.png')
