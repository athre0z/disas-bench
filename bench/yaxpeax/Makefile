make-bench-yaxpeax:
	cargo build --release --features formatter
	cp target/release/yaxpeax-bench bench-yaxpeax-fmt
	cargo build --release --features decoder
	cp target/release/yaxpeax-bench bench-yaxpeax-no-fmt

clean:
	cargo clean
	rm -f bench-yaxpeax-fmt bench-yaxpeax-no-fmt
