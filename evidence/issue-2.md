## Summary
A single hard threshold (`distance < 30`) with no hysteresis causes the barrier/LED to stutter open-closed when jitter bounces the reading around the boundary.

## Expected Behavior
Barrier state changes only when the lane is decisively clear, tolerating ±1–2 cm of measurement noise.

## Actual Behavior
With a vehicle idling near 29–31 cm, the barrier toggles OPEN/CLOSED rapidly and the LEDs flicker.

## Steps to Reproduce
1. Upload baseline firmware (`65de664`).
2. Place a box at ~30 cm from the HC-SR04.
3. Watch the barrier arm and LED stutter.

## Root Cause Analysis (5-Why)
1. **Why** does the barrier stutter? Every raw reading flips the comparison.
2. **Why** does each reading change the output? No dead-band.
3. **Why** no dead-band? Baseline used a single hard-threshold comparison.
4. **Why** is hysteresis needed? Ultrasonic noise of ±1 cm flips the boundary.
5. **Why** is boundary behaviour critical? Repeated open/close at one spot is a safety risk.

**ROOT CAUSE:** absence of a hysteresis band on the distance decision.

## Labels
severity: high · type: bug · Activity 2 / CO2

## Proposed Resolution
- Branch: `fix/issue-2-hysteresis`
- Approach: add `DIST_HYST_CM = 5`; closing commits only when `distance > CLEAR_DISTANCE_CM + DIST_HYST_CM`.
- Verification plan: sweep a target between 26–34 cm; zero state toggles expected.

## Resolution Log
| Date | Commit | Note |
|------|--------|------|
| 2026-10-13 | `e4cb6d2` | fixes #2 — hysteresis band merged to main |
