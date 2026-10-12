## Summary
Any single PIR HIGH instantly opens the barrier — stray thermal pulses (sunlight, heater draft) cause false opens with no vehicle present.

## Expected Behavior
A vehicle is confirmed only after sustained PIR presence; one-shot pulses are ignored.

## Actual Behavior
Barrier opens spuriously when a warm breeze / sunlight hits the PIR.

## Steps to Reproduce
1. Upload baseline firmware (`65de664`).
2. Aim a warm-air source at the PIR dome for 2 s.
3. Observe barrier opening with no vehicle.

## Root Cause Analysis (5-Why)
1. **Why** does the barrier open with no vehicle? PIR output goes HIGH.
2. **Why** does the PIR go HIGH? Stray thermal pulses.
3. **Why** does a single pulse open the gate? Baseline treats any HIGH as valid.
4. **Why** no settle window? Baseline lacks sample-confirmation logic.
5. **Why** does sample confirmation help? Real vehicle occupancy produces sustained IR, not one-shot spikes.

**ROOT CAUSE:** insufficient temporal filtering on the PIR input.

## Labels
severity: medium · type: bug · Activity 2 / CO2

## Proposed Resolution
- Branch: `fix/issue-3-pir-debounce`
- Approach: require `PIR_SETTLE_SAMPLE = 10` consecutive 50 ms HIGH samples; any LOW resets the counter.
- Verification plan: 5 min thermal-interference test → zero false opens; real approach still triggers within ~0.5 s.

## Resolution Log
| Date | Commit | Note |
|------|--------|------|
| 2026-10-13 | `ef917c4` | fixes #3 — 10-sample confirmation merged to main |
