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
cad/                OpenSCAD source (butterfly.scad), build script, assembly preview
stl/                exported printable parts
hardware/           BOM and wiring
docs/               LaTeX guide
```

## Safety

LiPo cells can burn. Use a cell with a protection PCB, a hard power switch, charge it **off the clip**
at the cell's rated current, never unattended, and enclose the gears. Details in the guide.

## Credit

This project is inspired by the **Flapping Butterfly Clip** by **@resetwithregine** on TikTok
([video](https://www.tiktok.com/@resetwithregine/video/7689586414178061589)) and the
*Flapping Butterfly Clip v2 Upgrades* build guide
([Notion](https://like-nose-ff4.notion.site/Flapping-Butterfly-Clip-v2-Upgrades-3f82c3bc670780679308c135163affcc)),
who also published v1 with the original gear mechanism. This repository contains an independent
firmware rewrite, CAD and documentation; no STL files, code or images from the original are included.
Please support the original creator.

## If you use or adapt this

Please give credit: link back to this repository (https://github.com/Electronics-AndresForero/flapping-butterfly-clip)
and to the original *Flapping Butterfly Clip* author above, whose idea inspired it. The MIT license
requires keeping the copyright notice; crediting is how we ask you to honour the people behind the idea.
Student projects and remixes are very welcome.

## License

MIT, see [LICENSE](LICENSE).
