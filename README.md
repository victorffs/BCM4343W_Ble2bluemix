Welcome to the BCM4343W_Ble2bluemix wiki!

# 1.  Requirements:

### ZentriOS Account
https://dms.zentri.com/signup/

### ZentriOS SDK
http://resources.zentri.com/zentrios_sdk/ZentriOS_SDK_Installer.exe

### JRE - Java Runtime Environment
http://www.oracle.com/technetwork/java/javase/downloads/jre8-downloads-2133155.html

### IBM Bluemix Account
https://console.ng.bluemix.net/registration/

* Obs. I'm suposing that you have already setup a Bluemix Application and you know your device Credencials to this app. I will do another tutorial explaining how to do that soon.

# 2. Download the application

* Download to BCM4343W
* Import this project to ZentriOS SDK
* Right click on the project > ZentriOS > Build/Download/Run
* Wait for finish

# 3. Setting up Demo
* Start a Serial communication with Tera Term or Putty
* If this board have the factory configurations, go to step 3.1, else go to step 3.2
* You can reset all the configurations using the comand "fac" and your MAC Adress as arg. Example: **fac XX:XX:XX:XX:XX:XX**
* The command to show your MAC Adress is: **get wlan.mac**

## 3.1 Setting up network
* Use the command `nup -s` to scan all networks
* Type the number # that matches your Network
* Type the password for your Network
* Use the command `save` to save your configurations
* Reset using the reset button or the command `reboot`

## 3.2 Setting up IBM Connection
### Set your organization ID
*Use the command `set bluemix.org_id` with your Organization ID provided by IBM in your application
*This ID should be a combination of 6 chars including letters and numbers

### 3.3 Set your device token
*Use the command `set bluemix.device_token` with your Autentication Token provided by IBM in your application
*This Token should be a combination of 18 chars including letters, numbers and symbols

### 3.4 Save and reboot
* Use the command save
* Use the command `reboot`

### 3.5 Verify informations
* If your setup is correct, your board will connect automatically to IBM Cloud after reset
Terminal will shows a message like this:   
> IBM Bluemix Application Started:  
> Organization ID: xxxxxx  
> Device Type: xxxxxxxx  
> Device ID: xxxxxxxxxxx  
> Device Token: xxxxxxxxxxxxxxxxxx  
> Bluemix Dashboard: `http://<org.ID>.internetofthings.ibmcloud.com/dashboard ` 
* Verify this info  

### 3.6 Connecting Wiced Sense 2
* After step 3.5 the BCM4343 will automaticaly start to scan for Wiced Sense 2 Kit.
* Turn your Wiced Sense 2 Kit on
* Your device will automaticaly appears in the terminal
* Use the comand `set ble2cloud.sensor_bdaddr` with the MAC Adress of your Wiced Sense 2 Kit.
* Tip: This program scans for a BLE device in loop. It's easier you type the command in the notepad, copy and paste in the terminal ("right button" if you are using Tera Term).
* The terminal will shows the message "Send PUBLISH frame" what means that you are receiving data on your Bluemix Application

# 4. Working with IBM Bluemix
* If your application has been correctly configured you can acess the Bluemix Dashboard Link (step 3.5) and see your dashboards.
* If you was redirected to another page when acessed the link, probably your application and dashboard isn't activated.
* You can activate them using the link https://console.ng.bluemix.net/dashboard/apps/
* Dont forget to activate the application `<appname>.mybluemix.net` and the IoT Dashboards Services (at the bottom of the page)
* Have fun!

