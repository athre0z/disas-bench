ifeq ($(shell uname -s),Darwin)
    DISTORM_LIB = ../distorm/make/mac/libdistorm3.dylib
else
    DISTORM_LIB = ../distorm/make/linux/libdistorm3.so
endif

BASE_ARGS = -O3 -I../distorm/include $(DISTORM_LIB) main.c

make-bench-distorm:
	cc -o bench-distorm-fmt $(BASE_ARGS)
	cc -DDISAS_BENCH_NO_FORMAT -o bench-distorm-no-fmt $(BASE_ARGS)

clean:
	rm -f bench-distorm* *.o
