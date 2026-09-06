# STM32-PVT-Sensor-Calibration
# VLSI Post-Silicon Validation: PVT Sensor Calibration

Firmware for synchronized test vector generation and post-silicon validation of PVT sensors.

## Overview
This repository contains the firmware and simulation artifacts for a microcontroller-based hardware validation system. It was designed to generate synchronized test vectors for calibrating on-chip Process, Voltage, and Temperature (PVT) sensors during the post-silicon validation phase of VLSI physical design.

## Hardware Architecture
The system is modeled using the **Wokwi** simulation environment and utilizes the **STM32C031C6 Nucleo** board as the primary hardware controller.

**Pin Mapping:**
*   `PA5` (D13): SPI Clock (CLK)
*   `PA6` (D12): SPI Master In, Slave Out (MISO) - *Simulated Telemetry*
*   `PA7` (D11): SPI Master Out, Slave In (MOSI)
*   `PA1` (A1): Pulse Width Modulation (PWM) Actuation Wrapper

## Firmware Design
Due to hardware timer limitations in the simulation environment, the firmware utilizes a precise, microsecond-delayed "bit-banging" methodology in bare-metal C. This guarantees the 16-bit SPI payload (`0x4000`) and the surrounding PWM voltage framing are generated with accurate hardware timing.

Two firmware variants are included:
1.  **`waveform_generator.c`**: The primary executable that generates the synchronized hardware logic traces across all four channels.
2.  **`serial_monitor_debug.c`**: A diagnostic script featuring a custom Verilog-style `$monitor` function to output a real-time, text-based logic trace over UART.

## Validation & Results
The system's logic was successfully verified using a simulated logic analyzer (PulseView). The logic traces mathematically prove the correct generation of the 16-bit test payload bounded by the active PWM control signal.

*(See the attached PDF report for full waveform screenshots and serial communication logs).*
