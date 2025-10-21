CC := cc
CFLAGS := -O3 -I../../libs/bddisasm/inc
LDFLAGS := -L../../libs/bddisasm/build -lbddisasm

make-bench-bddisasm:
	$(CC) main.c $(CFLAGS) $(LDFLAGS) \
	    -o bench-bddisasm-fmt
	$(CC) main.c $(CFLAGS) $(LDFLAGS) \
	    -DDISAS_BENCH_NO_FORMAT \
	    -o bench-bddisasm-no-fmt
	$(CC) main.c $(CFLAGS) $(LDFLAGS) \
	    -DDISASM_BENCH_USE_BDD_MINI \
	    -o bench-bddisasm-mini-fmt
	$(CC) main.c $(CFLAGS) $(LDFLAGS) \
	    -DDISASM_BENCH_USE_BDD_MINI -DDISAS_BENCH_NO_FORMAT \
	    -o bench-bddisasm-mini-no-fmt

clean:
	rm -f bench-bddisasm-fmt bench-bddisasm-no-fmt bench-bddisasm-mini-fmt bench-bddisasm-mini-no-fmt
