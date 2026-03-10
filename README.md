# Arduino-Bluetooth-SMS-Messenger
This project implements a functional messaging device on the Arduino Uno. It uses "Multi-Tap" text entry method used on early mobile phones and communicates over Bluetooth (HC-05) using a standard 4x4 matrix keypad and a 16x2 LCD.
### Technical Features
- **Legacy Multi-Tap Entry:** Implemented a custom time-multiplexed algorithm where holding a numeric key cycles through associated letters (e.g., Key '2' cycles '2', 'A', 'B','C') at 1-second intervals.
- **Asynchronous Bidirectional Messaging:** Utilized `SoftwareSerial` to manage background Bluetooth data reception without interrupting the user's active typing.
- **Priority State Machine:** Added a state-driven UI that prioritizes "Incoming Message" alerts which locks the keypad and displays the received text for exactly 10 seconds before auto-reverting to the composer.
- **Buffer Management:** Managed a dynamic `String` buffer for message composition, with triggers for "Send" (#) and "Clear" (*).
### Hardware
- **Microcontroller:** Arduino Uno (R3 or R4)
- **Communication:** HC-05 Bluetooth Module (set at 9600 Baud)
- **Input:** 4x4 Membrane Keypad
- **Output:** 16x2 LCD Display
- **Additional Hardware:** Male-To-Male Jumper Wires, Breadboard, Data Transfer USB-C cable (For Arduino UNO R4 if using UNO R3 use Data Transfer USB-B cable)
### Software
- **Programming:** Arduino IDE
- **Sending Test Messages:** CoolTerm.exe (used for debugging Bluetooth transmission and sending test messages)
### Challenge & Solution
The main problem was being able to handle three things at once which was detecting key "holds", updating the LCD display, and listening for incoming Bluetooth data. The obstacle that I faced was that standard Arduino code runs linearly so using a function like `delay()` to time the 1-second cycles would stop the Bluetooth radio from accepting incoming messages. To overcome this issue, I implemented a non-blocking timing using the `millis()` function. By comparing timestamps, the code cycles characters and receives wireless data simultaneously, ensuring no messages are dropped while the user is typing on the keypad.
### Project Structure
- `SMS_Messenger.ino`: The main logic that contains the multi-tap state machine and serial polling
- `getChar()` / `charCount()`: The look-up tables that map physical key coordinates to the character sets.
