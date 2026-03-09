#!/bin/bash
cd "$(dirname "$0")"
g++ -c -g -Wall src/*.cpp -I include
g++ *.o -o bin/debug/prog -lSDL3 -lSDL3_ttf
