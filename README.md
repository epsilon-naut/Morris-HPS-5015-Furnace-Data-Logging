# HPS Data Logger

## Overview
Tool for extracting and logging pressure and temperature data for the Morris HPS-5015 Oxygen Furnace. Shown below is an example of the GUI recording both temperature and pressure data, which it can save to a user-defined CSV.

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

## Current Issues (as of 09.05.2026)
- There seems to be issues with the Waveforms SDK's logic.record function with our Analog Discovery 2, preventing digital temperature measurements for the time being. Pressure data readings are unaffected.
- The issue may stem from trying to port the code onto a different computer for data logging with a different version of the Waveforms SDK, but the logic analyzer feature on the Waveforms GUI (which I presume uses logic.record) seems to work, so the Analog Discovery 2 I'm using might still be able to read logic signals.
- Since the main purpose of using the tool as of right now is to log pressure data to check for leaks, no significant changes will be made in the near future (see pressure_only.cpp as an example of code for a non-GUI version that only tracks pressure data)

## Notes
Developed for use with the Morris HPS-5015 Oxygen Furnace in MP089 (Department of Physics, University of Toronto)
