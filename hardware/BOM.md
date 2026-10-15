# Bill of Materials

Two phases: **Phase 1 = bench prototype** (parts on hand, no purchases), **Phase 2 = wearable clip**
(small, light, battery powered). This file is the Phase 2 BOM; Phase 1 substitutions are listed at the end.

Status: **HAVE** = already in the workshop inventory, **BUY** = needs ordering, **VERIFY** = confirm
against the physical part before ordering/committing.

## Phase 2 — wearable clip

| # | Item | Qty | Spec / notes | Status |
|---|------|-----|--------------|--------|
| 1 | ESP32-C3 board with integrated 0.42" OLED | 1 | RISC-V, USB-C, 72x40 px OLED. Runs from the `5V` pin via its own 3.3 V LDO. VERIFY board outline and whether a battery connector exists. | HAVE (3 free) |
| 2 | Micro servo, **2 g, 3.7–4.2 V** | 1 | The weight-critical part. A 9 g SG92R / 13 g MG90S will not sit on hair. Fallback: any 3.7 g micro servo rated for 1S LiPo. | BUY |
| 3 | 1S LiPo, 3.7 V, **150–300 mAh**, with protection PCB | 1 | e.g. 501230 / 502030 class. Must have a PCB (over-discharge + short-circuit). Check the connector polarity and pitch before plugging in. | BUY |
| 4 | Boost converter, 5 V out (MT3608) | 1 | Set to 5.0 V with a multimeter **before** connecting the board. Optional, see "Power note". | HAVE (1) + 5 in cart |
| 5 | TTP223 capacitive touch module | 1 | Digital out, works at 3.3 V. Needed for touch on/off. | BUY |
| 6 | Slide switch (SS12D00G) | 1 | Hard battery cut-off. The original guide has none. | BUY (on to-buy list) |
| 7 | Silicone hook-up wire, 28–30 AWG | ~1 m | Flexible and light. The 22 AWG solid core on the to-buy list is too stiff for a clip. | BUY |
| 8 | Electrolytic capacitor, 100–470 µF, ≥10 V | 1 | Across the 5 V rail to stop servo-induced resets. Pick the smallest can that fits. | HAVE (kit) |
| 9 | Resistors 100 kΩ | 2 | Battery-voltage divider to an ADC pin. | HAVE (kit) |
| 10 | Heat-shrink tubing, assorted | few cm | Insulate every joint. Keep electronics off the metal clip and the skin. | HAVE |
| 11 | Magnets 5 mm, ~8 | 8 | Optional: snap-on wings. Check the diameter and thickness against the wing STL before ordering. | BUY (optional) |
| 12 | M3 heat-set inserts + screws | few | Close the printed case so it can be opened repeatedly. | HAVE |
| 13 | Single-cell LiPo charger (TP4056) | 1 | **Off-clip** charging only. Set the current to the cell's rating (swap Rprog for small cells). | HAVE (4) |
| 14 | 3D-printed case, wings, gear train | — | PLA/PETG. Gear design: see the original v1 guide. | PRINT |

Tools (all on hand): soldering station, multimeter, tweezers, 3D printer.

## Power note

- **With the boost (as listed):** battery → switch → MT3608 (5 V) → board `5V` pin and servo. The board
  stays stable down to ~3.0 V on the cell. Costs ~2 g, one part, and ~15 % conversion loss.
- **Without the boost (optional lighter variant):** with a 3.7–4.2 V servo you can feed the cell straight
  to the board `5V` pin and the servo. The board LDO drops out near 3.5 V, so the firmware cutoff moves
  to ~3.5 V and you lose some capacity. Decide after measuring in Phase 1.
- **Runtime (estimate, not measured):** servo moving ~100–150 mA plus board and OLED ~40 mA gives roughly
  150–200 mA from the cell, so ~1–1.5 h from 250 mAh. Phase 1 should measure the real figure.
- **Firmware cutoff:** stop the wings and show a low-battery message at ~3.3 V (3.5 V without the boost).
  The cell's PCB cuts at ~2.5 V, which is far too low to rely on.

## Phase 1 — bench prototype (no purchases)

| Phase 2 part | Phase 1 stand-in | Notes |
|---|---|---|
| 2 g servo | SG92R (untested) or MG90S (6 free) | Power from the 5 V rail, not the cell. Test the SG92R first, it's still on the loose-ends list. |
| Small LiPo | 455572 2000 mAh (has PCB) | Bench only, too big to wear. |
| TTP223 | Tactile button to GND, `INPUT_PULLUP` | Keep the touch module for Phase 2. |
| Slide switch | Unplug the cell | — |
| 28–30 AWG wire | Breadboard + jumpers | — |

## Open items

- [ ] Verify the C3 board's pinout (I²C pins, free GPIOs, ADC-capable pins) and battery connector.
- [ ] Choose and verify the exact LiPo size against the case.
- [ ] Measure Phase 1 current to confirm the runtime estimate and the boost decision.
