## Summary
`pulseIn(ECHO_PIN, HIGH)` has **no timeout** — when the HC-SR04 returns no echo (target out of range or absorbing), `loop()` hangs forever and the LCD/servo freeze.

## Expected Behavior
A missing echo is handled gracefully ("lane clear", `999` cm fallback) — the state machine keeps running.

## Actual Behavior
Aiming the sensor at open air (>4 m) freezes the entire controller.

## Steps to Reproduce
1. Upload baseline firmware (`65de664`).
2. Point the HC-SR04 at open space beyond 4 m.
3. Watch the LCD stop updating and servo stall.

## Root Cause Analysis (5-Why)
1. **Why** does loop() hang? `pulseIn()` never times out.
2. **Why** no echo? Target out of range / soft absorbent material.
3. **Why** is out-of-range not handled? Baseline assumed an echo always arrives.
4. **Why** is that assumption unsafe? Real environments have range gaps.
5. **Why** does handling matter? A hung controller cannot close the barrier behind a vehicle.

**ROOT CAUSE:** missing timeout and "no echo" fallback on the ultrasonic read path.

## Labels
severity: high · type: bug · Activity 2 / CO2

## Proposed Resolution
- Branch: `fix/issue-4-echo-timeout`
- Approach: add 30 ms explicit timeout `pulseIn(ECHO_PIN, HIGH, 30000UL)`; on timeout return `999` (lane clear).
- Verification plan: aiming at open air yields a graceful "lane clear" state instead of a freeze.

## Resolution Log
| Date | Commit | Note |
|------|--------|------|
| 2026-10-13 | `109c782` | fixes #4 — echo timeout + fallback merged to main |
