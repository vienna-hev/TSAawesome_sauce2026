#!/bin/bash
cd "$(dirname "$0")"
g++ -c src/*.cpp -O2 -I include
g++ *.o -o bin/release/rhombus-rampage -O2 -lSDL3 -lSDL3_ttf
