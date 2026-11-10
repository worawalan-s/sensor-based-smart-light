# Smart Touchless Hallway Lighting System

> **Term Project** — 2110366 Embedded System Lab I (Academic Year 2026/1)  
> Department of Computer Engineering, Faculty of Engineering, Chulalongkorn University

---

## 📖 Project Overview

The **Smart Touchless Hallway Lighting System** is an energy-efficient and touchless building automation solution designed for safety and automated illuminance control. The illumination load turns ON automatically only when two conditions are simultaneously satisfied:

1. **Ambient Darkness**: The photoresistor detects that ambient illuminance has dropped below a calibrated threshold.
2. **Obstacle Presence**: The active infrared (IR) sensor detects a person or object in detection range.

Upon a **Qualified Trigger Event**, the system energizes the relay to turn on the lamp, initiating a non-blocking countdown (**Hold Duration** of 15 seconds default). The timer is **retriggerable** through **Obstacle Presence refresh** while the lamp is illuminated. Additionally, the system incorporates an optical **Light Lockout** mechanism to prevent optical feedback oscillation (where the illuminated lamp fools the light sensor into abruptly turning off).

> **Prototype safety scope:** This course prototype uses a low-voltage LED/lamp load only. Connecting or testing a mains-powered load is out of scope.

---

## 🛠️ Bill of Materials (BOM)

| Item | Module Identifier | Description | System Role |
| :--- | :--- | :--- | :--- |
| **Microcontroller** | NUCLEO-F411RE | STM32 ARM Cortex-M4 @ 100 MHz development board | Main processor & FSM controller |
| **No. 29** | KY-032 | Obstacle Avoidance Sensor Module (Active IR) | Proximity / tripwire obstacle detection |
| **No. 16** | KY-018 | Photoresistor Module (LDR) | Ambient illuminance measurement |
| **No. 17** | KY-019 | 5V Relay Module | Load switching; the course prototype uses a low-voltage load only |
| **Load / Power** | - | Low-voltage LED/lamp + external power supply | Primary illumination target for the prototype |

---

## 🔌 Pinout & Signal Mapping

> **Team Notice**: The table below holds initial suggested pinouts marked as `[TBD]`. Final pin assignments should be confirmed by the team before configuring `.ioc` in STM32CubeMX.

| Module | Module Pin | Suggested STM32 Pin | Pin Mode | Logic Level / Signal | Agreement Status |
| :--- | :--- | :--- | :--- | :--- | :---: |
| **KY-018 (LDR)** | `S` (Signal) | `PA0` | ADC1_IN0 (Analog) | 12-bit ADC (0 - 4095) | `[TBD]` |
| | `+` (VCC) | `3V3` or `5V` | Power | VCC | `[TBD]` |
| | `-` (GND) | `GND` | Ground | 0V | `[TBD]` |
| **KY-032 (IR)** | `OUT` | `PA1` | GPIO_Input (Pull-up) | Active-LOW (`0` = detected, `1` = clear) | `[TBD]` |
| | `EN` | *Float / Jumper* | Enable | Enabled | `[TBD]` |
| | `VCC` | `5V` or `3V3` | Power | VCC | `[TBD]` |
| | `GND` | `GND` | Ground | 0V | `[TBD]` |
| **KY-019 (Relay)** | `IN` | `PA4` | GPIO_Output | Active-HIGH (`1` = ON, `0` = OFF) | `[TBD]` |
| | `VCC` | `5V` | Power | Relay coil drive supply | `[TBD]` |
| | `GND` | `GND` | Ground | Common Ground with STM32 | `[TBD]` |

---

## 🧠 System Architecture & Finite State Machine (FSM)

The system operates using a non-blocking Finite State Machine (utilizing `HAL_GetTick()` for deterministic timing):

```
                    Qualified Trigger Event
+--------------+ -------------------------> +----------------+ <---+
|  STATE_IDLE  |                            | STATE_LIGHT_ON |     | Obstacle Presence
+--------------+                            +----------------+ ----+ refresh
       ^                                             |
       | Optical settle complete                     | Hold Duration expires
       |                                             v
       +----------------------------------- +----------------------+
                                           | STATE_OPTICAL_SETTLE |
                                           +----------------------+
```

### Optical Feedback Protection (Light Lockout)
When the primary lamp is activated, light reflection can strike the LDR sensor, causing measured illuminance to surge and falsely indicating daytime. To eliminate rapid ON/OFF oscillation:
- While in `STATE_LIGHT_ON`, the system applies **Light Lockout**, suspending LDR state evaluation.
- The lamp state is governed strictly by the countdown timer (Hold Duration) and Obstacle Presence refresh.
- While Obstacle Presence remains asserted, it refreshes the Hold Duration, so timeout cannot occur until presence clears.
- Once the hold duration elapses and the relay opens (lamp turns OFF), the system enters `STATE_OPTICAL_SETTLE`; LDR sampling resumes only after the optical settle period. A later Qualified Trigger Event may then activate the lamp again.

---

## 📁 Repository Structure

```text
sensor-based-smart-light/
├── .github/
│   ├── pull_request_template.md        # Pull request template with safety checklist
│   └── workflows/
│       └── pr-check.yml                # CI workflow verifying binaries, linting, & syntax
├── .gitignore                          # Excludes build artifacts (Debug/, *.elf, *.hex)
├── AGENTS.md                           # Agent workflow and documentation pointers
├── README.md                           # System documentation and wiring guide
├── GLOSSARY.md                         # Ubiquitous domain language definitions
├── docs/
│   ├── agents/                         # Issue-tracker, labels, and domain-doc conventions
│   └── adr/
│       └── 0001-target-nucleo-f411re.md  # Architectural decision record for MCU selection
└── (STM32CubeIDE project files: .ioc, Core/, Drivers/ generated via STM32CubeMX)
```

---

## 💻 Getting Started with STM32CubeIDE

1. **Clone the repository**:
   ```bash
   git clone https://github.com/worawalan-s/sensor-based-smart-light.git
   cd sensor-based-smart-light
   ```
2. **Open / Initialize the Project**:
   - Launch **STM32CubeIDE**.
   - Select `File` -> `Open Projects from File System...` and choose this folder.
   - When generating code via STM32CubeMX: Select target MCU **NUCLEO-F411RE** (`STM32F411RETx`) and save the `.ioc` configuration in the project root.
3. **Development Guidelines**:
   - Implement custom logic exclusively inside `/* USER CODE BEGIN ... */` and `/* USER CODE END ... */` comment blocks in `Core/Src/main.c` so changes persist across CubeMX code regenerations.
   - Never commit build output directories (`Debug/`, `Release/`) or binary files.

---

## 📚 References & Decisions
- Domain Glossary: [GLOSSARY.md](GLOSSARY.md)
- Architectural Decision Records: [docs/adr/0001-target-nucleo-f411re.md](docs/adr/0001-target-nucleo-f411re.md)
- Team Work Plan: [docs/team-work-plan.md](docs/team-work-plan.md)
