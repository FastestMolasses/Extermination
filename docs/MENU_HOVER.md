# Original menu hover selection

`func_0020D930` selects a menu entry from the normalized analog stick. Table 0 is the normal status hub; table 1 is ITEM. The remaining selector values share the alternate table. A changed nonzero selection plays cue 5; neutral input clears the hover byte without a cue.

The readable C now preserves the original float-to-soft-double magnitude comparison and corrects two wrong branches: table 0 maps angles below −3π/4 to entry 4, and the alternate table maps its last angular sector to entry 1. These are source corrections, not proposed visual or input redesigns.

The native repository's `tools/test_menu_hover_source_reference.py` compiles this canonical C with explicit stick/audio workers and compares it with the original ELF instructions. All 8,064 cases agree in hover state and sound calls, across selector values −1, 0, 1, 2, 3, and 255, all prior hover values, neutral/threshold magnitudes and angular boundaries. The reference executes the original soft-double conversion and comparison. Its bounded EE runner is test infrastructure, not a game runtime.

Measured with mwcc 2.3.3, `-O4,p -sdatathreshold 0`, the corrected function is 72.58049% similar to the original 1,640-byte function; the candidate object contains 1,488 bytes. Receipt: ignored `build/item_root/d930_hub_result.json`. It remains NEARMISS and the linked boot image continues to use original assembly. Remaining code-generation differences are unclassified.

Validation: `.venv/bin/python tools/verify_all.py` passes all six stages after these corrections: byte-identical boot loadable region (0x175b00 bytes), 19/19 byte-identical overlays, match report, sample glTF, animation decoder self-test and GS VRAM offset check. Full receipt: ignored `build/item_root/verify_d930_hub.log`. This does not claim a new compiled-C byte match.
