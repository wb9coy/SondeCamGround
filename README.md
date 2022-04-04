# SondeCamGround

Ground station gateway to receive Sonde pictures using Uputronics HAB-LORAZERO-4 
https://store.uputronics.com/index.php?route=product/product&path=61&product_id=99

How to install on Rapsberry PI
1) Install the latest version of the Raspberry PI OS Debian version: 11 (bullseye)
2) sudo apt-get install libwebsockets-dev
3) sudo apt-get install ftp-upload
4) download the zip file for this project
5) Open the zip file.
6) Within SodeCamGround-main copy the SoundGround directory to /home/pi
7) cd to /home/pi/SondeGround/Release
8) type make
9) cd /home/pi/SondeGround
10) edit the habGWconfig.ini for your particular gateway
11) sudo reboot
12) To autostart at boot refer to /home/pi/SondeGround/systemd/readme.txt

To view the pictures and telemetry data go to: http://192.169.177.148:8080/
