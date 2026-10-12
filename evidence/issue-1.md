## Summary
`loop()` calls `delay(200)` at the top of every cycle, blocking the CPU for 200 ms.

## Expected Behavior
LCD refreshes smoothly, servo sweeps continuously, headroom exists for future features (buzzer, alarm).

## Actual Behavior
Display updates at only 5 Hz; servo motion is jerky; no concurrent tasks are possible.

## Steps to Reproduce
1. Upload baseline firmware (commit `65de664`).
2. Watch the LCD refresh rate.
3. Observe the servo visibly stepping instead of sweeping smoothly.

## Root Cause Analysis (5-Why)
1. **Why** does the display freeze? Because `delay()` blocks the CPU.
2. **Why** was `delay()` chosen? As a quick way to pace the sensing loop.
3. **Why** is pacing needed? To avoid re-triggering the ultrasonic sensor too fast.
4. **Why** not a non-blocking timer? The baseline lacked `millis()` bookkeeping.
5. **Why** does this matter? Future features (servo ramps, alarms) need concurrent timing.

**ROOT CAUSE:** synchronous blocking scheduling.

## Labels
severity: high · type: bug · Activity 2 / CO2

## Proposed Resolution
- Branch: `fix/issue-1-delay-to-millis`
- Approach: replace `delay(200)` with a `millis()`-based non-blocking tick (`SENSOR_TICK_MS = 50` sensing, `SERVO_SWEEP_MS = 15` servo stepping).
- Verification plan: LCD should refresh every 50 ms with no freezing; servo sweep should appear smooth.

## Resolution Log
| Date | Commit | Note |
|------|--------|------|
| 2026-10-13 | `43b846c` | fixes #1 — non-blocking scheduler merged to main |
