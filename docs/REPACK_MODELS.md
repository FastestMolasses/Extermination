# Design for a topology-changing native model compiler

This document specifies future work. The current `tools/repack/models.py`
editor only patches attributes in existing packets. It does not implement
re-topology, new materials, new palette nodes, new animation channels or a
general glTF importer. An unchanged supported model must retain every native
byte; that remains a separate requirement from compiling a new mesh.

## Current boundary

The established actor path consumes framed STCYCL(4,4), UNPACK V4-32 batches
of 32 four-qword records. The records hold TEX0, normalized ST, an authored
normal and a bone-local position. The position-W word also carries a rigid
matrix address, strip restart and winding information. One matrix address is
one influence with weight 1. There is no evidence of arbitrary blended weights
in this packet class. Static colour-kernel packets instead carry authored RGB
with a distinct fourth component; normal and colour modes must not be inferred
from a vector's length alone.

The current view preserves every packet boundary, vertex identity, material,
index, rigid weight, header and unknown byte. Repeated tail records are linked
to their original editable vertex. The glTF skin uses identity palette nodes
because animation-generated transforms live outside the model. It therefore
shows native bone-local geometry, not a promised assembled rest pose.

The current importer compares those native semantics, not accessor numbers or
JSON formatting. It accepts reordered accessors/views, changed buffer packing,
strided attributes, multiple local or embedded buffers, harmless names/extras,
and explicit identity node/bind transforms. It still requires each primitive's
original vertex sequence and custom `_NATIVE_ID` pairs, identical indices and
material/skin/node meaning. A DCC export that welds, splits, reorders vertices,
drops custom identities or changes transforms is rejected. Sparse accessors,
GLB containers, remote buffers and compression extensions are outside this
import surface. The output follows [glTF 2.0's vertex attribute and skin rules](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html):
native identities use two unsigned-short components and palette joints share
an explicit common root.

Actor header bounds are not the bounds of the stored bone-local vertex list.
They cannot safely be replaced by the glTF POSITION min/max. The current
encoder restricts positions to each existing joint's original local envelope;
static edits stay within each packet's original envelope, rather than merely
within a whole scene's bounds. It preserves the original culling metadata. Extending geometry beyond that
envelope requires the culling investigation below.

## Semantic intermediate representation

Separate source data from transport packets. The intermediate representation
needs vertices with stable identities, native-space positions, normals or
colours, UVs, explicit rigid influence indices, triangle topology, draw order,
material references, object/segment identity and native culling metadata. Store
the original record provenance and opaque spans alongside decoded fields.

Keep material identity separate from the native GS state needed to draw it:
TEX0 texture/palette address, pixel storage mode, dimensions, buffer width and
CLUT load control; TEX1 sampling, CLAMP, PRIM, ALPHA, TEST and other packet state
must be audited before the compiler emits altered material packets. A glTF
material is not an equivalent replacement for this state. Preserve draw order
and transparency classification explicitly.

The importer must reject unsupported skins, morph targets, tangent-space
normal maps, mesh compression, instancing extensions and animation changes
until each has a native representation. Coordinate units, handedness and
normal transforms must be explicit, reversible import options. Applying node
transforms requires transforming normals and accounting for mirrored winding.

## Native packet classes

Implement separately validated emitters instead of treating all discovered
UNPACK bytes as a universal mesh format:

1. Framed actor blobs with a declared block count, QWC, palette size and byte
   size. Preserve the 32-record double-buffered VU input contract until a
   different capacity is independently established.
2. Packed model containers with multiple variant/LOD segments and their own
   matrix descriptors. Resolve segment ownership and embedded directories
   before rebuilding offsets.
3. Static level colour packets, including SUBMESH headers and MATRIX instance
   sections. Identify leading unframed geometry separately; a bounded MESH
   scanner is not a complete level compiler.
4. Other quantized or per-bone packet classes only after their position,
   normal, UV, palette and output-kernel contracts are independently decoded.
   Do not reuse a float-record emitter for Q-format payloads.

For each class, record an explicit VIF state contract: cycle lengths, unpack
format/count/address, TOPS/base/offset use, masking and signedness, VU program
entry, scratch addresses, palette region and output GIF region. Reject a
packet if any field needs an unknown conversion.

## Triangle scheduling and VU limits

Build a deterministic stripifier, then split strips into legal packets.
Respect native ADC/restart and parity semantics, including carry vertices at
packet boundaries. A triangle's kick vertex selects its native material; the
compiler cannot change this by grouping arbitrary vertex records by texture.
Tail padding must follow a proven native pattern without introducing visible
triangles. Retain degenerate triangles required to join strips, but distinguish
them from editable geometry in the intermediate representation.

Validate every generated batch against VU data memory, input/output overlap,
double-buffer spacing, matrix slots, instruction entry and DMA QWC limits.
Do not infer available space merely from the 1,024-qword VU memory size.
Recompute all packet and parent blob counts/sizes from the emitted bytes and
verify them using an independent decoder.

## Skeletons and weights

For the supported rigid actor class, map a glTF joint to the engine palette
slot explicitly. The low position-W bits encode an eight-qword-stride matrix
address; restart/parity bits remain separate. New nodes require matching
animation parent tables, model node metadata, matrix allocation limits and all
runtime users, not just a larger glTF skin.

Fractional weights require identifying a different existing kernel or
implementing a new kernel and its dispatcher. They cannot be encoded by
rounding weights into the current rigid selector. Splitting vertices between
rigid joints is not an equivalent approximation and must never happen
silently. Bind/inverse-bind transforms require a verified conversion between
glTF common bind space and the game's stored bone-local coordinates.

## Bounds, collision and quantization

Trace actor header bounds into the runtime culling call sites and determine
the authored pose/space and radius or extent convention. Test a large mesh
edit near view-frustum edges to distinguish actual culling from camera or
clipping effects. Maintain model, submesh, instance and any acceleration bounds
at their respective levels. A graphics edit must not silently rewrite an
independent collision hull or world collision mesh.

For a quantized packet class, specify scale, origin, signed range and rounding
mode for every component. Reject overflow. Report the maximum reconstruction
error and require an explicit lossy option for nonrepresentable values. Preserve
exact source encoding when the semantic attribute is unchanged.

## Relocation and integration

The emitter returns a complete native leaf plus an explicit relocation table
for internal offsets, DMA references, model directories and segment pointers.
Classify pointers as file-relative, container-relative, load-address-relative
or patched at runtime. Unknown pointer-like words stay opaque until proven;
blindly shifting every word in an address range is unacceptable.

Only after internal relocation validates should archive packing update leaf
lengths and DATA/INDEX offsets. The archive layer handles disc-sector framing;
it cannot repair internal VIF/DMA alignment. Rebuild any shared library entry
directory and update every known reference to moved material or model data.
Texture reallocation also needs a complete reference inventory across native
leaves, overlays and boot-owned sprite/material tables.

## Acceptance gates

Use original tooling plus synthetic fixtures only in version control. Keep
BYO-disc native payloads, intermediate glTF and receipts under ignored
`build/repack/model-edit/`.

The topology compiler needs independent tests for packet bounds, winding and
restarts, per-corner material assignment, split strips, rigid transforms,
duplicate vertices, every supported quantization boundary, relocation growth
and malformed input rejection. Unchanged inputs must still select the original
bytes rather than normalizing source encodings.

For edited real meshes, compare output geometry/materials through the existing
forward exporters, rebuild the archive and ISO, then cold-boot that disc in the
original game. Confirm the edited native data was actually loaded, capture a
visible geometry change, and test camera/culling behavior. A viewer render or
successful container round trip alone does not establish runtime compatibility.
