import subprocess
import matplotlib.pyplot as plt
import numpy as np
import time
import os
import platform
import sys
import re

root_dir = os.path.dirname(os.path.realpath(__file__))

x86_bench_defaults = {
        "arch": "x86",
        "filename": os.path.join(root_dir, 'input', 'xul.dll'),
        "file_code_offs": 0x400,
        "file_code_len": 0x2460400,
        "code_loop_count": 20,
}

armv7_bench_defaults = {
        "arch": "armv7",
        "filename": os.path.join(root_dir, 'input', 'tdfx_dri.so'),
        "file_code_offs": 0x17ba8,
        "file_code_len": 0x1bba4c,
        "code_loop_count": 2000,
}

def parse_args():
    # Argument parsing here is super hacky.. Want to accept command lines like:
    # # oldest
    # ./bench.py 0x400 0x2460400 input/xul.dll
    # # slightly newer: accepts loop count
    # ./bench.py 0x400 0x2460400 input/xul.dll 20
    # # input/xul.dll is in the repo, so we have defaults for using it
    # ./bench.py
    # # we have ARM defaults and a few ARM bench targets
    # ./bench.py -a armv7
    # # and even then, we can provide parameters explicitly
    # ./bench.py -a armv7 0x17ba8 0x1bba4c input/rdfx_dri.so 2001
    # # finally, explicitly picking the old behavior should work
    # ./bench.py -a x86 0x400 0x2460400 ./input/xul.dll 20
    #
    # so: the plan is to pick out a `-a` if we see it, and from there if there are
    # zero arguments, we're done. if there are four arguments, it's the old style
    # and pick an implicit loop count. and if there are five arguments, everything
    # is specified and `-a` if present just selects bench targets.

    if len(sys.argv) == 1:
        # no argument, return None and hope usage helps.
        return None

    bench_settings = None

    if len(sys.argv) > 2 and sys.argv[1] == "-a":
        # ./bench.py -a foo
        # consume both arguments, continue onward
        arch = sys.argv[2]
        del sys.argv[2]
        del sys.argv[1]

        if arch == "x86":
            bench_settings = x86_bench_defaults
        elif arch == "armv7":
            bench_settings = armv7_bench_defaults
        else:
            print(f"Unknown architecture: {arch}")
            return None


    if len(sys.argv) == 4 or len(sys.argv) == 5:
        bench_settings["file_code_offs"] = int(sys.argv[1])
        bench_settings["file_code_len"] = int(sys.argv[2])
        bench_settings["filename"] = sys.argv[3]
        if len(sys.argv) >= 5:
            bench_settings["code_loop_count"] = int(sys.argv[4])
        else:
            # default already set in bench_settings.
            pass

    return bench_settings

bench_settings = parse_args()

if not bench_settings:
    print(f'  {sys.argv[0]} [-a {{x86,armv7}}] [<code-offset> <code-len> <filename> [loop-count]]')
    print(f'')
    print(f'  For example: {sys.argv[0]} -a x86 0x1000 0x4000 your-benchmark.dll 65536')
    sys.exit(1)

if not os.path.exists(bench_settings["filename"]):
    print(f'File `{bench_settings["filename"]}` does not exist')
    sys.exit(1)
if bench_settings["file_code_offs"] < 0:
    print(f'Invalid code-offset {bench_settings["file_code_offs"]}')
    sys.exit(1)
if bench_settings["file_code_len"] < 0:
    print(f'Invalid code-len {bench_settings["file_code_len"]}')
    sys.exit(1)
if bench_settings["code_loop_count"] < 0:
    print(f'Invalid loop-count {bench_settings["code_loop_count"]}')
    sys.exit(1)


targets_x86 = [
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
targets_armv7 = [
    # Decode + format
    ['bench/yaxpeax-arm/bench-yaxpeax-fmt', 'yaxpeax-arm decode+fmt'],

    # Decode only
    ['bench/yaxpeax-arm/bench-yaxpeax-no-fmt', 'yaxpeax-arm decode'],
]

timings = []

targets = {
        "x86": targets_x86,
        "armv7": targets_armv7
}[bench_settings["arch"]]

if platform.system() == 'Windows':
    for x in targets:
        x[0] = x[0].replace('/', '\\') + '.exe'

assert all(os.path.exists(x[0]) for x in targets)

# Open & read file once before to make sure it's in OS cache.
with open(bench_settings["filename"], 'rb') as f:
    f.read()

print('[*] Performing benchmarks')

for cur_target in targets:
    print(f'[*] Benchmarking {cur_target[0]} ...')
    pwd = os.getcwd()
    os.chdir(os.path.dirname(cur_target[0]))

    process_args = [
        os.path.join(root_dir, cur_target[0]),
        f'0x{bench_settings["code_loop_count"]:X}',
        f'0x{bench_settings["file_code_offs"]:X}',
        f'0x{bench_settings["file_code_len"]:X}',
        bench_settings["filename"]
    ]
    prev = time.time()
    process = subprocess.run(process_args, stdout=subprocess.PIPE)
    diff = time.time() - prev
    os.chdir(pwd)
    if process.returncode != 0:
        raise ValueError(f'{cur_target[0]} exited with code {process.returncode}')
    output = process.stdout.decode('utf-8')
    m = re.search(r'Disassembled (\d+) instructions \((\d+) valid, (\d+) bad\), (\S+) ms', output)
    if m is None:
        raise ValueError(f"Couldn't parse output: `{output}`")
    groups = m.groups()
    total_s = float(groups[3]) / 1000.0

    print(output, end='')
    timings.append(total_s)
    print(f'[+] Completed in {total_s:.2f} ({diff:.2f}) seconds')


print('[*] Generating chart')

code_len_mb = bench_settings["file_code_len"] / 1024 / 1024
mbs = [(code_len_mb * bench_settings["code_loop_count"] / x) for x in timings]
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
