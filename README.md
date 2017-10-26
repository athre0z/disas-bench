Disassembler Benchmark
======================

This repository holds benchmarking code for various x86/x86-64  disassembler libraries.

## Results
![Bench](https://i.imgur.com/gCUzomq.png)
macOS 10.13, Apple Clang 900, i7-6850k

## Candidates

[Capstone](https://github.com/aquynh/capstone)

[DiStorm](https://github.com/gdabah/distorm)

[Intel XED](https://github.com/intelxed/xed)

[Zydis](https://github.com/zyantific/zydis)

## Suffixes

| Suffix    | Explaination |
| --------- | ------------ |
| `-no-fmt` | Decoding only |
| `-fmt`    | Decoding + formatting |
| `-min`/`-full` | Zydis specific: `ZYDIS_DECODE_GRANULARITY_(MINIMAL|FULL)` argument |

Decoding: Parsing the raw instruction bytes into a machine processable structure

Formatting: Translating the structure to human readable assembly

## Benchmarking
```bash
git clone --recursive 'https://github.com/athre0z/disas-bench.git'
./make-all.sh
python3 -mvenv venv
source venv/bin/activate
pip install -r requirements.txt
python bench.py
```

## Contributing
If you feel like the benchmark for a lib doesn't drive it to its full potential or treats it unfairly, I'd be happy to accept PRs with improvements!
