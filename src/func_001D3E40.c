// NEARMISS func_001D3E40  (vram 0x001D3E40, 0x110 bytes) — readable decompilation, NOT byte-identical.
//
// objdiff 81.26% via mwcc 2.3 (mwcps2-2.3-991202) (-O4,p -sdatathreshold 4). Object similarity does not prove semantic equivalence.
// Remaining differences in this candidate:
// GPR allocation, channel-offset lifetime and reload scheduling differ; candidate is 4 bytes longer. The channel argument correction is independently checked by 384 original-instruction DMA cases.
//
// Boot ELF stays byte-identical: the linker fills this function from the splat .s, NOT
// from this C (// NEARMISS is treated like a stub). Not compiled / not an objdiff unit /
// excluded from matched_code. Registry: docs/NEARMISS.md.
//
// COMPILER: mwcc
// CFLAGS: -O4,p -sdatathreshold 4

/* Submit a face mesh through the selected DMA channel. The first helper
 * takes the channel index, NOT model+0x40; it installs the morph program.
 * The following REF tags upload per-scene constants, an optional two-qword
 * setup block, and the model's VIF packets. Reloads through the channel
 * cursor preserve the original store order and aliasing behavior. */
extern void vif_append_ref_tag(int channel, void *program);
extern int func_001D2910(int a0);

extern char *D_00275670;
extern char D_0023C480[];
extern char D_00816440[];
extern char D_002514B0[];

void func_001D3E40(int channel, char *model)
{
    int channel_offset;
    char *mesh_packets;
    char *channel_base;
    int *dma_tag;

    vif_append_ref_tag(channel, D_0023C480);
    mesh_packets = model + 0x40;

    channel_offset = channel << 2;
    channel_base = D_00275670 + channel_offset;
    dma_tag = *(int **)(channel_base + 0x10);
    *(char *)((char *)dma_tag + 3) = 0x30;
    dma_tag = *(int **)(channel_base + 0x10);
    *(int *)((char *)dma_tag + 4) = (int)(D_00816440 + (*(int *)(D_00275670 + 0x9C) << 7));
    dma_tag = *(int **)(channel_base + 0x10);
    *(short *)dma_tag = 8;
    dma_tag = *(int **)(channel_base + 0x10);
    *(int **)(channel_base + 0x10) = (int *)((char *)dma_tag + 0x10);

    if (func_001D2910(0) == 0) {
        channel_base = D_00275670 + channel_offset;
        dma_tag = *(int **)(channel_base + 0x10);
        *(char *)((char *)dma_tag + 3) = 0x30;
        dma_tag = *(int **)(channel_base + 0x10);
        *(int *)((char *)dma_tag + 4) = (int)D_002514B0;
        dma_tag = *(int **)(channel_base + 0x10);
        *(short *)dma_tag = 2;
        dma_tag = *(int **)(channel_base + 0x10);
        *(int **)(channel_base + 0x10) = (int *)((char *)dma_tag + 0x10);
    }

    channel_base = D_00275670 + channel_offset;
    dma_tag = *(int **)(channel_base + 0x10);
    *(char *)((char *)dma_tag + 3) = 0x30;
    dma_tag = *(int **)(channel_base + 0x10);
    *(int *)((char *)dma_tag + 4) = (int)mesh_packets;
    dma_tag = *(int **)(channel_base + 0x10);
    *(short *)dma_tag = *(int *)(model + 4);
    dma_tag = *(int **)(channel_base + 0x10);
    *(int **)(channel_base + 0x10) = (int *)((char *)dma_tag + 0x10);
}
