BASE_ARGS = -O3 -I../../libs/distorm/include ../../libs/distorm/distorm3.a 

make-bench-distorm:
	cc main.c -o bench-distorm-fmt $(BASE_ARGS)
	cc main.c -DDISAS_BENCH_NO_FORMAT -o bench-distorm-no-fmt $(BASE_ARGS)

clean:
	rm -f bench-distorm* *.o
