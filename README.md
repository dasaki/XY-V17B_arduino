# XY-V17B_arduino
Arduino sketch to control XY-V17B MP3 module in UART mode.

Notes: for anyone's interest, I solved some noise issues by connecting the speakers's ground "-" terminal to the closest possible point of the XY-V17B GND pin. Also don't leave any audio channel unconnected (L and R), if you want a single speaker connect both channels to the same "+" terminal of the speaker.

![Connections schematic](https://github.com/dasaki/XY-V17B_arduino/blob/master/hardware/XY-V17B_arduino_uno_schematic.png)

![Breadboard connections](https://github.com/dasaki/XY-V17B_arduino/blob/master/hardware/XY-V17B_arduino_uno_breadboard.png)


David Sanz Kirbis December 2019



Using definitions and some code from Peter Barsznic:
https://github.com/electron1979/Arduino_XY-V17B_mp3_wav_player/blob/master/XY-V17B.ino
