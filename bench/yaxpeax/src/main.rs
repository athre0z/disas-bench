use std::fs::File;
use std::io::Read;
use std::io::Seek;
use std::io::SeekFrom;

use yaxpeax_arch::{Decoder, Reader};
use yaxpeax_x86::long_mode as amd64;
use yaxpeax_x86::long_mode::{Arch, DecodeError};
#[cfg(feature = "formatter")]
use yaxpeax_x86::long_mode::DisplayStyle;

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

    let decoder = <Arch as yaxpeax_arch::Arch>::Decoder::default();

    #[cfg(feature = "formatter")]
    let mut buf = yaxpeax_x86::long_mode::InstructionTextBuffer::new();

    let mut instruction = amd64::Instruction::default();

    let mut num_valid_insns: usize = 0;
    let mut num_bad_insns: usize = 0;
    let time = std::time::SystemTime::now();
    for _ in 0..loop_count {
        let mut offset = 0u64;
        let mut reader = yaxpeax_arch::U8Reader::new(&code[(offset as usize)..]);
        loop {
            match decoder.decode_into(&mut instruction, &mut reader) {
                Ok(()) => {
                    #[cfg(feature = "formatter")]
                    {
                        buf.format_inst(&instruction.display_with(DisplayStyle::Intel))
                            .expect("can format successfully");
                    }

                    num_valid_insns += 1;
                }
                Err(DecodeError::ExhaustedInput) => {
                    break;
                }
                Err(_) => {
                    num_bad_insns += 1;
                    // manually seek forward one byte to try again
                    let update: u64 = Reader::<u64, u8>::total_offset(&mut reader) - Reader::<u64, u8>::offset(&mut reader) + 1u64;
                    offset += update;
                    reader = yaxpeax_arch::U8Reader::new(&code[(offset as usize)..]);
                }
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
