# VRC-Haptics-Firmware-ESP
Firmware for the DIY haptics solution, See the projects PC application for more info: [VRCH-server](https://github.com/VRC-Haptics/VRCH-Server)

## Usage
This is the current setup, soon this should change.

### Setup Environment
1. This project uses the Visual Studio Code extension PlatformIO. Please install both:
* [Visual Studio Code](https://code.visualstudio.com/)
* [PlatformIO](https://docs.platformio.org/en/latest/integration/ide/pioide.html#platformio-for-vscode)

2. Download this repository:
* Use the big green "Code" button on at the top of the page and select Download Zip. 
* Extract this repository somewhere you can find it later.
	* We will reference this location (path) as `<repository>` going forward.
   
### Setup Device
### Upload Firmware:
* Open the folder`<repository>/VRC-Haptics-Firmware-ESP` in Visual Studio Code
* Once PlatformIO has configured the project, simply upload your board variation.
	No Firmware configuration needed.
	1. Connect Board to computer VIA USB
	2. Expand your platform type from the PlatformIO (wasp logo) on the left sidebar
	3. Build and upload firmware: `<PLATFORM NAME>` -> `General` -> `Upload`
  
### Set Configuration: 
---
_Configuration is set via serial, I use arduinoIDE but that requries a relative lot of work to setup. Setting config via OSC is supported along with sending commands via the server, but not implemented yet. (simple python script maybe?)_
---

* Commands are formatted `<COMMAND> <NAME> <VALUE>` and are case insensitive (string values will be kept as they are)
	- `GET ALL` is a special command that dumps the current settings
	- `SET DEFAULT` Resets config to default. Needed since config is persistant across FW versions.
* `<COMMAND>`: commands are either `SET` or `GET`
	
	There are a few items to configure:
	1. WIFI: 
		* `SET WIFI_SSID <wifi_name>` this is your WIFI name you see on your computer. It tells the haptic device which network to connect to.
		* `SET WIFI_PASSWORD <wifi-password>`This is the password to your network, like what is used to connect your computer to WIFI.
	2. Device Name:
		* `SET MDNS_NAME <name>` Please note that names are currently limited to
		 12 regular characters. 
		 
		 -> __From this point your board should connect to the internet and show up on the server__. 
	3. Motor configuration:
		* `set motor_map_ledc <csv_map>` List the pins that are directly hooked to your motors. (up to 64 supported in the firmware)
		* `set motor_map_i2c <csv_map>` List the pin indices for PWM outputs over I2C modules. (2 modules max)

### Enjoy!
If your configuration is accurate, your board is now capable of connecting to the server and driving your haptics. Have fun!