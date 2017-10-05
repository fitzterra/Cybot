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
Sheet 5 5
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
L TSOP384xx IR-R
U 1 1 59CCF2E1
P 3950 2400
F 0 "IR-R" H 3550 2700 50  0000 L CNN
F 1 "IR Receiver" H 3550 2100 50  0000 L CNN
F 2 "Opto-Devices:IRReceiver_Vishay_MINICAST-3pin" H 3900 2025 50  0001 C CNN
F 3 "" H 4600 2700 50  0001 C CNN
	1    3950 2400
	1    0    0    -1  
$EndComp
$Comp
L R R?
U 1 1 59CCF350
P 4900 1900
F 0 "R?" V 4980 1900 50  0000 C CNN
F 1 "100" V 4900 1900 50  0000 C CNN
F 2 "" V 4830 1900 50  0001 C CNN
F 3 "" H 4900 1900 50  0001 C CNN
	1    4900 1900
	0    1    1    0   
$EndComp
$Comp
L C C?
U 1 1 59CCF38E
P 4650 2150
F 0 "C?" H 4675 2250 50  0000 L CNN
F 1 "0.47uF" H 4675 2050 50  0000 L CNN
F 2 "" H 4688 2000 50  0001 C CNN
F 3 "" H 4650 2150 50  0001 C CNN
	1    4650 2150
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 59CCF436
P 4650 2750
F 0 "#PWR?" H 4650 2500 50  0001 C CNN
F 1 "GND" H 4650 2600 50  0000 C CNN
F 2 "" H 4650 2750 50  0001 C CNN
F 3 "" H 4650 2750 50  0001 C CNN
	1    4650 2750
	1    0    0    -1  
$EndComp
Text Label 5200 2400 0    60   ~ 0
IR_PIN
Text Label 5200 1700 0    60   ~ 0
+5V
Text Notes 3600 1750 0    60   ~ 0
IR Receiver\nin dome
Wire Wire Line
	4350 2200 4400 2200
Wire Wire Line
	4400 2200 4400 1900
Wire Wire Line
	4400 1900 4750 1900
Connection ~ 4650 1900
Wire Wire Line
	4350 2400 5200 2400
Wire Wire Line
	4650 2600 4350 2600
Wire Wire Line
	4650 2300 4650 2750
Connection ~ 4650 2600
Wire Wire Line
	4650 1900 4650 2000
Wire Wire Line
	5050 1900 5200 1900
Wire Wire Line
	5200 1900 5200 1700
Wire Notes Line
	3300 1500 5600 1500
Wire Notes Line
	5600 1500 5600 3000
Wire Notes Line
	5600 3000 3300 3000
Wire Notes Line
	3300 3000 3300 1500
Text Label 3650 4450 2    60   ~ 0
LDR_L
Connection ~ 3850 4450
Wire Wire Line
	3850 4450 3650 4450
Text Label 3850 3850 0    60   ~ 0
+5V
$Comp
L GND #PWR?
U 1 1 59D5A0B0
P 3850 5000
F 0 "#PWR?" H 3850 4750 50  0001 C CNN
F 1 "GND" H 3850 4850 50  0000 C CNN
F 2 "" H 3850 5000 50  0001 C CNN
F 3 "" H 3850 5000 50  0001 C CNN
	1    3850 5000
	1    0    0    -1  
$EndComp
Wire Wire Line
	3850 4850 3850 5000
Wire Wire Line
	3850 4400 3850 4550
Wire Wire Line
	3850 3850 3850 4100
$Comp
L R R?
U 1 1 59D59FDB
P 3850 4700
F 0 "R?" V 3930 4700 50  0000 C CNN
F 1 "4k7" V 3850 4700 50  0000 C CNN
F 2 "" V 3780 4700 50  0001 C CNN
F 3 "" H 3850 4700 50  0001 C CNN
	1    3850 4700
	1    0    0    -1  
$EndComp
$Comp
L LDR07 LDR-L
U 1 1 59D59F9A
P 3850 4250
F 0 "LDR-L" V 3950 4250 50  0000 C CNN
F 1 "LDR07" V 3925 4250 50  0001 C TNN
F 2 "Opto-Devices:Resistor_LDR_5.1x4.3_RM3.4" V 4025 4250 50  0001 C CNN
F 3 "" H 3850 4200 50  0001 C CNN
	1    3850 4250
	1    0    0    -1  
$EndComp
Text Label 4600 4450 0    60   ~ 0
LDR_R
Connection ~ 4400 4450
Wire Wire Line
	4400 4450 4600 4450
Text Label 4400 3850 0    60   ~ 0
+5V
$Comp
L GND #PWR?
U 1 1 59D5A20E
P 4400 5000
F 0 "#PWR?" H 4400 4750 50  0001 C CNN
F 1 "GND" H 4400 4850 50  0000 C CNN
F 2 "" H 4400 5000 50  0001 C CNN
F 3 "" H 4400 5000 50  0001 C CNN
	1    4400 5000
	1    0    0    -1  
$EndComp
Wire Wire Line
	4400 4850 4400 5000
Wire Wire Line
	4400 4400 4400 4550
Wire Wire Line
	4400 3850 4400 4100
$Comp
L R R?
U 1 1 59D5A217
P 4400 4700
F 0 "R?" V 4480 4700 50  0000 C CNN
F 1 "4k7" V 4400 4700 50  0000 C CNN
F 2 "" V 4330 4700 50  0001 C CNN
F 3 "" H 4400 4700 50  0001 C CNN
	1    4400 4700
	1    0    0    -1  
$EndComp
$Comp
L LDR07 LDR-R
U 1 1 59D5A21D
P 4400 4250
F 0 "LDR-R" V 4500 4250 50  0000 C CNN
F 1 "LDR07" V 4475 4250 50  0001 C TNN
F 2 "Opto-Devices:Resistor_LDR_5.1x4.3_RM3.4" V 4575 4250 50  0001 C CNN
F 3 "" H 4400 4200 50  0001 C CNN
	1    4400 4250
	-1   0    0    -1  
$EndComp
Wire Notes Line
	3300 3550 4950 3550
Wire Notes Line
	4950 3550 4950 5250
Wire Notes Line
	4950 5250 3300 5250
Wire Notes Line
	3300 5250 3300 3550
Text Notes 3500 3650 0    60   ~ 0
Left and Right Light Sensors
$EndSCHEMATC
