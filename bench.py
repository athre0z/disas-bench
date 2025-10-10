import subprocess
import matplotlib.pyplot as plt
import numpy as np
import time
import os
import platform
import sys
import re

root_dir = os.path.dirname(os.path.realpath(__file__))

code_filename = os.path.join(root_dir, 'input', 'xul.dll')
file_code_offs = 0x400
file_code_len = 0x2460400
code_loop_count = 20

if len(sys.argv) == 1:
    pass
elif len(sys.argv) == 4 or len(sys.argv) == 5:
    file_code_offs = int(sys.argv[1])
    file_code_len = int(sys.argv[2])
    code_filename = sys.argv[3]
    if len(sys.argv) >= 5:
        code_loop_count = int(sys.argv[4])
    else:
        # Match the old file + loop-count
        code_loop_count = round(0x2460400 * 20 / file_code_len)
else:
    print('Expected no args or:')
    print(f'  {sys.argv[0]} <code-offset> <code-len> <filename> [loop-count]')
    sys.exit(1)

if not os.path.exists(code_filename):
    print(f'File `{code_filename}` does not exist')
    sys.exit(1)
if file_code_offs < 0:
    print(f'Invalid code-offset {file_code_offs}')
    sys.exit(1)
if file_code_len < 0:
    print(f'Invalid code-len {file_code_len}')
    sys.exit(1)
if code_loop_count < 0:
    print(f'Invalid loop-count {code_loop_count}')
    sys.exit(1)


targets = [
    # Decode + format
    ['bench/cs/bench-cs-fmt', 'Capstone decode+fmt'],
    ['bench/zydis/bench-zydis-full-fmt', 'Zydis (full) decode+fmt'],
    ['bench/xed/bench-xed-fmt', 'XED decode+fmt'],
    ['bench/distorm/bench-distorm-fmt', 'diStorm decode+fmt'],
    ['bench/iced-x86/bench-iced-fmt', 'iced decode+fmt'],
    ['bench/bddisasm/bench-bddisasm-fmt', 'bddisasm decode+fmt'],
    ['bench/bddisasm/bench-bddisasm-mini-fmt', 'bddisasm mini decode+fmt'],
    ['bench/yaxpeax/bench-yaxpeax-fmt', 'yaxpeax decode+fmt'],
    ['bench/sleigh/bench-sleigh-fmt', 'sleigh decode+fmt'],

    # Decode only
    ['bench/zydis/bench-zydis-min-no-fmt', 'Zydis (min) decode'],
    ['bench/zydis/bench-zydis-full-no-fmt', 'Zydis (full) decode'],
    ['bench/xed/bench-xed-no-fmt', 'XED decode'],
    ['bench/distorm/bench-distorm-no-fmt', 'diStorm decode'],
    ['bench/iced-x86/bench-iced-no-fmt', 'iced decode'],
    ['bench/bddisasm/bench-bddisasm-no-fmt', 'bddisasm decode'],
    ['bench/bddisasm/bench-bddisasm-mini-no-fmt', 'bddisasm mini decode'],
    ['bench/yaxpeax/bench-yaxpeax-no-fmt', 'yaxpeax decode'],
]
timings = []

if platform.system() == 'Windows':
    for x in targets:
        x[0] = x[0].replace('/', '\\') + '.exe'

assert all(os.path.exists(x[0]) for x in targets)

# Open & read file once before to make sure it's in OS cache.
with open(code_filename, 'rb') as f:
    f.read()

print('[*] Performing benchmarks')

for cur_target in targets:
    print(f'[*] Benchmarking {cur_target[0]} ...')
    pwd = os.getcwd()
    os.chdir(os.path.dirname(cur_target[0]))

    process_args = [
        os.path.join(root_dir, cur_target[0]),
        f'0x{code_loop_count:X}',
        f'0x{file_code_offs:X}',
        f'0x{file_code_len:X}',
        code_filename
    ]
    prev = time.time()
    process = subprocess.run(process_args, stdout=subprocess.PIPE)
    diff = time.time() - prev
    os.chdir(pwd)
    if process.returncode != 0:
        raise ValueError(f'{cur_target[0]} exited with code {process.returncode}')
    output = process.stdout.decode('utf-8')
    m = re.search('Disassembled (\d+) instructions \((\d+) valid, (\d+) bad\), (\S+) ms', output)
    if m is None:
        raise ValueError(f"Couldn't parse output: `{output}`")
    groups = m.groups()
    total_s = float(groups[3]) / 1000.0

    print(output, end='')
    timings.append(total_s)
    print(f'[+] Completed in {total_s:.2f} ({diff:.2f}) seconds')


print('[*] Generating chart')

code_len_mb = file_code_len / 1024 / 1024
mbs = [(code_len_mb * code_loop_count / x) for x in timings]
plt.rcdefaults()
fig = plt.figure(figsize=(10, 5))
ax = fig.add_subplot(1, 1, 1)

libs = [os.path.basename(x[1]) for x in targets]
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
