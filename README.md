# DE10-Standard HPS-FPGA Control System

A dual-layered System-on-Chip (SoC) project that demonstrates high-level C software controlling FPGA hardware peripherals. This project utilizes the **ARM Cortex-A9** processor on the DE10-Standard to drive FPGA-side LEDs, switches, and 7-segment displays via the **Lightweight HPS-to-FPGA AXI Bridge**.

## 🚀 Overview

The project consists of two main components:
1.  **Hardware (Verilog):** A top-level design that instantiates the HPS component and exports FPGA peripherals (LEDs, HEX displays, Switches) to the HPS address space.
2.  **Software (C):** Linux-based applications that map the physical hardware addresses into virtual memory space, allowing direct software control of FPGA logic.

### Key Features
* **Memory-Mapped I/O:** Accessing FPGA hardware registers via `/dev/mem` and `mmap()`.
* **Dynamic 7-Segment Control:** Software-driven hex encoding for scrolling text ("intel soc") and numerical timers.
* **Real-Time Interaction:** Interrupt-safe (SIGINT) handling to ensure hardware resets gracefully on exit.
* **Synchronized Peripherals:** Concurrent management of 10x LEDs, 10x Switches, and 6x 7-Segment displays.

---

## 🛠 Project Components

### 1. Hardware Implementation (`lab2EM.v`)
The Verilog top-level integrates the HPS system generated via Platform Designer (Qsys).
* **Clocking:** Driven by a 50MHz oscillator.
* **Bridges:** Utilizes the HPS DDR3 controller and the `h2f_lw_axi_master` for peripheral communication.
* **Exports:** Custom PIO (Parallel I/O) ports for `hex03`, `hex45`, `led`, and `switches`.

### 2. Software Applications
* **Text Scroller (`lab2p1.c`):** Implements a state-machine based "intel soc" scrolling message across the HEX displays using custom 7-segment bitmasks.
* **Digital Timer/Counter (`lab2p3.c`):** A sophisticated timer controlled by the onboard Push Buttons (`KEY`). It allows setting and clearing specific numerical values for seconds, minutes, and days.

---

## 📂 Repository Structure

* `lab2EM.v`: Verilog top-level module for the DE10-Standard.
* `lab2p1.c`: C source for the HEX display string scroller.
* `lab2p3.c`: C source for the interactive hardware timer.
* `hps_0.h`: Header file containing the base addresses and offsets for the FPGA peripherals.

---

## ⚙️ Technical Specifications

* **Bridge Address:** `0xff200000` (Lightweight HPS-to-FPGA Bridge).
* **Operating System:** Embedded Linux (e.g., Terasic SoC-FPGA Desktop).
* **Compiler:** `arm-linux-gnueabihf-gcc` (Cross-compiler).

---

## 🛠 Setup & Usage

### Hardware Synthesis
1. Open the project in **Quartus Prime**.
2. Generate the HPS system in **Platform Designer**.
3. Compile the design and program the `.sof` file to the DE10-Standard.

### Software Compilation
To compile the C code on the HPS or via cross-compiler:
```bash
gcc lab2p3.c -o timer_app
