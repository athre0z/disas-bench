make-bench-yaxpeax:
	cargo build --release
	cp target/release/yaxpeax-bench bench-yaxpeax-fmt
	RUSTFLAGS="--cfg DISAS_BENCH_NO_FORMAT" cargo build --release
	cp target/release/yaxpeax-bench bench-yaxpeax-no-fmt

clean:
	cargo clean
	rm -f bench-yaxpeax-fmt bench-yaxpeax-no-fmt
