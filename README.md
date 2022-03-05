# SondeCamGround

Ground station gateway to receive Sonde pictures using Uputronics HAB-LORAZERO-4 
https://store.uputronics.com/index.php?route=product/product&path=61&product_id=99

How to install on Rapsberry PI
1) Install the latest version of the Raspberry PI OS Debian version: 11 (bullseye)
2) sudo apt-get install libwebsockets-dev
3) sudo apt-get install ftp-upload
4) Copy the zip file from git into the home directory
5) Unzip the file
6) cd SondeGround/Release
7) type make
8) cd SondeGround
9) edit the habGWconfig.ini for your particular gateway
10) To autostart at boot refer to SondeGround/systemd/readme.txt

To view the pictures and telemetry data go to: http://192.169.177.148:8080/
