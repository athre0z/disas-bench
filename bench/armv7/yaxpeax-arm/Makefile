make-bench-yaxpeax:
	RUSTFLAGS="-C force-frame-pointers=yes" cargo build --release --features formatter
	cp target/release/yaxpeax-bench bench-yaxpeax-fmt
	RUSTFLAGS="-C force-frame-pointers=yes" cargo build --release
	cp target/release/yaxpeax-bench bench-yaxpeax-no-fmt

clean:
	cargo clean
	rm -f bench-yaxpeax-fmt bench-yaxpeax-no-fmt
