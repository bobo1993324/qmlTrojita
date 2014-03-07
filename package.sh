#! /bin/bash
mkdir package
cp ./logo.png ./qmltrojita.json ./qmltrojita.desktop ./manifest.json build/src/qmltrojita build/src/qml package/ -rf
cd package
click build .
cp *.click ..
cd ..
rm -rf package

