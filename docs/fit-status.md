# Saved MINUIT status

A newly saved fit includes additive `fit-status` metadata. Numerical output,
fit methods and exit semantics are unchanged. The engine reads its work logs
before saving, so clients such as OneNMRD on Windows need only the resulting
JSON, even when the work directory is inside WSL. The object also survives SAV
round trips through the existing nested-object support.

```json
{
  "fit-status": {
    "version": 1,
    "blocks": [
      {
        "block": 1,
        "tag": "run-z001",
        "scope": "block",
        "log": "fit1.log",
        "migrad": "CONVERGED",
        "migradErrorMatrix": "accurate",
        "minos": "SUCCESSFUL",
        "finalCommand": "MINOS",
        "finalStatus": "SUCCESSFUL",
        "errorMatrix": "accurate"
      }
    ]
  }
}
```

`block` is the one-based engine data-block number; `tag` is the exact tag used
in the result table. Consumers must not equate it with a numeric tag suffix.
Individual/MIXED fits use each `fitN.log`. Plain global and hybrid fits share
`fit.log` and identify that evidence with `scope: "global"`.

`migrad` and `migradErrorMatrix` describe the last printed MIGRAD report.
`errorMatrix` describes the final printed MINUIT report, which can be from a
later MINOS or HESSE command. `minos`, when present, describes the last MINOS
report after that MIGRAD, not an older MINOS preceding a new minimization.
MINOS can change the covariance or discover another minimum; its problems
must not be hidden by earlier MIGRAD convergence. The corresponding raw
`migradLine`, `minosLine` and `errorMatrixLine` preserve the source statements.
No commands or mathematical results are changed by this reporting.

Matrix values are `accurate`, `approximate`, `forced-positive-definite`,
`not-positive-definite`, `none` or `unknown`. MINUIT's `UNCERTAINTY=` text
alone does not distinguish accurate from approximate covariance; MIGRAD/HESSE
parameter headers and notes are used. A MINOS `PARABOLIC` header alone does
not establish covariance accuracy. Missing logs/reports or ambiguous evidence
remain unknown. Older files have no status object. Refit saves replace
inherited status with evidence from the current invocation.

These fields report MINUIT's diagnostic state, not scientific model validity,
identifiability, uncertainty calibration or a new success/failure exit policy.
Consumers should preserve fitted values and mark unreliable fields rather than
blanking an entire run. OFE-Go and OFE-Raku write the same versioned object. No OFE-GUI change is
needed to preserve it. Ship engine support before relying on it in clients.
