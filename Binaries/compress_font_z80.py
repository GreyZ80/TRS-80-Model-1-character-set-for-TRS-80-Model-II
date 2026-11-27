#!/usr/bin/env python3
"""
Z80 Font Compressor for IDT7134 Dual-Port RAM Character Generator
-----------------------------------------------------------------
Reads a 4,096-byte (4 KB) character ROM binary (256 characters x 16-byte stride),
strips the 6-byte ASCII label per character (keeping the 10 glyph scanlines),
normalizes background polarity (XOR 0x20 -> 0x00), and compresses the resulting
2,560-byte bitmap table into a Z80-friendly 12-bit LZSS binary stream.

Usage:
    python3 compress_font_z80.py input_4k.bin output_lzss.bin [--no-xor] [--inc]
"""

import argparse
import os
import sys


def strip_and_prepare(raw_4k: bytes, xor_space: bool = True) -> bytes:
    """
    Extracts the first 10 scanlines of each 16-byte character record (256 * 10 = 2,560 bytes)
    and optionally XORs each byte with 0x20 so space (0x20) becomes blank (0x00).
    """
    if len(raw_4k) != 4096:
        raise ValueError(f"Expected exactly 4,096 bytes (4 KB), got {len(raw_4k)} bytes.")

    stripped = bytearray(2560)
    mask = 0x20 if xor_space else 0x00
    dst = 0
    for ch in range(256):
        base = ch * 16
        for row in range(10):
            stripped[dst] = raw_4k[base + row] ^ mask
            dst += 1
    return bytes(stripped)


def lzss_compress_z80(data: bytes) -> bytes:
    """
    Compresses byte stream using a Z80-optimized 12-bit sliding-window LZSS format:
      - Control byte: 8 flag bits (LSB first: bit 0 = first item, bit 7 = 8th item)
          * Flag bit = 1: 1 literal byte follows
          * Flag bit = 0: 2-byte back-reference follows:
                Byte 0: offset_low  (bits 0..7 of backward distance, 1..4095)
                Byte 1: (offset_high << 4) | (length - 3)
                        where offset_high is bits 8..11 (0..15)
                        and length is 3..18 bytes (encoded as 0..15)
      - End-of-stream marker: flag bit = 0 followed by 0x00, 0x00 (offset = 0)
    """
    n = len(data)
    pos = 0
    out = bytearray()

    while pos < n:
        flag_byte = 0
        chunk = bytearray()

        for bit in range(8):
            if pos >= n:
                # Emit end-of-stream marker (flag=0, offset=0x000)
                chunk.extend((0x00, 0x00))
                break

            best_len = 0
            best_dist = 0
            max_len = min(18, n - pos)
            search_start = max(0, pos - 4095)

            # Search backward in the 4,095-byte sliding window for the longest match
            if max_len >= 3:
                for cand in range(pos - 1, search_start - 1, -1):
                    if data[cand] == data[pos]:
                        length = 1
                        while length < max_len and data[cand + length] == data[pos + length]:
                            length += 1
                        if length > best_len:
                            best_len = length
                            best_dist = pos - cand
                            if best_len == max_len:
                                break

            if best_len >= 3:
                # Encode 2-byte match reference (flag bit = 0)
                b0 = best_dist & 0xFF
                b1 = ((best_dist >> 4) & 0xF0) | ((best_len - 3) & 0x0F)
                chunk.extend((b0, b1))
                pos += best_len
            else:
                # Encode literal byte (flag bit = 1)
                flag_byte |= (1 << bit)
                chunk.append(data[pos])
                pos += 1
        else:
            # If the loop finished all 8 bits right at the end of input, emit EOS in next block
            pass

        out.append(flag_byte)
        out.extend(chunk)

        # If we hit exact end of input on bit 7, append a dedicated EOS block
        if pos >= n and len(chunk) == 8 and (flag_byte == 0xFF or chunk[-2:] != bytearray((0x00, 0x00))):
            # Check if EOS was already emitted inside the loop
            pass

    # Verify whether EOS marker (flag=0, 0x00, 0x00) was already emitted; if not, append it
    # Re-run cleanly with explicit EOS state tracking to guarantee exact termination
    return _lzss_compress_exact(data)


def _lzss_compress_exact(data: bytes) -> bytes:
    n = len(data)
    pos = 0
    out = bytearray()
    eos_written = False

    while not eos_written:
        flag_byte = 0
        chunk = bytearray()

        for bit in range(8):
            if pos >= n:
                chunk.extend((0x00, 0x00))
                eos_written = True
                break

            best_len = 0
            best_dist = 0
            max_len = min(18, n - pos)
            search_start = max(0, pos - 4095)

            if max_len >= 3:
                for cand in range(pos - 1, search_start - 1, -1):
                    if data[cand] == data[pos]:
                        length = 1
                        while length < max_len and data[cand + length] == data[pos + length]:
                            length += 1
                        if length > best_len:
                            best_len = length
                            best_dist = pos - cand
                            if best_len == max_len:
                                break

            if best_len >= 3:
                b0 = best_dist & 0xFF
                b1 = ((best_dist >> 4) & 0xF0) | ((best_len - 3) & 0x0F)
                chunk.extend((b0, b1))
                pos += best_len
            else:
                flag_byte |= (1 << bit)
                chunk.append(data[pos])
                pos += 1

        out.append(flag_byte)
        out.extend(chunk)

    return bytes(out)


def lzss_decompress_verify(comp: bytes) -> bytes:
    """Reference Python decompressor matching the Z80 LZSS_UNPACK routine."""
    src = 0
    dst = bytearray()

    while src < len(comp):
        flags = comp[src]
        src += 1
        for bit in range(8):
            if flags & (1 << bit):
                dst.append(comp[src])
                src += 1
            else:
                b0 = comp[src]
                b1 = comp[src + 1]
                src += 2
                dist = b0 | ((b1 & 0xF0) << 4)
                if dist == 0:
                    return bytes(dst)
                length = (b1 & 0x0F) + 3
                start = len(dst) - dist
                for i in range(length):
                    dst.append(dst[start + i])

    raise ValueError("Corrupt stream: missing end-of-stream marker.")


def write_asm_include(comp: bytes, inc_path: str) -> None:
    """Writes a Z80-compatible .DB assembly file alongside the binary."""
    lines = [
        "; Auto-generated Z80 LZSS Compressed Font Data (2,560 bytes unpacked)",
        f"; Compressed size: {len(comp)} bytes",
        f"FONT_DATA_SIZE EQU {len(comp)}",
        "FONT_DATA:"
    ]
    for i in range(0, len(comp), 16):
        row = ", ".join(f"${b:02X}" for b in comp[i:i + 16])
        lines.append(f"        DB      {row}")
    with open(inc_path, "w", encoding="utf-8") as f:
        f.write("\n".join(lines) + "\n")


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Compress a 4 KB character ROM .bin into a Z80 LZSS binary."
    )
    parser.add_argument("input_bin", help="Path to input 4,096-byte (.bin) font file")
    parser.add_argument("output_bin", help="Path to output compressed (.bin) file")
    parser.add_argument(
        "--no-xor",
        action="store_true",
        help="Keep original 0x20 background bytes instead of XORing with 0x20 to 0x00"
    )
    parser.add_argument(
        "--inc",
        metavar="FILE",
        help="Also export a Z80 assembly .DB include file"
    )
    args = parser.parse_args()

    if not os.path.isfile(args.input_bin):
        print(f"Error: Input file not found: {args.input_bin}", file=sys.stderr)
        sys.exit(1)

    with open(args.input_bin, "rb") as f:
        raw_4k = f.read()

    stripped = strip_and_prepare(raw_4k, xor_space=not args.no_xor)
    compressed = _lzss_compress_exact(stripped)

    # Lossless round-trip verification
    restored = lzss_decompress_verify(compressed)
    assert restored == stripped, "Verification failed: decompressed output mismatch!"

    with open(args.output_bin, "wb") as f:
        f.write(compressed)

    if args.inc:
        write_asm_include(compressed, args.inc)

    print(f"Input file:       {args.input_bin} ({len(raw_4k)} bytes)")
    print(f"Stripped bitmaps: {len(stripped)} bytes (256 chars x 10 scanlines)")
    print(f"Compressed size:  {len(compressed)} bytes ({len(raw_4k) / len(compressed):.2f}x vs 4K, {len(stripped) / len(compressed):.2f}x vs 2.5K)")
    print(f"Saved binary to:  {args.output_bin}")
    if args.inc:
        print(f"Saved Z80 .DB to: {args.inc}")


if __name__ == "__main__":
    main()
