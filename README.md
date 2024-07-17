# Embedded Lab - RASPNet Protocol

## Contents
 * [Overview](#overview)
 * [Project Tree](#project-tree)
 * [How to build the project and flash application to ATMEGA328p](#how-to-build-the-project-and-flash-application-to-atmega328p)
 * [Code Documentation](#code-documentation)
### Overview
Implementation of [RASPNet Protocol](https://osg.informatik.tu-chemnitz.de/lehre/emblab/raspnet.pdf) 

### Project Tree
```sh
├── README.md -> Brief docummentaion of the project 
├── Makefile  ->  Contain the make rules for the project
├── RASPNet/ -> Implementation of the protocol
    ├─ CRC/
    ├─ Layer1/
    ├─ Layer2/
    ├─ Layer3/
    ├─ PinConfig/
    ├─ UART/
    ├─ FrameQueue/ 
    ├─ PiAtmegaAPI/
    ├─ Timer/
    ├─ Utils/
└── DoxyConfig -> Config file for Doxygen Documentation
```

### How to build the project and flash application to ATMEGA328p 
- Pre Requisites:
  * [make](https://www.gnu.org/software/make/#download) to build the Project
  * [AVR-GCC](https://github.com/avrdudes/avr-libc) to compile code for ATMEGA chip
  * [DoxyGen](https://doxygen.nl/download.html) for document generation
  * [Graphviz](https://www.graphviz.org/download/) for generating graps along with documentation

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
- Connect to RPi serial port using and Serial monitor tool 
```sh
minicom ama0
```

### Code Documentation
- Tool Used
  * [DoxyGen](https://doxygen.nl/download.html)
  * [Graphviz](https://www.graphviz.org/download/) for generating graps along with documentation

- Generate Documents
- The repo will contain the generated documents initially, as a backup option because if dependencies are missing the graphs will not be genrated.
```sh
make doc
```

- Access the Documents
  * After running the previous command "RASPNet_Documentation" directory is created
  * Current "DoxyConfig" generates HTML docments
  * Navigate to RASPNet_Documentation\html
  * Open index.html in any browser
- Sample screenshots of the documentation 