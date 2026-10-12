## Summary
LCD backlight is switched on once in `setup()` and never powered down — constant ~25 mA draw even when the lane is idle for long periods.

## Expected Behavior
Backlight should turn on with activity and switch off after extended idle to save power (relevant for a battery/solar powered gate).

## Actual Behavior
Backlight stays on permanently; measured idle current ~48 mA (vs ~23 mA with backlight off).

## Steps to Reproduce
1. Upload baseline firmware (`65de664`).
2. Leave the board idle for 2 minutes.
3. Measure current draw — no reduction occurs.

## Root Cause Analysis (5-Why)
1. **Why** is current consumed at idle? Backlight LED is always on.
2. **Why** is it on? `lcd.backlight()` is called once in `setup()`.
3. **Why** was it set unconditionally? Baseline never revisits display state.
4. **Why** does that matter? For battery/solar gate it is a lifetime energy cost.
5. **Why** fix rather than accept? One-line, low-risk improvement with clear power savings.

**ROOT CAUSE:** no power-saving policy for the auxiliary display.

## Labels
severity: low · type: enhancement · Activity 2 / CO2

## Proposed Resolution
- Branch: `fix/issue-5-lcd-power-save`
- Approach: backlight on when `state != IDLE` or samples are pending; off after `LCD_IDLE_MS = 10000` ms of inactivity; `lcdBacklightIsOn()` helper tracks state.
- Verification plan: measure idle current before/after; confirm no usability loss on next vehicle.

## Resolution Log
| Date | Commit | Note |
|------|--------|------|
| 2026-10-13 | `061affc` | fixes #5 — auto power-save merged to main |
