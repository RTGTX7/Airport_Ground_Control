# DEVS Manual Example (RT)
|  Linux | Windows| ESP32 | MSP432 |
|:--|:--|:--|:--|
|:heavy_check_mark:|:heavy_check_mark:|:x:|:x:|

## Introduction
This project models and simulates an airport landing system using the DEVS formalism. The system manages aircraft landing, parking, and maintenance through a hierarchical structure of atomic and coupled models. 

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
./bin/Airport_Ground_Control_System
```

__NOTE__: Remember to chnage the file path to the testcase files in `Landing_Control.hpp`. Else it would result in a fault
