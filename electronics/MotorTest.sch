EESchema Schematic File Version 2
LIBS:cybot-rescue
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:cybot-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 3 5
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Conn_01x07_Male J1
U 1 1 59C699AD
P 4100 3150
F 0 "J1" H 4100 3550 50  0000 C CNN
F 1 "Header" H 4100 2750 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Angled_1x07_Pitch2.54mm" H 4100 3150 50  0001 C CNN
F 3 "" H 4100 3150 50  0001 C CNN
	1    4100 3150
	1    0    0    -1  
$EndComp
Wire Wire Line
	4300 2850 4750 2850
Wire Wire Line
	4450 2850 4450 3350
Wire Wire Line
	4300 3050 4750 3050
Wire Wire Line
	4300 3350 4750 3350
Connection ~ 4450 3050
Text HLabel 4750 2850 2    60   Output ~ 0
M2A
Text HLabel 4750 2950 2    60   Output ~ 0
M2B
Text HLabel 4750 3050 2    60   Output ~ 0
M1A
Text HLabel 4750 3150 2    60   Output ~ 0
M1B
Text HLabel 4750 3250 2    60   Output ~ 0
+5V
Text HLabel 4750 3350 2    60   Output ~ 0
+6V
Text HLabel 4750 3450 2    60   Output ~ 0
GND
Wire Wire Line
	4300 2950 4750 2950
Wire Wire Line
	4300 3150 4750 3150
Wire Wire Line
	4300 3250 4750 3250
Wire Wire Line
	4300 3450 4750 3450
Connection ~ 4450 2850
Connection ~ 4450 3350
NoConn ~ 4300 3150
NoConn ~ 4300 3250
NoConn ~ 4300 3450
NoConn ~ 4300 2950
$EndSCHEMATC
