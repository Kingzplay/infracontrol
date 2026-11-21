# Infracontrol
Save and play infrared signals from remote controls using a Raspberry Pi Pico
## Table of contents
* [Setup](#setup)
* [Demonstration](#demonstration)
* [Notes](#notes)
# Setup
Connect the X and Y pins of the joystick to analog pins (default: pin 26 and pin 27)
Connect the Z (button) pin of the joystick to a GPIO pin (default: pin 25)
Connect the output pin of the IR receiver to a GPIO pin (default: pin 24)
Connect the anode of the IR LED to a GPIO pin (default: pin 14)
Connect the SDA and SCL pins of a SH1106 controlled OLED to I2C compatible GPIO pins (default: pin 13 and pin 12)
You can change the default pins by changing the values of corresponding variables (check the header files to see the variable names) but make sure to pick the right pins by reading the RP2040 datasheet
# Demonstration
https://github.com/user-attachments/assets/be8f782c-5770-43fe-b8d0-3ddf1218281d
I unplugged the Pico halfway through to show that the signals are saved in the Flash memory (permanent memory)
Sorry for the bad filming
# Notes
The 'Settings' menu hasn't been coded, I may or may not complete it
It might not work with AC remote controls or other really goofy remotes with different modulation frequencies or very long signals


