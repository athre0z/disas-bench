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

#[allow(clippy::manual_strip)]
fn parse_int(s: &str) -> Result<usize, String> {
    if s.starts_with("0x") {
        usize::from_str_radix(&s[2..], 16)
    } else {
        s.parse()
    }
    .map_err(|_| format!("Failed to parse {}", s))
}

fn read_file() -> Result<(Box<[u8]>, usize), String> {
    let args: Vec<_> = std::env::args().skip(1).collect();
    if args.len() != 4 {
        return Err("Expected args: <loop-count> <code-offset> <code-len> <filename>".into());
    }
    let loop_count: usize =
        parse_int(&args[0]).map_err(|e| format!("Couldn't parse loop-count: {}", e))?;
    let file_offset: u64 =
        parse_int(&args[1]).map_err(|e| format!("Couldn't parse code-offset: {}", e))? as u64;
    let bin_len: usize =
        parse_int(&args[2]).map_err(|e| format!("Couldn't parse code-len: {}", e))?;
    let filename: &str = &args[3];

    let mut f = File::open(filename).map_err(|e| format!("Couldn't open {}, {}", filename, e))?;
    f.seek(SeekFrom::Start(file_offset))
        .map_err(|e| format!("Couldn't seek: {}", e))?;
    let mut code = vec![0; bin_len];
    f.read_exact(&mut code)
        .map_err(|e| format!("Couldn't read {} bytes, {}", bin_len, e))?;
    Ok((code.into_boxed_slice(), loop_count))
}

pub fn main() -> Result<(), Box<dyn std::error::Error>> {
    let (code, loop_count) = read_file()?;

    let mut decoder = Decoder::new(64, &code, DecoderOptions::NONE);

    #[cfg(feature = "formatter")]
    let mut text = String::new();
    #[cfg(feature = "formatter")]
    let mut formatter = Formatter::new();

    let mut instruction = Instruction::default();

    let mut num_valid_insns: usize = 0;
    let mut num_bad_insns: usize = 0;
    let time = std::time::SystemTime::now();
    for _ in 0..loop_count {
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
    let elapsed = time.elapsed().unwrap();
    println!(
        "Disassembled {} instructions ({} valid, {} bad), {} ms",
        num_valid_insns + num_bad_insns,
        num_valid_insns,
        num_bad_insns,
        elapsed.as_millis(),
    );
    Ok(())
}
