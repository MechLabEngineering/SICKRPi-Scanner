<h1>SICKRPi-Scanner</h1>
===============

SICK Laserscanner, Tinkerforge IMU und RaspberryPi Scanner

<h2>Build</h2>
===============

**Dependencies:**

* IbeoLux
* Boost [http://www.boost.org/](http://www.boost.org/)
* Octomap [https://github.com/OctoMap/octomap](https://github.com/OctoMap/octomap)
* Tinkerforge (included) [http://www.tinkerforge.com](http://www.tinkerforge.com)

Adapt the library paths in the makefile.

Change to SICKRPi-Scanner folder and run make.

<h2>Config</h2>
===============

Use the config.cfg file to change scanner and program parameter.

The path to config file is included in CReader.cpp

<h2>Run</h2>
===============

* check if Tinkerforge devices and LRF active
* scan beginn if the leds from the imu brick active

**Without button control:**

Execute the main binary.
To stop the scan, run the quit.py script, which will send the stop cmd.

**With button control:**

Use the start script (start.py) to control the scanner with the buttons from the tinkerforge dual button bricklet.

