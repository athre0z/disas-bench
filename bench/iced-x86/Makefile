make-bench-iced-x86:
	cargo build --release
	cp target/release/iced-bench bench-iced-fmt
	RUSTFLAGS="--cfg DISAS_BENCH_NO_FORMAT" cargo build --release
	cp target/release/iced-bench bench-iced-no-fmt

clean:
	cargo clean
	rm bench-iced-fmt
	rm bench-iced-no-fmt
