# Microcontroller-Engine-Governor:
## Quick Links:
- [Project Explanation Video - Spring 2023](https://drive.google.com/file/d/1iZ02NeBIo6aJ6PkN2pi3ibztsFP9TJ7J/view?usp=share_link)
- [Final Report - Spring 2023 (Rough Draft)](https://github.com/december454/Microcontroller-Engine-Governor/blob/main/Final%20Reports/Microcontroller%20Based%20Engine%20Governor%20-%20Spring%202023%20(Rough%20Draft).pdf)

- [Test Run Videos](https://drive.google.com/drive/folders/1vf2UiMMJDWpa3UTO7wDQ3YrPN7G2oo9Z?usp=sharing)
- [Program - Spring 2023](https://github.com/december454/Microcontroller-Engine-Governor/blob/main/Contract%20Course%20-%20Spring%202023/Final-Program/Final-Program.ino)
- [Program - Fall 2022](https://github.com/december454/Microcontroller-Engine-Governor/blob/main/Contract%20Course%20-%20Fall%202022/Final%20Program%20-%20Different%20Versions/Final/Engine-Governor-Final-Commented/Engine-Governor-Final-Commented.ino)
- [Final Report - Fall 2022](https://github.com/december454/Microcontroller-Engine-Governor/blob/main/Final%20Reports/Microcontroller%20Based%20Engine%20Governor%20Simulator%20-%20Fall%202022.pdf)

## Abstract
Many internal combustion engines are designed to run at a constant, specified RPM. This is particularly common in small gasoline engines, such as those found in lawnmowers, generators, snow-blowers, etc. In these cases, the device must run at a specific speed to function properly. For example, a generator’s RPM is tied to the voltage which it outputs.

To maintain a specified RPM, engines utilize a governor. This is a mechanical or electronic device which regulates the engine’s throttle. If the RPM begins to drop below the set value, the governor will open the throttle more, causing the engine to speed up. If the RPM begins to rise above the set value, the governor will close the throttle, causing the engine to slow down. This is analogous to cruise-control in a car.

Most small engines utilize rudimentary mechanical centrifugal governors. These devices are simple to manufacture and well understood. However, a computerized governor has a number of potential benefits over a purely mechanical design, such as increased RPM accuracy, increased RPM precision, and improved engine reliability / consistency.

This project will explore these benefits and measure how much of an improvement is gained by transitioning to a computer-controlled governor. It will measure factors such as, RPM consistency, engine response to changing engine loads, engine performance at various loads, etc. 

Additionally, this project will document the design / fabrication process of creating the computerized governor. This will provide valuable insight on engine control theory and engine electronics.

## Experiment Results:
- [Raw Data](https://github.com/december454/Microcontroller-Engine-Governor/tree/main/Data-Logging/Experiments): Click here to view the individual Excel spreadsheets / txt data logs for each experiement trial.
### Mechanical Governor:
- [No Load (Idle)](https://github.com/december454/Microcontroller-Engine-Governor/blob/main/Experiment%20Summaries/Summary%20-%20Mech%20Gov%20-%20No%20Load.xlsx)
- [Constant Load](https://github.com/december454/Microcontroller-Engine-Governor/blob/main/Experiment%20Summaries/Summary%20-%20Mech%20Gov%20-%20Constant%20Load.xlsx)
- [Abrupt Load Increase (Initially No Load)](https://github.com/december454/Microcontroller-Engine-Governor/blob/main/Experiment%20Summaries/Summary%20-%20Mech%20Gov%20-%20Load%20Increase%20(Initially%20No%20Load).xlsx)
- [Abrupt Load Increase (Initially Constant Load)](https://github.com/december454/Microcontroller-Engine-Governor/blob/main/Experiment%20Summaries/Summary%20-%20Mech%20Gov%20-%20Load%20Increse%20(Initially%20Constant%20Load).xlsx)
- [Abrupt Load Decrease](https://github.com/december454/Microcontroller-Engine-Governor/blob/main/Experiment%20Summaries/Summary%20-%20Mech%20Gov%20-%20Load%20Decrease.xlsx)
### Microcontroller Governor:
- [No Load (Idle)](https://github.com/december454/Microcontroller-Engine-Governor/blob/main/Experiment%20Summaries/Summary%20-%20Micocontroller%20Gov%20-%20No%20Load.xlsx)
- [Constant Load](https://github.com/december454/Microcontroller-Engine-Governor/blob/main/Experiment%20Summaries/Summary%20-%20Microcontroller%20Gov%20-%20Constant%20Load.xlsx)
- [Abrupt Load Increase (Initially No Load)](https://github.com/december454/Microcontroller-Engine-Governor/blob/main/Experiment%20Summaries/Summary%20-%20Microcontroller%20Gov%20-%20Load%20Increase%20(No%20Load).xlsx)
- [Abrupt Load Increase (Initially Constant Load)](https://github.com/december454/Microcontroller-Engine-Governor/blob/main/Experiment%20Summaries/Summary%20-%20Microcontroller%20Gov%20-%20Load%20Increase%20(Constant%20Load).xlsx)
- [Abrupt Load Decrease](https://github.com/december454/Microcontroller-Engine-Governor/blob/main/Experiment%20Summaries/Summary%20-%20Micrcontroller%20Gov%20-%20Load%20Decrease.xlsx)
### Baseline - No Throttle Addjustment:
- [No Load (Idle)](https://github.com/december454/Microcontroller-Engine-Governor/blob/main/Experiment%20Summaries/Summary%20-%20No%20Throttle%20Input%20-%20No%20load.xlsx)
- [Constant Load](https://github.com/december454/Microcontroller-Engine-Governor/blob/main/Experiment%20Summaries/Summary%20-%20No%20Throttle%20Input%20-%20Constant%20Load.xlsx)

## To-Do
- ~~Reinforce Stepper Motor Bracket~~ (Completed 3-3-2023)
- ~~Secure Limit Switch~~ (Completed 3-3-2023)
- ~~Rewrite code so that the "directionFlag" is controlled by the PID output.~~ (Completed 3-7-2023)
- ~~Add debugging info that outputs to serial on program startup.~~ (Completed 3-9-2023)
- ~~Test lowering the RPM Calc Interval.~~ (Completed 3-9-2023)
- ~~Replace I2C LCD display with traditional 1602 LCD.~~ (Completed 3-8-2023)
- ~~Add Main Power Switch.~~ (Completed 3-8-2023)
- ~~Add Stepper Power Switch.~~ (Completed 3-8-2023)
- ~~Change RPM calc timer (timeElapsed) from milliseconds to microseconds.~~ (Completed 3-10-2023)
- ~~Add Spring to Pre-Load Stepper Gearbox~~ (Completed 3-22-2023)
- ~~Improve Shutdown Behavior~~
- ~~Improve Data Logging.~~
- ~~Tune PID Loop~~
- ~~Add more useful info to readme.~~
- ~~Perfrom Experiments on Electroic Governor~~ (Completed 4-5-2023)
- ~~Perfrom Experiments on Original Governor~~ (Completed 4-3-2023)
- ~~Perfrom Experiments on Control (No Throttle Input)~~ (Completed 3-31-2023)
- ~~Analyze Data~~ (Completed 4-9-2023)
- Complete Final Report
