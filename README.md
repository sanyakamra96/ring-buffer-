# ring-buffer-
# CAN Ring Buffer Gateway

## Overview
This project implements a fixed-size circular (ring) buffer for handling CAN messages in a producer–consumer architecture. It simulates a simple gateway where incoming raw data is deserialized into CAN frames and passed to a consumer thread for processing.

The design demonstrates basic concurrency concepts using C++11 atomic operations and threading.

---

## Features
- Fixed-size ring buffer (1024 entries)
- Simple CAN message structure with ID, DLC, and data payload
- Producer thread: deserializes raw buffer into CAN messages
- Consumer thread: reads messages from the ring buffer and processes them
- Lock-free index management using atomic variables

---

## Architecture

### Components
- `can_message`
  - Represents a CAN frame
  - Fields:
    - `can_id` (uint32_t)
    - `dlc` (uint8_t)
    - `can_data[8]`

- `Ringbuffer`
  - Circular buffer with overwrite protection
  - Uses `write_idx` and `read_idx` for coordination

- Producer (`upd_can`)
  - Parses raw byte buffer
  - Converts it into `can_message`
  - Pushes into ring buffer

- Consumer (`send_can`)
  - Continuously reads from ring buffer
  - Prints received CAN IDs

---

## Build Instructions

### Requirements
- C++11 or later
- g++ or clang++
- pthread support

### Compile
```bash
g++ -std=c++17 -pthread ring_buffer.cpp -o gateway
