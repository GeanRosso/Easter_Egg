BIO-LOCK v2: 3-Tier Embedded Security System
This project is a full-stack IoT security solution that bridges physical hardware with a modern web interface. It implements a complete data pipeline spanning from low-level C programming on an AVR microcontroller to a high-level React frontend, connected by an ASP.NET Core middleware.

System Overview
The BIO-LOCK v2 utilizes an ultrasonic sensor to monitor user proximity. As a target enters the detection range, a "Snake" LED animation is triggered across both the physical breadboard and the web UI. Access is managed through a secure identity PIN, featuring a synchronized hardware and software self-destruct sequence if unauthorized access is attempted three times.

3-Tier Architecture
1. Perception Layer (Hardware)
Language: C (AVR)

Microcontroller: ATmega2560

Components: HC-SR04 Ultrasonic Sensor, I2C 16x2 LCD, 6x LED Array

Description: This layer manages real-time distance measurements and local state transitions. It broadcasts system telemetry as a JSON-formatted string via UART (Serial) and listens for single-byte control characters to update the local display and LED states.

2. Middleware Layer (Gateway)
Language: C# (ASP.NET Core WebAPI)

Description: The middleware serves as the essential bridge between the Serial Port and the Web. A background service monitors the COM port, deserializes incoming telemetry into Data Transfer Objects (DTOs), and exposes RESTful endpoints. It handles the core security logic, including password validation and command routing to the hardware.

3. Application Layer (Frontend)
Language: React (Vite) / CSS3

Description: The frontend provides a state-driven command center. It employs asynchronous polling to fetch hardware updates every 200ms, ensuring the digital "Snake" animation remains synchronized with physical sensor data. The UI dynamically adapts based on system state, transitioning from an idle mode to an active authorization prompt or a security lockout screen.

Project Structure
Plaintext
1- Hardware_AVR/          # C drivers for UART, I2C, LCD, and Sensors
2- WebAPI_DotNet/         # .NET Service Layer, Serial Bridge, and Controllers
3- Frontend_React/        # React components, State management, and Styles
Technical Features
Bidirectional Data Pipeline: Real-time sensor telemetry flows upstream to the UI, while authenticated control commands flow downstream to the hardware.

Synchronization Protocol: The system utilizes a shared state machine logic, ensuring that physical LCD prompts and digital UI elements transition simultaneously.

Security Lockout Policy: Implements a three-strike failure threshold. Upon the third failed attempt, the system triggers a "Panic" state, initiating a coordinated hardware alarm and a web-based system termination overlay.

Optimized Serial Communication: Uses a lightweight single-byte command protocol for downstream instructions to minimize latency and hardware overhead.

Installation and Deployment
Hardware: Compile and flash the C source code to the ATmega2560. Verify the wiring for the I2C LCD and the Port A LED array.

Middleware: Navigate to the WebAPI directory and execute dotnet run. Confirm that the COM port configuration in the Hardware Service matches the device connection.

Frontend: Navigate to the Frontend directory, execute npm install, followed by npm run dev.
