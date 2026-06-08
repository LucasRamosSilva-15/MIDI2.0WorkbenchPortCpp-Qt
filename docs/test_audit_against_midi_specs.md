# Test Audit against MIDI 2.0 / UMP Specifications

**Date:** 2026-06-08
**Specification References:** 
- M2-104-UM_v1-1-2 (UMP and MIDI 2.0 Protocol Specification)
- M2-100-U_v1-1 (MIDI 2.0 Specification Overview)
- M2-103-UM_v1-2 (MIDI-CI Property Exchange)

## 1. Audit Summary

| Test Name | File | Input | Expected / Current Behavior | Spec Reference | Status | Technical Note |
|---|---|---|---|---|---|---|
| MIDI 1.0 CV valid/MT/Size | `UmpParserTests.cpp` | `20904000` | MT: 0x2, Size: 32 bits | M2-104: Section 4 | **OK** | Note On valid. |
| MIDI 2.0 CV valid/MT/Size | `UmpParserTests.cpp` | `40904000 40000000` | MT: 0x4, Size: 64 bits | M2-104: Section 4 | **Parcial / Intencional** | Parser covers structure logic, but specific payload extraction (Bank, Options) might still be partial. |
| SysEx7 valid/MT/Form | `UmpParserTests.cpp` | `30040000 00000000` | MT: 0x3, 64 bits, Complete | M2-104: Section 4 | **OK** | Properly handles SysEx7 packet. |
| SysEx8 valid/MT/StreamID | `UmpParserTests.cpp` | `501D...` | MT: 0x5, 128 bits | M2-104: Section 4 | **OK** | Identifies stream ID natively. |
| MDS Header valid | `UmpParserTests.cpp` | `508E...` | MT: 0x5, Mixed Data Set | M2-104: Section 4 | **OK** | Identifies header packet. |
| Flex Data valid | `UmpParserTests.cpp` | `D0000101...` | MT: 0xD, 128 bits | M2-104: Section 4 | **OK** | Flex Data base support implemented. |
| UMP Stream valid | `UmpParserTests.cpp` | `F0000000...` | MT: 0xF, 128 bits | M2-104: Section 4 | **OK** | Endpoint info support implemented. |
| System Real-Time (Offline) | `UmpParserTests.cpp` | `10F80000` | MT 0x1, 32 bits | M2-104: Section 4 | **OK** | Test exists for Timing Clock, Start, and Stop. |
| Decoder Note On Vel 0 | `UmpParserTests.cpp` | `0x92, 0x3E, 0x00` | "Note On (vel 0 / Note Off)" | M2-104: Section 4.1 | **OK** | Decodes semantics but preserves original 0x9n status internally. Correct for MIDI 1.0 parsing without corrupting 0x90->0x80. |
| Converter Program Change | `UmpParserTests.cpp` | `C0 05` | `20C00500` | M2-104: Section 4.1 | **OK** | Fills trailing byte with 0 pad. |
| Converter Pitch Bend | `UmpParserTests.cpp` | `E0 00 40` | `20E00040` | M2-104: Section 4.1 | **OK** | Encodes correctly to 32-bits UMP. |
| Converter System RT | `UmpParserTests.cpp` | `F8` | unsupported | M2-104: Section 4 | **OK** | Converter strictly converts MIDI 1.0 bytes to MT 0x2. F8 demands MT 0x1. |
| MIDI-CI Property Exchange | - | - | - | M2-103 | **Fora de escopo** | No infrastructure built yet. |
| FakeUmpBackend | `UmpParserTests.cpp` | N/A | Generates MT 0x2 simulation | Architecture | **OK** | Clear that it captures no real hardware. |

## 2. Technical Evaluation

### MT 0x2 (MIDI 1.0 Channel Voice)
The offline parser and preview converters show excellent adherence. Crucially:
- `Program Change` and `Channel Pressure` properly manage the zero-padding at the lowest byte of the 32-bit packet, as mandated by the M2-104.
- `Note On velocity 0` is accurately represented by `Midi1LiveDecoder` without aggressive modification of the 0x9n status block.

### MT 0x4 (MIDI 2.0 Channel Voice)
There is a base test asserting parsing length (64 bits) and MT identification. Given the experimental scope, lack of deep 16-bit velocity interpolation / 32-bit CC testing in the parser is acceptable (`Parcial / Intencional`). 
- Em MIDI 1.0 e UMP MT 0x2, Note On velocity 0 pode ser interpretado como equivalente musical a Note Off, mas o status permanece 0x9n.
- Em MIDI 2.0 Channel Voice MT 0x4, Note On com velocity 0 é estritamente um Note On e NÃO deve ser documentado nem convertido como Note Off. A bateria atual respeita essa segmentação ao não forçar conversões ilegais.

### System Common / Real-Time (MT 0x1)
**Resolved:** The project includes unit tests proving the UmpParser handles `10F80000` (Timing Clock), `10FA0000` (Start), and `10FC0000` (Stop) as MT 0x1. Single-byte representations like `F8` remain solely valid in the Live Decoder environment, not in UMP packet offline analysis.

### SysEx / Flex Data / UMP Stream
Base parsing tests exist and validate MT and payload length. Heavy reconstruction algorithms are out of scope. Property Exchange tests (M2-103) are intentionally absent to avoid false claims of support.

### UMP Preview Converter
Perfectly bounded. Tests prove it converts MIDI 1.0 datastream into 32-bit UMP MT 0x2 words. System messages (`F8`) are rightfully rejected by this specific method as they require MT 0x1.

## 3. Required Test Updates
- **To Add:** *None remaining*. MT 0x1 tests were successfully added to the CTest suite.
