These are captured numerical engine logs, not MINUIT source code.

- `converged.log`: the real Go fit log retained during the 18 November 2026
  Carver–Richards integration validation (`go-fit1-log`).
- `call-limit.log`: the real Florence SS audit fit log retained on 30 October
  2026 (`florenceN_fH_SS_test/fit.log`).

Both histories include intermediate and final reports. Tests must distinguish
last MIGRAD evidence from subsequent MINOS/final-matrix reports rather than
searching the whole file for warning words. Identical fixtures are used in the
Raku engine tests to keep parser evidence consistent across the two engines.
