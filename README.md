# DEVS Manual Example (RT)
|  Linux | Windows| ESP32 | MSP432 |
|:--|:--|:--|:--|
|:heavy_check_mark:|:heavy_check_mark:|:x:|:x:|

## Introduction
This repository contains the example code for the Cadmium Developers Manual

## Dependencies
This project assumes that you have Cadmium installed in a location accessible by the environment variable $CADMIUM.
_This dependency would be met by default if you are using the DEVSsim servers. To check, try `echo $CADMIUM` in the terminal_

## Build
To build this project, run:
```sh
source build_sim.sh
```
__NOTE__: Everytime you run build_sim.sh, the contents of `build/` and `bin/` will be replaced.

## Execute
To run the two models in this project, run:
```sh
./bin/counter_tester
./bin/testcase_counter
```

__NOTE__: Remember to chnage the file path to the testcase files in `counter_tester.hpp` and `testcases.hpp`. Else it would result in a fault

For more information about the models in this repository, and to learn more about modeling in Cadmium, look at the developer's manual found [here](https://devssim.carleton.ca/manuals/developer/).