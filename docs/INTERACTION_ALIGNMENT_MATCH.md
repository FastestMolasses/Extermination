# Interaction angle wrapper and placement

Original001B1470 now uses ordinary readable C. Its two loops wrap into the
interval(-pi,pi], including conversion of the negative-pi endpoint to
positive-pi. The mwcc233 O4,p/sdatathreshold0 build matches all156 function
bytes at100%; its original linked slot includes4 trailing padding bytes.

All six verification gates pass after canonical integration. The fresh
build/angle_wrap_match/link_provenance.json confirms that this function is
selected from its freshly compiled ordinary-C object and that both text and
relocations match the prepared link filler. No stale assembly fallback is
being counted as the new C implementation.

There are now1511 ordinary-C sources,637 inline-assembly sources,790 NEARMISS
sources and15 include-assembly stubs. Of3010 boot slots,1225 select ordinary-C
objects (314608 padded bytes),536 select inline-assembly objects and1249 use
original-assembly routes. All1761 selected objects preserve exact prepared
text/relocations. Objdiff coverage remains98.16% with2051/2148 tracked units;
replacing an already exact inline-assembly unit improves readable C, not that
headline score.

The native companion separately checks001B6F00 and its complete SDK transform
in1639 cases, including three captured panel world matrices. The homogeneous
local interaction point transforms through owner+D0 before retaining current
player ground Y. That actor world transform is distinct from a displayed bone
matrix. The final182F90 position-mirror application has its own original test.
