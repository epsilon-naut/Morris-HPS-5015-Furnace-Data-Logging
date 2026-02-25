# HPS Data Logger

## Overview
Tool for extracting and logging pressure and temperature data for the Morris HPS-5015 Oxygen Furnace.

<img width="1918" height="1015" alt="Data Logger 5" src="https://github.com/user-attachments/assets/ede8ed32-12d4-4307-916f-acf6dcf952f5" />

## Features
- Analog pressure data acquisition
- Digital temperature data acquisition
- Ability to write pressure and temperature data to CSV files for further analysis
- Visual real-time display of pressure and temperature data

## Languages / Packages Used
- C / C++
- Waveforms SDK
- Qt6

## Current Issues (as of 02.06.2026)
- There seems to be issues with the Waveforms SDK's logic.record function with our Analog Discovery 2, preventing digital temperature measurements for the time being. Pressure data readings are unaffected.

## Notes
Developed for use with the Morris HPS-5015 Oxygen Furnace in MP089 (Department of Physics, University of Toronto)
