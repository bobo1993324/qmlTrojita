#! /bin/bash
mkdir package
cp ./emailclient64.png ./ubuntu-emailclient-app.json ./ubuntu-emailclient-app.desktop ./manifest.json build/src/ubuntu-emailclient-app build/src/qml package/ -rf
cd package
click build .
cp *.click ..
cd ..
rm -rf package
