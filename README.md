Disassembler Benchmark
======================

This repository holds benchmarking code for various x86/x86-64 disassembler libraries.

## Results
![linux bench on i7-3960x](bench.png)
linux 4.15.0, GCC 7.5, Rust 1.45, i7-3960x

![Bench](https://i.imgur.com/PumBJjJ.png)
macOS 10.13, GCC 7, i7-6850k

![Bench](https://i.imgur.com/gCUzomq.png)
macOS 10.13, Apple Clang 900, i7-6850k

## Candidates

[Capstone](https://github.com/aquynh/capstone)

[DiStorm](https://github.com/gdabah/distorm)

[Intel XED](https://github.com/intelxed/xed)

[Zydis](https://github.com/zyantific/zydis)

[iced](https://github.com/icedland/iced)

[bddisasm](https://github.com/bitdefender/bddisasm)

[yaxpeax-x86](https://github.com/iximeow/yaxpeax-x86)

## Suffixes

| Suffix    | Explaination |
| --------- | ------------ |
| `-no-fmt` | Decoding only |
| `-fmt`    | Decoding + formatting |
| `-min`/`-full` | Zydis specific: `ZYDIS_DECODE_GRANULARITY_(MINIMAL/FULL)` argument |

Decoding: Parsing the raw instruction bytes into a machine processable structure

Formatting: Translating the structure to human readable assembly

## Benchmarking

Windows:

```cmd
REM Start "x64 Native Tools Command Prompt for VS 2019"
REM Start git bash:
"C:\Program Files\Git\bin\bash.exe"
```

Windows/Linux/macOS:

```bash
git clone --recursive 'https://github.com/athre0z/disas-bench.git'
cd disas-bench.git
./make-all.sh
# Windows: python
python3 -mvenv venv
# Windows: source venv/Scripts/activate
source venv/bin/activate
pip install -r requirements.txt
# Optional args: <code-offset> <code-len> <filename> [loop-count]
# Linux: LD_LIBRARY_PATH is needed
LD_LIBRARY_PATH=$(pwd)/bench/distorm python bench.py
```

The optional `bench.py` arguments are:

- `<code-offset>` = offset of the code section (in decimal or 0x hex)
- `<code-len>` = length of the code section (in decimal or 0x hex)
- `<filename>` = binary file to decode and format
- `[loop-count]` = optional loop count. Total number of bytes decoded and formatted is `<code-len> * [loop-count]`

## Contributing
If you feel like the benchmark for a lib doesn't drive it to its full potential or treats it unfairly, I'd be happy to accept PRs with improvements!
