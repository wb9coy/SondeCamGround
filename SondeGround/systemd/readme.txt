To enable the ground station gateway to startup at boot execute the following commands:

sudo cp /home/pi/SondeGround/systemd/sondeGround.service /lib/systemd/system
sudo chmod 644 /lib/systemd/system/sondeGround.service
sudo systemctl daemon-reload
sudo systemctl enable sondeGround.service
sudo reboot