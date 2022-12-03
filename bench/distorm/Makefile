DISTORM_LIB = ../../libs/distorm/distorm3.a
BASE_ARGS = -O3 -I../../libs/distorm/include $(DISTORM_LIB)

make-bench-distorm:
	cc -O3 main.c -o bench-distorm-fmt $(BASE_ARGS)
	cc -O3 main.c -DDISAS_BENCH_NO_FORMAT -o bench-distorm-no-fmt $(BASE_ARGS)

clean:
	rm -f bench-distorm* *.o
