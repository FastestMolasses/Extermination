# Disc inventory and identity

The local audit on 2026-10-07 compared the supplied original SCUS-97112 disc
with the main checkout’s `Extermination-rebuilt.iso`. Both are 2,060,386,304
bytes (1,006,048 sectors). They **do not have the same image bytes**.

| Image | SHA-256 |
|---|---|
| User original | SHA-256 `b6fdb617d2438d3bf1163553e46760365f056e5428030550943142f9c5cc6efe` |
| Existing rebuilt image | SHA-256 `15dd34d0105a8bc7a0605ec1afe844dfdfff2bb022bf1042614081cf7b9b0d33` |

Only `SCUS_971.12` differs: 1,246,308 file bytes differ. Every other file,
directory record, gap, and padding byte is identical. The original ELF SHA-256
matches the pinned `ee052236783e7d3e865754d3ff9fee71290addeb7d146c86caa7ff2724d1e17a`;
the embedded rebuilt ELF has SHA-256
`5a54336d470f111b72d8cb725187edb72ed70d9872911f8cf8c278584a273c87`.

The primary loadable bytes agree exactly for the original 1,530,624-byte
load span after aligning their ELF file offsets: original +768, rebuilt +128.
The ELF headers, program-header layout and file packaging differ. This is why
loadable-code equality did not imply disc-image equality. The new lossless
pipeline uses the user’s original image as its round-trip reference.

Local receipts: `build/repack/identity.json`, `identity_comparison.json`,
`inventory.json`, and `inventory_rebuilt.json`. The inventory JSON includes
every file’s SHA-256, byte offset, ISO name/version and directory-record offset.
These generated receipts are not committed.

## Every file

The table follows physical disc order. LBA × 2048 gives the absolute byte
offset. “Pad” is the zero padding from the declared byte length to the next
sector boundary. There are **43 files**: 20 source-build outputs, 2 archive
components, 12 streams and 9 opaque files. “Source” names the existing decomp
build route; the lossless unpacker preserves the supplied file bytes.

| File | LBA | Bytes | Pad | Class |
|---|---:|---:|---:|---|
| `EXTER.BIN` | 324 | 1073741824 | 0 | Opaque |
| `IRX/IOPRP20.IMG` | 524612 | 109809 | 783 | Opaque |
| `IRX/LIBSD.IRX` | 524666 | 24677 | 1947 | Opaque |
| `IRX/MCMAN.IRX` | 524679 | 82117 | 1851 | Opaque |
| `IRX/MCSERV.IRX` | 524720 | 6649 | 1543 | Opaque |
| `IRX/PADMAN.IRX` | 524724 | 44013 | 1043 | Opaque |
| `IRX/SIO2MAN.IRX` | 524746 | 6161 | 2031 | Opaque |
| `IRX/SNDN2DRV.IRX` | 524750 | 18853 | 1627 | Opaque |
| `DATA/DATA.DAT` | 524760 | 246398976 | 0 | Archive |
| `DATA/INDEX.IDX` | 645072 | 114688 | 0 | Archive |
| `OVERLAY/AREA00.BIN` | 645128 | 40832 | 128 | Source |
| `OVERLAY/AREA01.BIN` | 645148 | 38912 | 0 | Source |
| `OVERLAY/AREA02.BIN` | 645167 | 23936 | 640 | Source |
| `OVERLAY/AREA03.BIN` | 645179 | 23552 | 1024 | Source |
| `OVERLAY/AREA04.BIN` | 645191 | 37888 | 1024 | Source |
| `OVERLAY/AREA06.BIN` | 645210 | 23040 | 1536 | Source |
| `OVERLAY/AREA07.BIN` | 645222 | 19328 | 1152 | Source |
| `OVERLAY/AREA08.BIN` | 645232 | 18944 | 1536 | Source |
| `OVERLAY/AREA11.BIN` | 645242 | 30720 | 0 | Source |
| `OVERLAY/AREA13.BIN` | 645257 | 44416 | 640 | Source |
| `OVERLAY/AREA14.BIN` | 645279 | 20352 | 128 | Source |
| `OVERLAY/AREA15.BIN` | 645289 | 27136 | 1536 | Source |
| `OVERLAY/AREA16.BIN` | 645303 | 36864 | 0 | Source |
| `OVERLAY/AREA17.BIN` | 645321 | 22912 | 1664 | Source |
| `OVERLAY/AREA18.BIN` | 645333 | 2176 | 1920 | Source |
| `OVERLAY/AREA19.BIN` | 645335 | 50048 | 1152 | Source |
| `OVERLAY/AREA20.BIN` | 645360 | 18944 | 1536 | Source |
| `OVERLAY/AREA21.BIN` | 645370 | 42880 | 128 | Source |
| `OVERLAY/AREA22.BIN` | 645391 | 2304 | 1792 | Source |
| `STREAM/MUSIC.DAT` | 645393 | 315240448 | 0 | Stream |
| `STREAM/VOICE.DAT` | 799319 | 27822080 | 0 | Stream |
| `MOVIE/E001.PSS` | 812904 | 73383940 | 2044 | Stream |
| `MOVIE/E006S1.PSS` | 848737 | 23953412 | 2044 | Stream |
| `MOVIE/E007.PSS` | 860434 | 22396932 | 2044 | Stream |
| `MOVIE/E008S4.PSS` | 871371 | 10518532 | 2044 | Stream |
| `MOVIE/E010.PSS` | 876508 | 26656772 | 2044 | Stream |
| `MOVIE/E39S2.PSS` | 889525 | 8192004 | 2044 | Stream |
| `MOVIE/E46S4.PSS` | 893526 | 10059780 | 2044 | Stream |
| `MOVIE/E64S5.PSS` | 898439 | 22429700 | 2044 | Stream |
| `MOVIE/E900.PSS` | 909392 | 150945796 | 2044 | Stream |
| `SCUS_971.12` | 983097 | 1532624 | 1328 | Source |
| `SYSTEM.CNF` | 983846 | 57 | 1991 | Opaque |
| `EXTER1.DAT` | 983847 | 45432836 | 2044 | Stream |

`EXTER.BIN` is the original 1 GiB dummy file; it remains a whole opaque file.
`IRX/IOPRP20.IMG` and the IRX modules remain opaque. Movies, music and voice
remain whole streams. Music/voice cue boundaries are described in
`FINDINGS.md`; their cue tables live in the executable. The repacker does not
rewrite those executable tables or transcode the streams.

## ISO9660 layout

- Logical blocks are 2048 bytes. The primary volume descriptor is at LBA 16;
  its terminator is at LBA 17. There are no alternate ISO namespace descriptors.
- Four 78-byte path tables are at LBAs 257/258 (little endian) and 259/260
  (big endian). Each has six entries, in order: root, IRX, DATA, OVERLAY, STREAM,
  MOVIE. Both optional copies are present.
- Those directories occupy LBAs 261–266. Their declared byte lengths are
  596, 514, 212, 1236, 212 and 622, respectively. Their sector remainder bytes
  are preserved with all other non-file data.
- PVD +80 stores volume sectors as little-endian then big-endian u32. +120,
  +124 and +128 store volume set count, volume sequence and logical block size
  as both-endian u16. +132 is both-endian path-table byte size; +140/+144 and
  +148/+152 contain the respective little/big path-table LBA pointers.
- A directory record starts with its byte length; +2 is the file LBA and +10
  the byte length, both as little-endian then big-endian u32. +25 is flags,
  +28 is both-endian u16 volume sequence, +32 identifier length, +33 identifier.
  Zero length skips to the next sector. The original identifier, version,
  timestamp, system-use and reserved bytes remain intact.
- Each path-table entry contains u8 name length, u8 extended-attribute length,
  u32 directory LBA, u16 parent index, name and even-length padding. Counts are
  implicit in table byte size. Directory topology and names stay fixed, so
  repacking does not change any path-table field.
- File payload totals 2,059,637,864 bytes. The remaining 748,440 bytes include
  all filesystem metadata and gaps, not just zeros. File sector-padding totals
  52,120 bytes and is zero on this disc. The final trailer includes a UDF anchor
  and therefore must never be regenerated as blanket zero padding.

## UDF bridge

The disc also carries a UDF namespace pointing to the same file extents.
Volume recognition records BEA01/NSR02/TEA01 are at LBAs 18–20. Anchors are at
LBA 256 and the last sector, 1,006,047. Main and reserve descriptor sequences
begin at LBAs 32 and 48. The physical partition begins at LBA 267 and contains
1,005,780 sectors; the trailing anchor lies immediately outside that partition.
The two partition descriptors are at LBAs 34 and 50 and the logical-volume
integrity descriptor is at LBA 64. UDF names have their original mixed case;
case-insensitive matching identifies the corresponding ISO9660 file.

The file-set descriptor is at LBA 267, its terminator at 268, directory data
at 269–274, and the 49 file entries (six directories plus 43 files) at 275–323.
Directory file-identifier descriptors contain references to those file entries;
they do not hold the payload extent or byte length. Names and file-entry
locations remain fixed during archive growth.

The relocation fields are:

| UDF record | Fields relevant to repacking |
|---|---|
| Descriptor tag (16 bytes) | u16 ID +0; u8 tag checksum +4; u16 descriptor CRC +8; u16 CRC body length +10; u32 tag location +12 |
| Partition descriptor (both copies) | u32 start LBA +188; u32 partition sector count +192 |
| Logical volume descriptor | u32 block size +212; file-set descriptor address +248; u32 partition-map length +264 and count +268; integrity sequence extent +432 |
| Logical volume integrity descriptor | u32 partition count +72; u32 implementation-use length +76; free-sector array +80, then size array (the one-partition disc uses +84) |
| File entry | u64 information length +56; u64 recorded block count +64; u32 extended-attribute length +168; u32 allocation-descriptor length +172; allocation descriptors at +176 + extended-attribute length |
| Short allocation descriptor | u32 length/extent type +0, u32 partition-relative LBA +4; ordinary recorded data uses the lower 30 length bits |
| Anchor | u32 main-sequence length +16 and LBA +20; reserve length +24 and LBA +28; tag location identifies the anchor sector |

Descriptor body CRC uses CRC-16/CCITT (initial zero) over the declared body
length beginning at +16. The tag checksum sums its other 15 bytes modulo 256.
Changed descriptors must update both. The read-only disc has no unallocated
space descriptors or bitmaps advertising reusable sectors: the partition header
space-table/bitmap fields are zero and the unallocated-space descriptor count is
zero. The integrity free-sector count stays zero; old relocated file extents are
not advertised as reusable. File and directory counts do not change.

One original authoring quirk is preserved: `EXTER.BIN` has a 1 GiB information
length and a raw short-allocation length of `0x40000000`. That exceeds the usual
30-bit length field. Pycdlib exposes a zero allocation length but still reads
the full file using its information length; its extracted payload matches
ISO9660. The repacker preserves this known opaque entry verbatim and does not
permit resizing it.

The standard record definitions are in
[ECMA-167, second edition](https://www.ecma-international.org/wp-content/uploads/ECMA-167_2nd_edition_december_1994.pdf).
Generated manifests preserve the local descriptors; the repository contains
only original tooling and these small format/layout descriptions.
