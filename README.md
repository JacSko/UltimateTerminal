# UltimateTerminal

*** This project is still under development ***

## Project description

This application was designed to support communication with embedded systems during it's development and maintenance.
Supports communication over serial ports and the Ethernet as well.
Allows to send raw string-based data with defined delimiter at the end, more over, thanks to many customizable buttons
allows to automate the command sending.
File logging and regex-based trace filters allows to make logs gathering and parsing more convenient.

## Main features:
- Serial and Ethernet support,
- Up to 5 port opened simultaneously,
- Two data views. One generic to present data from all ports, second allows to filter out interesting traces via regular expressions,
- Customizable trace coloring for both data views,
- Logging to file,
- Customizable buttons that allows to send list of commands to defined,
- All configuration data is stored persistently and restored on startup,
- Possibility to disable view scroll-down to allow reading interesting traces on-the-fly. 

## Building:
Application is basing on QT framework, therefore before any build activities, please install QT5 on Your machine.  
For convenience, there are prepared bash scripts to automate the building and testing.

- Building application for OS target (currently only Linux platform is supported):

```
./build_app.sh
```
- Running the unit tests:

```
./run_ut.sh
```
- Running the unit tests coverage:

```
./run_coverage.sh  
  
The coverage report is available at ./build_ut_coverage/html/coverage.html
```

- Running the thread sanitizer:

```
./run_thread_sanitizer.sh  
```
- Running the address sanitizer:

```
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
The application has possibility to stub QT GUI interface (GUIController.h) and all related dialog windows with the stubs
with RPC server for testing purposes.  
This allows to perform extended tests of almost whole application, checking the GUI state without showing the GUI interface.
In the **testing** sub-directory there is the all simulation-related stuff defined.  

- **TestCases** - contains the test cases written with use of GTest Framework and TestFramework
- **TestFramework** - Implementations required to perform testing. There are methods that allows to check the GUI element state,
control the Ethernet or Serial peripherial, start the tested binary as well as setting up the two serial ports inside 
of the system with data forwarding.
- **TestStubs** - definitions of all stuff that have to be stubbed for simulation test, i.e the GUIController and all of the *Dialog windows.

## TODO list:
- [ ] Refactor the UserButtonHandler to avoid creating new thread for every button (consider one thread or timers)
- [ ] Windows support (currently only linux is supported)
- [ ] Create serial and socket drivers with use of WIN32 API (and replace the QT drivers)
- [ ] Investigate the overall application performance under high data traffic, check for improvements
- [ ] Create application logo and splash screen
- [ ] Develop simulation tests
- [ ] Develop unit tests
- [ ] Code documentation with block and sequence diagrams
- [ ] Cleanup in code formatting, Doxygen tags update and creation
- [ ] Refactor SettingsHandler and switch from JSON to XML (to support comments for setting keys)

Screenshots:
![main_application_data_new](https://user-images.githubusercontent.com/47041583/219138558-25f26352-057f-471e-8e34-660ef9a5c417.png)
![port_settings](https://user-images.githubusercontent.com/47041583/219138593-3df2c6b3-c776-4a94-a837-df5266788ed0.png)
![trace_filter_settings](https://user-images.githubusercontent.com/47041583/219138613-6f58f250-bab2-46ea-80aa-6571ded0924d.png)
![settings1](https://user-images.githubusercontent.com/47041583/219138618-a49a4b5a-d7cb-4498-bee5-93dc7e775a31.png)
![settings2](https://user-images.githubusercontent.com/47041583/219138626-eae35481-f214-42bb-a6a5-2038e346b9cc.png)
