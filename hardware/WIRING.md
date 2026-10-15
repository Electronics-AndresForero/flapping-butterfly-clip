# Wiring

Board: ESP32-C3 with integrated 0.42" OLED (72x40). Silkscreen: left header 3-10,
right header 0, 1, 2, TX, RX, V3 (3.3 V), GD (GND), V5 (5 V). **GPIO5/GPIO6 belong to the OLED.**
Confirm with `pio run -e hwcheck -t upload` before wiring anything.

## Pin map (both phases)

| Signal | Board pin | Notes |
|---|---|---|
| Servo signal | GPIO3 | |
| Trigger input | GPIO4 | Phase 1: button to GND. Phase 2: TTP223 `OUT`. |
| Battery sense | GPIO0 | Phase 2 only, ADC1 channel 0 |
| OLED SDA / SCL | GPIO5 / GPIO6 | On-board, don't touch |
| Free | GPIO1, GPIO7, GPIO10, TX, RX | GPIO2/8/9 are boot-strapping pins: avoid |

## Phase 1 - bench (USB powered)

| From | To |
|---|---|
| Servo red | `V5` (USB 5 V) |
| Servo brown/black | `GD` |
| Servo signal | GPIO3 |
| Button leg A | GPIO4 |
| Button leg B | `GD` |
| 470 uF cap (+ / -) | `V5` / `GD`, close to the servo |

Only a 5 V-rated micro servo (SG92R, MG90S). One small servo on USB is fine; never power several.

## Phase 2 - wearable (single LiPo)

| From | To |
|---|---|
| Battery + | Slide switch in |
| Switch out (`VBAT`) | MT3608 `IN+`, servo red, divider top (100 k) |
| Battery - | Common GND (MT3608 `IN-`/`OUT-`, board `GD`, servo brown, TTP223 GND, divider bottom) |
| MT3608 `OUT+` (5.0 V, set first!) | Board `V5` |
| 100 k / 100 k midpoint | GPIO0 |
| TTP223 VCC | Board `V3` (3.3 V) |
| TTP223 OUT | GPIO4 |
| Servo signal | GPIO3 |
| 100-470 uF cap | MT3608 `OUT+` / GND |

**Servo supply rule:** a 3.7-4.2 V servo goes on `VBAT` (battery), **not** the boosted 5 V.
A 5 V-rated servo goes on the MT3608 output instead. Never mix them up.

No-boost variant: connect `VBAT` straight to board `V5`, drop the MT3608, and raise
`VBAT_CUTOFF_MV` to 3500 in `include/config.h`.
