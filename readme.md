# Microcontroller-Engine-Governor: Abstract
Many internal combustion engines are designed to run at a constant, specified RPM. This is particularly common in small gasoline engines, such as those found in lawnmowers, generators, snow-blowers, etc. In these cases, the device must run at a specific speed to function properly. For example, a generator’s RPM is tied to the voltage which it outputs.

To maintain a specified RPM, engines utilize a governor. This is a mechanical or electronic device which regulates the engine’s throttle. If the RPM begins to drop below the set value, the governor will open the throttle more, causing the engine to speed up. If the RPM begins to rise above the set value, the governor will close the throttle, causing the engine to slow down. This is analogous to cruise-control in a car.

Most small engines utilize rudimentary mechanical centrifugal governors. These devices are simple to manufacture and well understood. However, a computerized governor has a number of potential benefits over a purely mechanical design, such as increased RPM accuracy, increased RPM precision, and improved engine reliability / consistency.

This project will explore these benefits and measure how much of an improvement is gained by transitioning to a computer-controlled governor. It will measure factors such as, RPM consistency, engine response to changing engine loads, engine performance at various loads, etc. 

Additionally, this project will document the design / fabrication process of creating the computerized governor. This will provide valuable insight on engine control theory and engine electronics.


# To-Do
- ~~Reinforce Stepper Motor Bracket~~ (Completed 3-3-2023)
- ~~Secure Limit Switch~~ (Completed 3-3-2023)
- ~~Rewrite code so that the "directionFlag" is controlled by the PID output.~~ (Completed 3-7-2023)
- Add debugging info that outputs to serial on program startup.
- Replace I2C LCD display with traditional 1602 LCD.
- Add Main Power Switch
- Add Stepper Power Switch
- Improve Shutdown Behavior
- Tune PID Loop
