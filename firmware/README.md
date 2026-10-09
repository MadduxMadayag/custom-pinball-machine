# Arduino Firmware

`Pinball_code.ino` is the original team sketch, preserved without modifications.

## Dependencies
- `Servo.h` — Arduino Servo library
- `DFRobotDFPlayerMini.h` — DFRobot DFPlayer Mini library

The firmware uses `Serial1` for the audio module and pin definitions at the start of the sketch. Choose a board with the required pins and hardware serial interface, and check those pin definitions against the wiring diagrams in `documentation/final-report.pdf` before flashing. The SD card audio files referenced by `playFolder()` are not included in this repository.

This is an archive of the working course design, **not a validated portable build configuration**. The final report records system testing; no automated compilation or hardware test is included here.
