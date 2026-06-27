# Real-Time AI System on Raspberry Pi

A modular real-time embedded application developed in C using POSIX Threads on Linux PREEMPT_RT.

## Hardware

- Raspberry Pi 3 B+
- USB Camera

## Software

- Debian Linux (PREEMPT_RT)
- POSIX Threads (pthreads)
- C Language
- FFmpeg
- Git & GitHub

## Project Goal

Develop a modular real-time system capable of:

- Periodic image acquisition
- Image preprocessing
- Neural network inference
- Actuator control

The project is developed incrementally following a modular software architecture.

## Project Structure

```
rt-linux-ai/
├── app/          # Main application
├── core/         # Time utilities and shared services
├── drivers/      # Hardware abstraction layer
├── tasks/        # Real-time tasks
├── include/      # Global configuration
├── images/       # Captured images
├── models/       # AI models
├── docs/         # Documentation
└── Makefile
```

## Sprint 1 — Periodic Image Capture

### Features

- PREEMPT_RT kernel running on Raspberry Pi 3 B+
- POSIX Thread for periodic image acquisition
- Periodic execution using `clock_nanosleep()` with `TIMER_ABSTIME`
- USB camera driver based on FFmpeg
- Circular image buffer (10 images)

### Current Architecture

```
main()
   │
   └── Capture Thread
            │
            ▼
      Camera Driver
            │
            ▼
         USB Camera
```

## Next Steps

- Implement time utility library (`time_utils`)
- Image preprocessing thread
- Shared buffer between tasks
- Neural network inference
- Actuator control thread

## License

This project was developed for academic purposes as part of a Real-Time Systems course.
