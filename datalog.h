#ifndef DATALOG_H
#define DATALOG_H

#include <stdio.h>
#include <vector>
#include <string>
#include <cstdint>

using namespace std;

#define EOT 0x04 // 0 0000100 1 1 -> 0000010011 -> 0x13 -> 1000010011 -> 10000100 -> 0x84 -> 1 1 0000100 0 -> 0 0010000 1 1 -> 0001000011
#define ENQ 0x05 // 0 0000101 0 1 -> 0000010101 -> 0x15 -> 1 0 0000101 0 -> 0 1010000 0 1 -> 0101000001
#define STX 0x02 // 0 0000010 1 1 -> 0000001011 -> 0x0B 
#define ETX 0x03 // 0 0000011 0 1 -> 0000001101 -> 0x0D 
#define ACK 0x06 // 0 0000110 0 1 -> 0000011001 -> 0x19  
#define NAK 0x15 // 0 0010101 1 1 -> 0001010111 -> 0x57
#define ZERO 0x30 // 0 0110000 0 1 -> 1 0 0110000 0 -> 0 0000110 0 1 -> 0000011001 
#define ONE 0x31 // 0 0110001 1 1 -> 1 1 0110001 0 -> 0 1000110 1 1 -> 0100011011
#define TWO 0x32
#define THREE 0x33
#define FOUR 0x34
#define FIVE 0x35
#define SIX 0x36
#define SEVEN 0x37
#define EIGHT 0x38
#define NINE 0x39
#define A 0x41
#define B 0x42
#define C 0x43
#define D 0x44
#define E 0x45
#define F 0x46
#define G 0x47
#define H 0x48
#define I 0x49
#define J 0x4A
#define K 0x4B
#define L 0x4C
#define M 0x4D
#define N 0x4E
#define O 0x4F
#define P 0x50 // 0 1010000 0 1 -> 1 0 1010000 0 -> 0 0000101 0 1 -> 0000010101
#define Q 0x51
#define R 0x52
#define S 0x53
#define T 0x54
#define U 0x55
#define V 0x56 // 0 1010110 0 1 -> 1 0 1010110 0 -> 0 0110101 0 1 -> 0011010101
#define W 0x57
#define X 0x58
#define Y 0x59
#define Z 0x5A

void datalog(string name, int config, double out_freq, double sample_rate, int chI, int chO, int del, int achI, int asr, double offset, double amp, string filename);

#endif