# Flapping Butterfly Clip

A wing-flapping butterfly hair clip on an **ESP32-C3 with integrated 0.42" OLED**,
built in two phases for a student project:

| Phase | Power | Servo | Trigger | Env |
|---|---|---|---|---|
| 1 - bench | USB | SG92R / MG90S (5 V) | push-button | `bench` |
| 2 - wearable | 1S LiPo (+ optional MT3608) | 2 g, 3.7-4.2 V | TTP223 touch | `wearable` |

The OLED shows state, battery voltage/percent and servo angle. Wings flap with sine easing;
a low-battery cutoff stops them and latches until reboot.

## Quick start (VS Code + PlatformIO)

```bash
pio run -e hwcheck -t upload && pio device monitor   # 1. confirm your board's pins
pio run -e bench    -t upload                        # 2. Phase 1
pio run -e wearable -t upload                        # 3. Phase 2
```

Wiring: [`hardware/WIRING.md`](hardware/WIRING.md) · Parts: [`hardware/BOM.md`](hardware/BOM.md) ·
Full guide with concepts explained: `docs/guide.pdf` (build with `cd docs && latexmk -pdf guide.tex`).

## Layout

```
include/config.h    all pins and tunables (start here)
include/*.h         WingServo, TouchButton, Battery, StatusDisplay (each documented)
src/main.cpp        state machine: IDLE / FLAPPING / LOW_BATT
tools/hwcheck/      I2C scan + ADC check
hardware/           BOM and wiring
docs/               LaTeX guide
```

## Safety

LiPo cells can burn. Use a cell with a protection PCB, a hard power switch, charge it **off the clip**
at the cell's rated current, never unattended, and enclose the gears. Details in the guide.

## Credit

Concept and parts approach come from the *Flapping Butterfly Clip v2 Upgrades* guide on Notion by its
original author (https://like-nose-ff4.notion.site/Flapping-Butterfly-Clip-v2-Upgrades-3f82c3bc670780679308c135163affcc),
who also published v1 with the gear mechanism. This repository contains an independent firmware rewrite
and documentation; no STL files or original images are included. Please support the original author.

## License

MIT, see [LICENSE](LICENSE).
