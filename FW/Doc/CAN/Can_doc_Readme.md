# CAN PDM – CAN Bus Message Reference

Reference for every CAN message received and transmitted by the PDM (Power Distribution Module), including byte layouts and the formulas needed to convert raw hexadecimal data into units (Amperes, Volts, °C).

| Bitrate | Cycle time | Frame length | Byte order | Units |
|:---:|:---:|:---:|:---:|:---:|
| **500 kbit/s** | **100 ms** | **DLC 8** | **Little-endian** | °C · V · A |

## Table of contents

1. [Overview](#1-overview)
2. [How to decode a value](#2-how-to-decode-a-value)
3. [Message list and IDs](#3-message-list-and-ids)
4. [Enumerations](#4-enumerations)
5. [Received messages](#5-received-messages)
6. [Transmitted messages](#6-transmitted-messages)
7. [Blink Marine keypad](#7-blink-marine-keypad)


---

## 1. Overview

The PDM drives nine Profet output channels, grouped by current capability:

| Group | Channels |
|---|---|
| High | H1, H2, H3 |
| Medium | M1, M2 |
| Low | L1, L2, L3, L4 |

Outputs can be switched:

- by a **Blink Marine keypad** (extended CAN frames), or
- by the **`0x200` command frame** (standard CAN frame) Only for bench-testing.

The PDM periodically transmits its own status, per-output currents and fault codes.

**Multiple boards on one bus.** `PDM_1` and `PDM_2` are not different hardware: they are two identical boards installed in the same vehicle. `PDM_ID` is set at compile time to give each board its own identity (CAN address and status IDs), so both can share one bus. The command frame `0x200` uses the same ID on both boards [To Be deleted].

**Legend used in the byte tables**

| Symbol | Meaning |
|:---:|---|
| ⬜ | Counter / heartbeat |
| 🟧 | State or flags |
| 🟩 | Current (A) |
| 🟪 | Voltage (V) |
| 🟥 | Temperature (°C) |
| 🟦 | Output number or key number |
| ⬛ | Reserved / unused |

---

## 2. How to decode a value

16-bit values are sent **LSB first**:

```
value = LSB + (MSB × 256)
```

Then apply the scale factor of the field:

| Quantity | Formula | Resolution |
|---|---|---|
| Current | `value / 10` → A | 0.1 A |
| Battery voltage | `value / 100` → V | 0.01 V |
| Temperature | `value / 100` → °C | 0.01 °C |

Example: bytes `64 00` → `0x0064` = 100 → 100 / 10 = **10.0 A**.


---

## 3. Message list and IDs

Each board takes its identity (`PDM_1` or `PDM_2`) from `PDM_ID`. CAN addresses used for the extended frames:

| Device | CAN address |
|---|:---:|
| Keypad (`KEYPAD_CAN_ADDRESS`) | `0x21` |
| PDM_1 (`PDM_CAN_ADDRESS`) | `0x30` |
| PDM_2 (`PDM_CAN_ADDRESS`) | `0x31` |

### Standard frames (11-bit ID)

| Message | Direction | PDM_1 | PDM_2 | Section |
|---|:---:|:---:|:---:|:---:|
| Output command | RX | `0x200` | `0x200` | [5.1](#51-output-command--0x200) |
| PDM status | TX | `0x300` | `0x400` | [6.1](#61-pdm-status--0x300--0x400) |
| High outputs fault types (H1–H3) | TX | `0x301` | `0x401` | [6.3](#63-fault-types) |
| Medium outputs fault types (M1–M2) | TX | `0x302` | `0x402` | [6.3](#63-fault-types) |
| Low 1 outputs fault types (L1–L2) | TX | `0x303` | `0x403` | [6.3](#63-fault-types) |
| Low 2 outputs fault types (L3–L4) | TX | `0x304` | `0x404` | [6.3](#63-fault-types) |
| High outputs status and currents | TX | `0x310` | `0x410` | [6.2](#62-output-status-and-currents) |
| Medium outputs status and currents | TX | `0x311` | `0x411` | [6.2](#62-output-status-and-currents) |
| Low 1 outputs status and currents | TX | `0x312` | `0x412` | [6.2](#62-output-status-and-currents) |
| Low 2 outputs status and currents | TX | `0x313` | `0x413` | [6.2](#62-output-status-and-currents) |

### Extended frames (29-bit ID, keypad)

| Message | Direction | PDM_1 | PDM_2 | Section |
|---|:---:|:---:|:---:|:---:|
| Key press from keypad | RX | `0x18EFFF21` | `0x18EFFF21` | [7.1](#71-key-press-keypad--pdm) |
| LED command to keypad | TX | `0x18EF2130` | `0x18EF2131` | [7.3](#73-led-command-pdm--keypad) |
| Brightness command to keypad | TX | `0x18EF2130` | `0x18EF2131` | [7.4](#74-brightness-command-pdm--keypad) |

Extended ID format: `0x18EF` + destination address + source address.

> Received frames reach **both** PDMs at the same time. Each PDM interprets them with its own rules: for the keypad, with its own key-to-output mapping ([7.2](#72-key-to-output-mapping)).

---

## 4. Enumerations

### 4.1 Output number (`PDM_Output_t`)

| Value | Output | Group |
|:---:|:---:|---|
| 1 | H1 | High |
| 2 | H2 | High |
| 3 | H3 | High |
| 4 | M1 | Medium |
| 5 | M2 | Medium |
| 6 | L1 | Low |
| 7 | L2 | Low |
| 8 | L3 | Low |
| 9 | L4 | Low |

### 4.2 PDM state (`PDM_State_t`)

| Value | State |
|:---:|---|
| 0 | `PDM_STATE_INIT` |
| 1 | `PDM_STATE_RUN` |
| 2 | `PDM_STATE_FAULT` |
| 3 | `PDM_STATE_OVERTEMP` |
| 4 | `PDM_STATE_UNDERVOLTAGE` |
| 5 | `PDM_STATE_SLEEP` |

### 4.3 Fault type (`ProfetFault_t`)

| Value | Fault | Keypad LED |
|:---:|---|---|
| 0 | `FAULT_NONE` | Follows output state |
| 1 | `FAULT_OVERCURRENT` | Amber/orange, blinking |
| 2 | `FAULT_SHORT_CIRCUIT` | Red, blinking |
| 3 | `FAULT_OPEN_LOAD` | Yellow, blinking |
| 4 | `FAULT_THERMAL` | Cyan, blinking |
| 5 | `FAULT_UNDERVOLTAGE` | No dedicated LED rule |

### 4.4 Keypad LED colors (`KeypadLedColor_t`)

| Value | Color |
|:---:|---|
| `0x00` | Off |
| `0x01` | Red |
| `0x02` | Green |
| `0x03` | Blue |
| `0x04` | Yellow |
| `0x05` | Magenta |
| `0x06` | Cyan |
| `0x07` | White |
| `0x08` | Amber / orange |

### 4.5 Keypad LED modes (`KeypadLedMode_t`)

| Value | Mode |
|:---:|---|
| `0x00` | Off |
| `0x01` | On (steady) |
| `0x02` | Blinking |

---

## 5. Received messages

### 5.1 Output command – `0x200`

Standard frame. Requests an output to be switched on or off.

| Byte 0 | Byte 1 | Byte 2 | Byte 3 | Byte 4 | Byte 5 | Byte 6 | Byte 7 |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 🟦 Output | 🟧 State | ⬛ | ⬛ | ⬛ | ⬛ | ⬛ | ⬛ |

| Field | Values |
|---|---|
| Output | 1–9, see [4.1](#41-output-number-pdm_output_t) |
| State | `0` = off, any non-zero value = on |

The PDM does not switch the output directly: it sets the channel's requested state, and the protection logic decides whether to enable it.

Examples:

| Frame data | Effect |
|---|---|
| `01 01 00 00 00 00 00 00` | Turn H1 on |
| `06 00 00 00 00 00 00 00` | Turn L1 off |

> In the firmware, PDM_1 and PDM_2 handle this frame identically (same output numbering, no PDM address check).

The keypad frame `0x18EFFF21` is described in [7.1](#71-key-press-keypad--pdm).

---

## 6. Transmitted messages

All messages below are sent every **100 ms**, DLC 8, little-endian.

### 6.1 PDM status – `0x300` / `0x400`

| Byte 0 | Byte 1 | Byte 2 | Byte 3 | Byte 4 | Byte 5 | Byte 6 | Byte 7 |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| ⬜ Alive | 🟧 State | 🟩 Current LSB | 🟩 Current MSB | 🟪 Voltage LSB | 🟪 Voltage MSB | 🟥 Temp LSB | 🟥 Temp MSB |

| Bytes | Field | Type | Conversion |
|:---:|---|---|---|
| 0 | Alive heartbeat | `uint8` | Counter, increments each message (0–255, then wraps) |
| 1 | PDM state | `uint8` | See [4.2](#42-pdm-state-pdm_state_t) |
| 2–3 | Total current | `uint16` | `value / 10` → **A** |
| 4–5 | Battery voltage | `uint16` | `value / 100` → **V** |
| 6–7 | PDM temperature | `uint16` | `value / 100` → **°C** |

**Example:** `2A 01 64 00 14 05 C4 09`

| Field | Raw | Result |
|---|:---:|---|
| Heartbeat | `0x2A` | 42 |
| State | `0x01` | RUN |
| Total current | `0x0064` = 100 | **10.0 A** |
| Battery voltage | `0x0514` = 1300 | **13.00 V** |
| Temperature | `0x09C4` = 2500 | **25.00 °C** |


### 6.2 Output status and currents

One message per output group. Byte 0 holds status bits, followed by one 16-bit current per output (`value / 10` → **A**).

#### High outputs – `0x310` / `0x410`

| Byte 0 | Byte 1 | Byte 2 | Byte 3 | Byte 4 | Byte 5 | Byte 6 | Byte 7 |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 🟧 Flags | 🟩 H1 LSB | 🟩 H1 MSB | 🟩 H2 LSB | 🟩 H2 MSB | 🟩 H3 LSB | 🟩 H3 MSB | ⬛ |

| Bit | Meaning |
|:---:|---|
| 0 | H1 enabled |
| 1 | H2 enabled |
| 2 | H3 enabled |
| 3 | H1 fault |
| 4 | H2 fault |
| 5 | H3 fault |
| 6–7 | Unused |

#### Medium outputs – `0x311` / `0x411`

| Byte 0 | Byte 1 | Byte 2 | Byte 3 | Byte 4 | Byte 5 | Byte 6 | Byte 7 |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 🟧 Flags | 🟩 M1 LSB | 🟩 M1 MSB | 🟩 M2 LSB | 🟩 M2 MSB | ⬛ | ⬛ | ⬛ |

| Bit | Meaning |
|:---:|---|
| 0 | M1 enabled |
| 1 | M2 enabled |
| 2 | M1 fault |
| 3 | M2 fault |
| 4–7 | Unused |

#### Low 1 outputs – `0x312` / `0x412`

| Byte 0 | Byte 1 | Byte 2 | Byte 3 | Byte 4 | Byte 5 | Byte 6 | Byte 7 |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 🟧 Flags | 🟩 L1 LSB | 🟩 L1 MSB | 🟩 L2 LSB | 🟩 L2 MSB | ⬛ | ⬛ | ⬛ |

| Bit | Meaning |
|:---:|---|
| 0 | L1 enabled |
| 1 | L2 enabled |
| 2 | L1 fault |
| 3 | L2 fault |
| 4–7 | Unused |

#### Low 2 outputs – `0x313` / `0x413`

| Byte 0 | Byte 1 | Byte 2 | Byte 3 | Byte 4 | Byte 5 | Byte 6 | Byte 7 |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 🟧 Flags | 🟩 L3 LSB | 🟩 L3 MSB | 🟩 L4 LSB | 🟩 L4 MSB | ⬛ | ⬛ | ⬛ |

| Bit | Meaning |
|:---:|---|
| 0 | L3 enabled |
| 1 | L4 enabled |
| 2 | L3 fault |
| 3 | L4 fault |
| 4–7 | Unused |

The **fault bit** is set when the channel's fault type is anything other than `FAULT_NONE`. The exact fault is reported in the fault messages ([6.3](#63-fault-types)).

**Example** – ID `0x310`, data `03 2C 01 00 00 00 00 00`:

| Field | Raw | Result |
|---|:---:|---|
| Flags | `0x03` | H1 and H2 enabled, no fault |
| H1 current | `0x012C` = 300 | **30.0 A** |
| H2 current | `0x0000` | **0.0 A** |
| H3 current | `0x0000` | **0.0 A** |

### 6.3 Fault types

One byte per output, containing the fault code from [4.3](#43-fault-type-profetfault_t). Unused bytes are `0`.

| ID (PDM_1) | ID (PDM_2) | Byte 0 | Byte 1 | Byte 2 | Bytes 3–7 |
|:---:|:---:|:---:|:---:|:---:|:---:|
| `0x301` | `0x401` | H1 | H2 | H3 | 0 |
| `0x302` | `0x402` | M1 | M2 | 0 | 0 |
| `0x303` | `0x403` | L1 | L2 | 0 | 0 |
| `0x304` | `0x404` | L3 | L4 | 0 | 0 |

**Example** – ID `0x301`, data `00 03 00 00 00 00 00 00`: H2 reports `FAULT_OPEN_LOAD`; H1 and H3 have no fault.

---

## 7. Blink Marine keypad

The keypad uses J1939-style extended frames (`0x18EF` + destination + source), addresses listed in [section 3](#3-message-list-and-ids).

### 7.1 Key press (keypad → PDM)

**ID `0x18EFFF21`** – destination `0xFF` (broadcast), source `0x21` (keypad). Received by both PDMs.

| Byte 0 | Byte 1 | Byte 2 | Byte 3 | Byte 4 | Byte 5 | Byte 6 | Byte 7 |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| ⬛ | ⬛ | ⬛ | 🟦 Key number | 🟧 Pressed | ⬛ | ⬛ | ⬛ |

| Field | Values |
|---|---|
| Key number | Key ID, see mapping below |
| Pressed | `1` = pressed, `0` = released |

Behavior on a **press edge**:

- If the associated output is in fault → the fault is reset and the output is requested **on**.
- Otherwise → the output **toggles** (on ↔ off).

### 7.2 Key-to-output mapping

Each PDM uses its own mapping. `30` is a placeholder meaning "no key assigned".

| Output | PDM_1 key | PDM_2 key |
|:---:|:---:|:---:|
| H1 | 8 | 6 |
| H2 | 14 | 7 |
| H3 | 10 | 30 (unassigned) |
| M1 | 3 | 13 |
| M2 | 2 | 30 (unassigned) |
| L1 | 1 | 1 |
| L2 | 30 (unassigned) | 5 |
| L3 | 30 (unassigned) | 30 (unassigned) |
| L4 | 30 (unassigned) | 30 (unassigned) |

> ⚠️ If the keypad has a real key numbered 30, pressing it would toggle **all** outputs mapped to 30 at the same time.

### 7.3 LED command (PDM → keypad)

**ID** `0x18EF0000 | (KEYPAD_CAN_ADDRESS << 8) | PDM_CAN_ADDRESS`

| PDM | ID |
|:---:|:---:|
| PDM_1 | `0x18EF2130` |
| PDM_2 | `0x18EF2131` |

| Byte 0 | Byte 1 | Byte 2 | Byte 3 | Byte 4 | Byte 5 | Byte 6 | Byte 7 |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| `0x04` | `0x1B` | `0x01` | 🟦 Key number | 🟩 Color | 🟪 Mode | `0xFF` | `0xFF` |

Colors: see [4.4](#44-keypad-led-colors-keypadledcolor_t). Modes: see [4.5](#45-keypad-led-modes-keypadledmode_t).

LED behavior, in priority order:

| Condition | Color | Mode |
|---|---|---|
| `FAULT_OPEN_LOAD` | Yellow | Blinking |
| `FAULT_OVERCURRENT` | Amber / orange | Blinking |
| `FAULT_SHORT_CIRCUIT` | Red | Blinking |
| `FAULT_THERMAL` | Cyan | Blinking |
| Output enabled | Green | On |
| Output disabled | Green | Off |

The command is sent when the LED state or mode changes, and **re-sent every 100 ms** otherwise, so the keypad recovers if a frame is lost.

### 7.4 Brightness command (PDM → keypad)

Same ID as the LED command.

| Byte 0 | Byte 1 | Byte 2 | Byte 3 | Byte 4 | Byte 5 | Byte 6 | Byte 7 |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| `0x04` | `0x1B` | `0x02` | 🟧 Brightness | `0xFF` | `0xFF` | `0xFF` | `0xFF` |

Brightness byte = `percent × 63 / 100` (0 % → `0x00`, 100 % → `0x3F`). The PDM sets 100 % at startup.

---

