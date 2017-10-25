make-bench-xed:
	cc -O3 -o bench-xed-fmt -I../intelxed/include/public/xed -I../intelxed/obj ../intelxed/obj/libxed.a main.c
	cc -O3 -o bench-xed-no-fmt -DDISAS_BENCH_NO_FORMAT -I../intelxed/include/public/xed -I../intelxed/obj ../intelxed/obj/libxed.a main.c

clean:
	rm -f bench-xed* *.o
