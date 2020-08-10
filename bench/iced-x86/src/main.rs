use std::fs::File;
use std::io::Read;
use std::io::Seek;
use std::io::SeekFrom;

use iced_x86::{Decoder, DecoderError, DecoderOptions, Instruction};

#[cfg(not(DISAS_BENCH_NO_FORMAT))]
use iced_x86::{Formatter, NasmFormatter};

// translated from `load_bin.inc`

const XUL_TEXT_OFFS: u64 = 0x400;
const XUL_TEXT_LEN: usize = 0x2460400;

fn read_xul_dll() -> std::io::Result<Box<[u8]>> {
    let mut f = File::open("../../input/xul.dll")?;
    f.seek(SeekFrom::Start(XUL_TEXT_OFFS))?;
    let mut code = vec![0; XUL_TEXT_LEN];
    f.read_exact(&mut code)?;
    Ok(code.into_boxed_slice())
}

pub fn main() {
    let xul_code = match read_xul_dll() {
        Ok(code) => code,
        Err(_) => {
            eprintln!("Can't read xul.dll");
            return;
        }
    };

    let mut decoder = Decoder::new(
        64, // 64-bit
        &xul_code,
        DecoderOptions::NONE
    );

    #[cfg(not(DISAS_BENCH_NO_FORMAT))]
    let mut text = String::new();
    #[cfg(not(DISAS_BENCH_NO_FORMAT))]
    let mut formatter = NasmFormatter::new();

    let mut instruction = Instruction::default();

    let mut num_valid_insns: usize = 0;
    let mut num_bad_insns: usize = 0;
    for _round in 0..20 {
        decoder.set_position(0);
        while decoder.can_decode() {
            let offset = decoder.position();
            decoder.decode_out(&mut instruction);

            if decoder.last_error() == DecoderError::None {
                #[cfg(not(DISAS_BENCH_NO_FORMAT))]
                {
                    text.clear();
                    formatter.format(&instruction, &mut text);
                }

                num_valid_insns += 1;
                // position is automatically updated in the happy path
            } else {
                num_bad_insns += 1;
                // manually seek forward one byte to try again
                decoder.set_position(offset + 1);
            }
        }
    }

    println!("Disassembled {} instructions ({} valid, {} bad)",
        num_valid_insns + num_bad_insns,
        num_valid_insns,
        num_bad_insns,
    );
}
