# Embedded Lab - RASPNet Protocol

## Overview
Implementation of [RASPNet Protocol](https://osg.informatik.tu-chemnitz.de/lehre/emblab/raspnet.pdf) 

### Project Tree
```sh
├── DoxyConfig -> Config file for Doxygen Documentation
├── Makefile  ->  Contain the make rules for the project
├── RASPNet -> Implementation of the protocol 
├── README.md -> 
├── Task1 -> (miscellaneous) Solution for Lab Task 1
├── Task2 -> (miscellaneous) Solution for Lab Task 2
└── Task3 -> (miscellaneous) Solution for Lab Task 3
```

### How to build the project and flash application to ATMEGA328p 
- Pre Requisites:
  * [AVR-GCC](https://github.com/avrdudes/avr-libc)
  * [DoxyGen](https://doxygen.nl/download.html) 
  * [make](https://www.gnu.org/software/make/#download)
  * RaspberryPi - (TODO)
  * Gertboard - (TODO)

Build all files (target + dependencies)
```sh
make all
```
Build all files and flash to micro contorller
```sh
make f
```
Clean all generated files
```sh
make clean
```

Interactig with Application
***TODO***
- Connect to RPi serial port using and Serial monitor tool 
```sh
minicom ama0
```

### Code Documentation
- Tool Used
  * [DoxyGen](https://doxygen.nl/download.html)

- Generate Documents
```sh
make doc
```
- Access the Documents
  * After running the previous command "RASPNet_Documentation" directory is created
  * Current "DoxyConfig" generates HTML docments
  * Navigate to RASPNet_Documentation\html
  * Open index.html in any browser