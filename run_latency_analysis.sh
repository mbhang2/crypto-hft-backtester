#!/bin/bash

if [ $# -ne 4 ]; then
    echo "Usage: $0 <Initial Spot Balance> <Initial Futures Balance> <Configuration Path> <Market Data Path>"
    exit 1
fi

arg1="$1"
arg2="$2"
arg3="$3"
arg4="$4"

# Boost is header-only here: brew install boost (macOS) or apt install libboost-dev
BOOST_INCLUDE="${BOOST_INCLUDE:-$(brew --prefix 2>/dev/null || echo /usr)/include}"
g++ -std=c++20 -O2 -I"$BOOST_INCLUDE" ./src/latency_analysis.cpp -o latency_executable

if [ $? -eq 0 ]; then
    ./latency_executable "$arg1" "$arg2" "$arg3" "$arg4"
    python3 ./src/latency_analysis.py
else
    echo "Compilation failed. Please check your code."
fi
