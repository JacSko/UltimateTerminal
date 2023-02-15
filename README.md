# UltimateTerminal

*** This project is still under development ***

See *dev* branch for details.

## Project description

This application was designed to support communication with embedded systems during it's development od maintenance.
Supports communication over serial ports and the Ethernet (sockets) as well.

Main features:
- Serial and socket support,
- Two terminal views. One generic to present data from all ports, seconds allows to filter out interesing traces via regular expressions,
- Customizable trace coloring for both windows,
- Logigng to file,
- Customizable buttons that allows to send list of commands to defined, opened port. Buttons are divided into tabs, configured via system_config.json,
- All configuration data is stored persistently and restored on startup,
- Possiblity to disable terminal scroll-down on trace adding,

Upcoming tasks:
- Fix Windows support (currently only linux is supported)
- Create serial and socket drivers with use of WIN32 API (and replace the QT drivers)
- Investigate the overall application performance under high data traffic, check for improvements
- Create application logo and splash screen
- Develop simulation tests
- Develop unit tests
- Create worthy readme :)
- Some documentation with block and sequence diagrams
- Cleanup in code formatting, Doxygen tags update and creation
- Refactor SettingsHandler and switch from JSON to XML (to support comments for setting keys)

Screenshots:
![main_application_data_new](https://user-images.githubusercontent.com/47041583/219138558-25f26352-057f-471e-8e34-660ef9a5c417.png)
![port_settings](https://user-images.githubusercontent.com/47041583/219138593-3df2c6b3-c776-4a94-a837-df5266788ed0.png)
![trace_filter_settings](https://user-images.githubusercontent.com/47041583/219138613-6f58f250-bab2-46ea-80aa-6571ded0924d.png)
![settings1](https://user-images.githubusercontent.com/47041583/219138618-a49a4b5a-d7cb-4498-bee5-93dc7e775a31.png)
![settings2](https://user-images.githubusercontent.com/47041583/219138626-eae35481-f214-42bb-a6a5-2038e346b9cc.png)
