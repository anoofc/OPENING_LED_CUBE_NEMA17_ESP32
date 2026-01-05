
# OPENING LED Cube NEMA17 ESP32

A Bluetooth-controlled stepper motor system for an LED cube opening mechanism using ESP32 and NEMA17 motors with acceleration/deceleration ramp support.

## Overview

This project controls four NEMA17 stepper motors via an ESP32 microcontroller. The system features:
- Bluetooth Serial communication for remote control
- Configurable step counts per motor
- Acceleration/deceleration ramping for smooth motion
- Homing and reset sequences
- Optional auto-reset timer
- Persistent configuration storage using NVS (Non-Volatile Storage)
- Limit switch support for home position detection

## Hardware Configuration

### GPIO Pin Assignments

| Component | Pin | Purpose |
|-----------|-----|---------|
| TRIGGER | 34 | Launch trigger input |
| RESET | 35 | Reset trigger input |
| LIMIT_1 | 32 | Motor 1 home limit switch |
| LIMIT_2 | 33 | Motor 2 home limit switch |
| LIMIT_3 | 25 | Motor 3 home limit switch |
| LIMIT_4 | 26 | Motor 4 home limit switch |
| MOTOR_1_DIR | 27 | Motor 1 direction control |
| MOTOR_1_PUL | 14 | Motor 1 pulse control |
| MOTOR_2_DIR | 23 | Motor 2 direction control |
| MOTOR_2_PUL | 22 | Motor 2 pulse control |
| MOTOR_3_DIR | 21 | Motor 3 direction control |
| MOTOR_3_PUL | 19 | Motor 3 pulse control |
| MOTOR_4_DIR | 18 | Motor 4 direction control |
| MOTOR_4_PUL | 4 | Motor 4 pulse control |

## Motion Control

### Ramping Configuration

- **START_DELAY**: 600 µs (slowest speed)
- **TARGET_DELAY**: 100 µs (fastest speed)
- **RAMP_STEPS**: 2000 steps (acceleration/deceleration duration)

The system uses linear acceleration during the first RAMP_STEPS and deceleration during the final RAMP_STEPS for smooth motor operation.

## Communication Protocols

### Bluetooth Commands

Send via Bluetooth Serial (BT_NAME: "OPENING_Cube"):

| Command | Function |
|---------|----------|
| `SET_STEP1 <steps>` | Set motor 1 step count |
| `SET_STEP2 <steps>` | Set motor 2 step count |
| `SET_STEP3 <steps>` | Set motor 3 step count |
| `SET_STEP4 <steps>` | Set motor 4 step count |
| `SET_ACCEL <steps>` | Set acceleration ramp steps |
| `SET_TIME <seconds>` | Set auto-reset timer (seconds) |
| `GET_CONFIG` | Retrieve and display current config |
| `L` | Launch sequence |
| `R` | Reset to home position |

### Serial Commands (USB)

- `l` - Launch sequence
- `r` - Reset to home

## Key Functions

### `moveStepperWithRamp(pulsePin, dirPin, totalSteps, direction)`
Moves a stepper motor with acceleration/deceleration ramping. Maps step position to delay values for smooth acceleration and deceleration phases.

### `launchSequence()`
Executes all four motors in sequence with ramped motion away from home position.

### `resetLaunch()`
Returns all motors to home position with ramped motion.

### `homingSequence()`
Moves all motors toward home until their respective limit switches are triggered.

### `processData(String data)`
Parses and executes Bluetooth commands.

## State Management

| Variable | Purpose |
|----------|---------|
| `launchSuccess` | Tracks if launch sequence completed |
| `reset` | Indicates motors are at home position |
| `stepper[1-4]_steps` | Configurable steps per motor |
| `RAMP_STEPS` | Acceleration profile duration |
| `resetTime` | Auto-reset timer duration (seconds) |

## Configuration Storage

Settings are persisted to ESP32 flash memory using Preferences:
- Namespace: "CUBE"
- Keys: `step1`, `step2`, `step3`, `step4`, `accel`, `time`
- Defaults: 10000 steps per motor, 2000 ramp steps, 0 seconds for auto-reset

## Debug Mode

Set `#define DEBUG 1` to enable serial debug messages.
