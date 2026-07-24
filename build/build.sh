#!/bin/bash

cd ..
mkdir bin
cd source
mkdir objects
cd game
make
cd ../server
make
