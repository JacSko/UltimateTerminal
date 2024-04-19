# UltimateTerminal

## Project description

This application was designed to support communication with embedded systems during it's development and maintenance.
Supports communication over serial ports, ethernet and is able to provide stdout of started process as well.
Allows to send raw string-based data with defined delimiter at the end, more over, thanks to many customizable buttons
allows to automate the command sending.
File logging and regex-based trace filters allows to make logs gathering and parsing more convenient.

## Main features:
- Serial, Ethernet and Command support,
- Up to 5 port opened simultaneously,
- Two data views. One generic to present data from all ports, second allows to filter out interesting traces via regular expressions,
- Customizable trace coloring,
- Logging to file,
- Customizable buttons that allows to send list of commands to defined,
- All configuration data is stored persistently and restored on startup,

## Building:
Application is basing on QT framework, therefore before any build activities, please install QT5 on Your machine.  
For convenience, there are prepared bash scripts to automate the building and testing.

- Building application for OS target (currently only Linux platform is supported):
```
./build_app.sh
```

- Running the unit-tests based checks:
```
./run_ut.sh
./run_coverage.sh (The coverage report is available at ./build_ut_coverage/html/coverage.html)
./run_thread_sanitizer.sh
./run_address_sanitizer.sh 
```

- Running the simulation tests:
```
./run_sim_tests.sh  
```

- Commit verification:
```
./verify_build.sh  
  
This runs the build check, unit tests and all simulation tests
```

## Simulation
The application has possibility to remotely control GUI elements.
This was achieved using GUITestServer class which implements the RPC server to handle requests from simulation tests (TestFramework).
Such feature allows to perform extended, automated tests of whole application and checking the GUI state without showing the GUI interface.
In the **testing** sub-directory there is the all simulation-related stuff defined.

- **TestCases** - contains the test cases written with use of GTest Framework and TestFramework
- **TestFramework** - Implementations required to perform testing. There are methods that allows to check the GUI element state,
control the Ethernet or Serial peripherial, start the tested binary as well as setting up the two serial ports inside 
of the system with data forwarding.
- **TestStubs** - definitions of all stuff that have to be stubbed for simulation test, i.e the GUIController and all of the *Dialog windows.


Screenshots:
![main_application_data_new](https://user-images.githubusercontent.com/47041583/219138558-25f26352-057f-471e-8e34-660ef9a5c417.png)
![port_settings](https://user-images.githubusercontent.com/47041583/219138593-3df2c6b3-c776-4a94-a837-df5266788ed0.png)
![trace_filter_settings](https://user-images.githubusercontent.com/47041583/219138613-6f58f250-bab2-46ea-80aa-6571ded0924d.png)
![settings1](https://user-images.githubusercontent.com/47041583/219138618-a49a4b5a-d7cb-4498-bee5-93dc7e775a31.png)
![settings2](https://user-images.githubusercontent.com/47041583/219138626-eae35481-f214-42bb-a6a5-2038e346b9cc.png)
