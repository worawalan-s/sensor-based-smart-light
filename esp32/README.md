# STM32 (NUCLEO-F411RE) ↔ ESP32-S3-N16R8 UART Smoke Test Guide

Step-by-step instructions to flash, wire, and verify bidirectional UART communication between `NUCLEO-F411RE` and `ESP32-S3-N16R8`.

---

## 1. Fetch the Test Branch

```bash
git fetch origin
git checkout feat/esp32-uart-test
```

---

## 2. Flash `NUCLEO-F411RE` (STM32CubeIDE)

1. Connect the `NUCLEO-F411RE` board via USB (ST-LINK).
2. Open **STM32CubeIDE** → `File` → `Open Projects from File System...` → select the `sensor-based-smart-light` folder → click **Finish**.
3. Click **Build** (hammer icon), then **Run** (green play button) to flash the MCU.
4. Verify that the green **`LD2`** LED on the Nucleo board blinks every 200 ms (confirms the non-blocking loop is running).

---

## 3. Flash `ESP32-S3-N16R8` (Arduino IDE)

1. Open `esp32/esp32_uart_bridge_test/esp32_uart_bridge_test.ino` in **Arduino IDE**.
2. *(First-time setup only)* Go to `Settings` → **Additional boards manager URLs** and add:
   ```text
   https://espressif.github.io/arduino-esp32/package_esp32_index.json
   ```
   Then open **Boards Manager** (left sidebar), search for `esp32`, and install **esp32 by Espressif Systems**.
3. Connect the `ESP32-S3-N16R8` via USB and configure **Tools**:
   - **Board:** `ESP32S3 Dev Module`
   - **Flash Size:** `16MB (128Mb)`
   - **PSRAM:** `OPI PSRAM`
   - **USB CDC On Boot:** `Enabled` *(if using the native `USB` port)* or `Disabled` *(if using the `UART`/`COM` port)*
   - **Port:** Select your ESP32-S3 serial port.
4. Click **Upload (`→`)** and wait for `Done uploading`.

---

## 4. Wire the Boards Together

> [!WARNING]
> - Unplug both USB cables before wiring.
> - **Do NOT** connect `3V3` or `5V` between the two boards; power each board via its own USB cable and share **GND** only.
> - On `ESP32-S3-N16R8`, use `GPIO 16` and `GPIO 17` only (avoid `GPIO 35–37`, which are reserved for Octal PSRAM).

| `NUCLEO-F411RE` | Connection | `ESP32-S3-N16R8` |
| :--- | :---: | :--- |
| `GND` | ────── | `GND` |
| `D8` (`PA9` / `USART1_TX`) | ──────▶ | `GPIO 16` (`Serial1 RX`) |
| `D2` (`PA10` / `USART1_RX`) | ◀────── | `GPIO 17` (`Serial1 TX`) |

---

## 5. Observe Results & Pass Criteria

Reconnect both USB cables and open **two Serial Monitor windows** at **`115200 baud`**:
- **Window 1 (ESP32-S3 port):** Arduino IDE → **Serial Monitor** (top-right magnifying glass icon)
- **Window 2 (STM32 ST-LINK VCP port):** Any serial terminal (PuTTY, Tera Term, `screen`, etc.)

### Expected Behavior
1. **Periodic `PING` / `PONG` & Dummy Telemetry (every 1 s):**
   - **ESP32-S3 Serial Monitor** prints:
     ```text
     [From STM32] PING:1
     [From STM32] PING:2
     [From STM32] STATE:LIGHT_ON,LDR:1200
     ```
   - **STM32 Serial Monitor** prints matching replies:
     ```text
     [From ESP32] PONG:1
     [From ESP32] PONG:2
     ```
2. **Remote Override Command:**
   - Type `CMD:OVERRIDE_ON` in the **ESP32-S3 Serial Monitor** input box and press Enter.
   - **STM32** prints `[From ESP32] CMD:OVERRIDE_ON`.
   - **ESP32-S3** receives `[From STM32] ACK:OVERRIDE_ON`.
3. **Non-blocking & Fault Recovery Check:**
   - Unplug `D2 (PA10)` and plug it back in: `LD2` on the Nucleo must keep blinking steadily without freezing, and `PONG` replies must resume automatically after reconnecting.

---

## 6. Test Results (Verified)

- **Date:** 2026-11-12
- **Hardware:** `NUCLEO-F411RE` ↔ `ESP32-S3-N16R8` (`USART1 PA9/PA10` ↔ `Serial1 GPIO 16/17` @ `115200 baud`, Common `GND`, separate USB power)
- **Status:** **PASSED**
  - `NUCLEO-F411RE` boots cleanly, blinks `LD2` every 200 ms without blocking, and outputs `STM32F411 UART Smoke Test Start` on `USART2` ST-LINK VCP.
  - Bidirectional UART communication (`PING` / `PONG`, dummy telemetry `STATE:LIGHT_ON,LDR:1200`, and `CMD:OVERRIDE_ON` / `ACK:OVERRIDE_ON`) verified across both Serial Monitors.
- **Setup Notes Confirmed During Testing:**
  - On `ESP32-S3-N16R8`, use `GPIO 16` / `GPIO 17` (not the board's `TX`/`RX` `UART0` pins) and set `USB CDC On Boot: Enabled` in Arduino IDE when using the native `USB` port.
  - Ensure `Core/Startup/startup_stm32f411retx.s` is tracked in Git so the STM32 project links with a valid vector table on any machine.
