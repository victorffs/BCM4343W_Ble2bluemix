Welcome to the BCM4343W_Ble2bluemix wiki!

# 1.  Requirements:

* ZentriOS account
https://dms.zentri.com/signup/

* IBM Bluemix Account
https://console.ng.bluemix.net/registration/

* ZentriOS SDK
http://resources.zentri.com/zentrios_sdk/ZentriOS_SDK_Installer.exe

* JRE - Java Runtime Environment
http://www.oracle.com/technetwork/java/javase/downloads/jre8-downloads-2133155.html

# 2. Setting up demo

* Download to BCM4343W
* Import this project to ZentriOS SDK
* Right click on the project > ZentriOS > Build/Download/Run
* Wait for finish

# 3. Setting up connection
* Start a Serial communication with Tera Term or Putty
* If this board have the factory configurations, go to step 3.1, else go to step 3.2
* You can reset all the configurations using the comand "fac" and your MAC Adress as arg. Example: fac XX:XX:XX:XX:XX:XX
* The command to show your MAC Adress is: get wlan.mac

#

* Verify the network connection
* You can see the status using the command "nup" 
