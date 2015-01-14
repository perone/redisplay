# Redisplay

Redisplay is a project that uses OLED displays to monitor
Redis activity. This project is the first prototype
towards a general platform for tiny physical and remote
monitors called **Stat Cubes**. It is still under development
and it will be redesigned in future to support different
services with a new hardware wireless platform.

## Architecture

The archictecure as it is today is very simple and is described
in the image below. There is an application gathering information
from the Redis server and then it serializes it using JSON
and send it over Serial (FTDI/USB) to the Arduino which in
turn is responsible for drawing the panels in the OLED display.

JSON is somehow overkill to send over the limited buffer that
Arduino has for Serial, but it was designed intentionally because
the Serial isn't going to be used in future for Stat Cubes.

![Image](./docs/arch.png?raw=true)

## Screenshots of the panels

![Image](./docs/redis_logo.png?raw=true)]

![Image](./docs/basic_stats.png?raw=true)]

![Image](./docs/arduino_mini_oled.png?raw=true)]

![Image](./docs/coin_reference.png?raw=true)]

![Image](./docs/ops_sec_stats.png?raw=true)]

![Image](./docs/advanced_stats.png?raw=true)]





