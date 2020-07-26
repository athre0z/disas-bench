make-bench-bddisasm:
	cc main.c -O3 -o bench-bddisasm-fmt -I../../libs/bddisasm/inc -L../../libs/bddisasm/bin/x64/Release -lbddisasm
	cc main.c -O3 -o bench-bddisasm-no-fmt -DDISAS_BENCH_NO_FORMAT -I../../libs/bddisasm/inc -L../../libs/bddisasm/bin/x64/Release -lbddisasm

clean:
	rm -f bench-bddisasm-fmt bench-bddisasm-no-fmt
