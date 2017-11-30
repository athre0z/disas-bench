ifeq ($(shell uname -s),Darwin)
    DISTORM_LIB = ../../libs/distorm/make/mac/libdistorm3.dylib
else
    DISTORM_LIB = ../../libs/distorm/make/linux/libdistorm3.so
endif

BASE_ARGS = -O3 -I../../libs/distorm/include $(DISTORM_LIB)

make-bench-distorm:
	cc main.c -o bench-distorm-fmt $(BASE_ARGS)
	cc main.c -DDISAS_BENCH_NO_FORMAT -o bench-distorm-no-fmt $(BASE_ARGS)

clean:
	rm -f bench-distorm* *.o
