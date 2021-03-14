use iced_x86::{Decoder, DecoderOptions, Instruction};
#[cfg(feature = "formatter")]
use iced_x86::{SpecializedFormatter, SpecializedFormatterTraitOptions};
use std::fs::File;
use std::io::Read;
use std::io::Seek;
use std::io::SeekFrom;

#[cfg(feature = "formatter")]
struct TraitOptions;
#[cfg(feature = "formatter")]
impl SpecializedFormatterTraitOptions for TraitOptions {
    const ENABLE_DB_DW_DD_DQ: bool = false;
    unsafe fn verify_output_has_enough_bytes_left() -> bool {
        false
    }
}
#[cfg(feature = "formatter")]
type Formatter = SpecializedFormatter<TraitOptions>;

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

    let mut decoder = Decoder::new(64, &xul_code, DecoderOptions::NONE);

    #[cfg(feature = "formatter")]
    let mut text = String::new();
    #[cfg(feature = "formatter")]
    let mut formatter = Formatter::new();

    let mut instruction = Instruction::default();

    let mut num_valid_insns: usize = 0;
    let mut num_bad_insns: usize = 0;
    for _ in 0..20 {
        decoder.try_set_position(0).unwrap();
        decoder.set_ip(0);
        while decoder.can_decode() {
            decoder.decode_out(&mut instruction);

            if !instruction.is_invalid() {
                #[cfg(feature = "formatter")]
                {
                    text.clear();
                    formatter.format(&instruction, &mut text);
                }

                num_valid_insns += 1;
                // position is automatically updated in the happy path
            } else {
                num_bad_insns += 1;
                // manually seek forward one byte to try again
                decoder
                    .try_set_position(decoder.position() - instruction.len() + 1)
                    .unwrap();
            }
        }
    }

    println!("Disassembled {} instructions ({} valid, {} bad)",
        num_valid_insns + num_bad_insns,
        num_valid_insns,
        num_bad_insns,
    );
}
