# Copilot Instructions for AHT20_Measure

- This is an 8051 embedded firmware project targeting `AT89C52` with the Keil MCS-51 toolchain.
- The build is driven by a Keil uVision project `AHT20_Measure.uvproj`; VS Code tasks use the EIDE extension commands in `.vscode/tasks.json`.
- Key source files:
  - `main.c` — application entry point, currently only an empty `while(1)` loop.
  - `PLATFORM.c` / `PLATFORM.h` — hardware abstraction and low-level GPIO/I2C/UART primitives.
  - `UART.c` / `UART.h` — UART module wrapper exposing `Uart_Init`, `Uart_Send_Str`, `Uart_Receive`, `Uart_Clear`.
  - `BEEP.c` / `BEEP.h` — buzzer module wrapper exposing `Beep_Init`, `Beep_SoundOnce`.
  - `AHT20.c` / `AHT20.h` — currently empty stubs, intended for sensor logic.
  - `STARTUP.A51` — 8051 startup assembly.

- Important architecture notes:
  - `PLATFORM.*` is the boundary between MCU-specific hardware and higher-level modules.
  - `UART` and `BEEP` use function-pointer structs (`UART_TypeDef`, `BEEP_TypeDef`) and then expose plain API wrappers.
  - `main.c` currently does not implement business logic, so do not assume the sensor flow is complete.
  - I2C state-machine enums in `PLATFORM.h` are extensive, but most of the AHT20 I2C task is scaffolded and not fully implemented.

- Coding patterns to follow:
  - Use `Pin_ID_TypeDef` and `GPIO_Set(PIN_..., state)` for pin control.
  - Keep 8051-specific constructs like `sbit`, `bit`, and `interrupt N` consistent.
  - Prefer the existing naming style: `Platform_*` for low-level drivers, `Uart_*` and `Beep_*` for module APIs.

- Known source inconsistencies:
  - `PLATFORM.h` declares `Platform_Uart_Init`, but `PLATFORM.c` defines `Platform_UART_Init`.
  - `PLATFORM.c` has an incomplete `delay_us` implementation and a partially empty `Platform_Aht20_Standard_IIC_Read_Task` state machine.

- Build / debug workflow:
  - Use VS Code task `build` to compile.
  - Use `build and flash` to compile and upload to device if the EIDE extension is configured.
  - Use `clean` or `rebuild` to reset build artifacts.
  - The project uses `reg52.h` and Keil startup code, so maintain 8051 compiler semantics.

- When editing:
  - Do not add new source files without updating the uVision project if the code must build through Keil.
  - Keep changes localized to platform abstractions when possible, since `UART` and `BEEP` rely on `PLATFORM` for hardware access.

If any section is unclear or you want the guide to emphasize a specific workflow, tell me and I can refine it.