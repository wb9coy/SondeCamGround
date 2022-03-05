# SondeCamGround

Ground station gateway to receive Sonde pictures using Uputronics HAB-LORAZERO-4 
https://store.uputronics.com/index.php?route=product/product&path=61&product_id=99

How to install on Rapsberry PI
1) Install the latest version of the Raspberry PI OS Debian version: 11 (bullseye)
2) Install the following:
	  sudo apt-get install libwebsockets-dev
	  sudo apt-get install ftp-upload
3) Copy the zip file from git in to the home directory
4) Unzip the file
5) cd SondeGround/Release
6) type make
7) cd SondeGround
8) edit the habGWconfig.ini for your particular gateway
9) To autostart at boot refer to SondeGround/systemd/readme.txt

To view the pictures and telemetry data go to: http://192.169.177.148:8080/
