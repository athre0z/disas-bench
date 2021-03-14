use std::fs::File;
use std::io::Read;
use std::io::Seek;
use std::io::SeekFrom;

use yaxpeax_arch::{Decoder, LengthedInstruction};
use yaxpeax_x86::long_mode as amd64;
use yaxpeax_x86::long_mode::Arch;

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

    let decoder = <Arch as yaxpeax_arch::Arch>::Decoder::default();

    #[cfg(feature = "formatter")]
    let mut text = String::new();

    let mut instruction = amd64::Instruction::default();

    let mut num_valid_insns: usize = 0;
    let mut num_bad_insns: usize = 0;
    for _ in 0..20 {
        let mut offset = 0u64;
        while offset < XUL_TEXT_LEN as u64 {
            match decoder.decode_into(
                &mut instruction,
                xul_code[(offset as usize)..].iter().cloned(),
            ) {
                Ok(()) => {
                    #[cfg(feature = "formatter")]
                    {
                        text.clear();
                        instruction
                            .write_to(&mut text)
                            .expect("can format successfully");
                    }

                    num_valid_insns += 1;
                    offset += instruction.len();
                }
                Err(_) => {
                    num_bad_insns += 1;
                    // manually seek forward one byte to try again
                    offset += 1;
                }
            }
        }
    }

    println!(
        "Disassembled {} instructions ({} valid, {} bad)",
        num_valid_insns + num_bad_insns,
        num_valid_insns,
        num_bad_insns,
    );
}
