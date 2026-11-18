# Corrected Carver-Richards CPMG model

The base C library provides these models to both OFE-Raku and OFE-Go:

- `R2_CR(tcp,T2A,T2B,tauA,tauB,omegaA,omegaB)` returns R2 = 1/T2 in s^-1.
- `T2_CR(tcp,T2A,T2B,tauA,tauB,omegaA,omegaB)` returns T2 in seconds.
- `R2_CR_pb(tcp,T2A,T2B,pB,kB,dw)` returns R2 using free (A) and bound (B) parameters.

These implement Carver and Richards, J. Magn. Reson. **6** (1972), 89-105,
Appendix A1-A10, DOI [10.1016/0022-2364(72)90090-X](https://doi.org/10.1016/0022-2364(72)90090-X),
with A3-A5 replaced by Hills, Takacs and Belton, Mol. Phys. **67** (1989),
903-918, p.911 equations (1)-(3), DOI
[10.1080/00268978900101531](https://doi.org/10.1080/00268978900101531).
The implementation is `C/local/CarverRichards.c` in the shared C repository.

## Parameters and units

`tcp` is the separation of successive **180-degree pulses**, in seconds.
It is twice the 90-to-180 delay. Convert milliseconds to seconds before fitting.
`T2A`, `T2B`, `tauA` and `tauB` are also in seconds. Residence times are
`tauA=1/kA` for A-to-B exchange and `tauB=1/kB` for B-to-A exchange.
All times must be positive. Positive infinite residence times represent zero
exchange; use the lifetime form for that limit.

`omegaA`, `omegaB` and `dw=omegaB-omegaA` are angular frequencies in **rad/s**,
not Hz. For a shift difference in ppm at a spectrometer frequency in MHz,
`dw=2*pi*frequency_MHz*shift_ppm`. Only the frequency difference matters.

`pB` is the equilibrium bound fraction, with `0<pB<1`, and `kB>0` is in s^-1.
The wrapper enforces detailed balance: `kA=pB*kB/(1-pB)`.
This `kB` is not the total exchange rate, which is `kA+kB`.

The result is the dominant-mode effective rate from A1. It does not predict
finite echo-train amplitudes or alternating echo-height modulation.
The data may not determine every physical parameter independently; fix intrinsic
T2 values and the shift difference from independent measurements when possible.

## Availability

Rebuild and install the shared C source containing these models (core API
6.1.0 or later), then use that engine root with either CLI. No `AuxCode` or
`AuxDeclar` is needed. `onefite help R2_CR_pb` and `onefite list models`
read the installed C catalog. Rebuild the merged catalog with the normal
extension-install step if `META-CATALOG.json` from an older install is present;
an old merged catalog can otherwise hide new base metadata.

OFE-Go release packages bundle the C tag in `engine-sources.txt`. Packages
pinned to C v6.0.4 do not contain this addition; a C release and a pin update
are required before a Go release can bundle it. Do not invent or move a tag.
The catalog/alias changes alone do not update an already installed C library.

OFE-Raku also pins the C core: `C-CODE-VERSION` in `INSTALL` currently
selects `v6.0.3`. `onefite upgrade` updates the Raku checkout, selects that
pinned C tag, and rebuilds it. Publish the new C model in a release tag and
update this installer pin before expecting a normal Raku upgrade to provide
the registered model. The standalone auxiliary JSON can be used meanwhile.

## Example: fit a synthetic reference

From this repository's `examples/carver-richards` directory:

```bash
onefite fit '#CPMG-CR' dispersion.dat --no-plot --work-folder=./work
```

The installed default alias `CPMG-CR` is:

```text
R2(tcp[1e-6<1],T2A=2,T2B=0.0038,pB:0.01[1e-6<0.1],kB:10000[1<1e7],dw=2000)=R2_CR_pb(tcp,T2A,T2B,pB,kB,dw)
```

It fixes `T2A`, `T2B` and `dw`, fitting only `pB` and `kB`. These fixed values
are illustrative, not universal protein or water constants. The synthetic
31-point fixture is independently generated using a two-interval complex
2x2 Bloch-McConnell propagator, with true `pB=0.012` and `kB=8000 s^-1`.
It is a numerical regression example, not experimental validation.

A local `aliases.json` can override installed aliases. If the alias is missing
or overridden in an older installation, pass the full expression directly:

```bash
onefite fit 'R2(tcp,T2A=2,T2B=0.0038,pB:0.01[1e-6<0.1],kB:10000[1<1e7],dw=2000)=R2_CR_pb(tcp,T2A,T2B,pB,kB,dw)' dispersion.dat --no-plot --work-folder ./work
```
