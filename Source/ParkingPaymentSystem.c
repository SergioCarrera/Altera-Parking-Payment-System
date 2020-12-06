#include <stdio.h>
#include <alt_types.h>
#include <sys/alt_alarm.h>
#include <sys/alt_irq.h>
#include "address_map.h"

// Declaration of global variables
int buttons_to_bills(int number);
volatile int *displayBaseAddress = HEX3_HEX0_BASE;
volatile int *greenLEDBaseAddress = GREEN_LED_BASE;
volatile int *buttonAddress = PUSHBUTTON_BASE;
volatile int *switchAddress = SLIDER_SWITCH_BASE;
volatile int *redLEDBaseAddress = RED_LED_BASE;
int HEX3to0_display (int value, unsigned char * table);
unsigned char table[10] = { 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x7, 0x7f, 0x6f};
unsigned char table2[10] = { 0x40, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x7, 0x7f, 0x6f};
int deposit_sum = 0;
volatile int *btn_ptr = (int *) 0x10000050;
alt_u32 alarm_callback(void *context);
void buttonISR(void *context, alt_u32 id);
static alt_alarm alarm1;
int myVar = 0;
int depositAmount = 0;
int billInserted = 0;
int tempBill = 0;
int changeDue = 0;
int value = 0;
int total = 0;
int hundredsPlace = 0;
int tensPlace = 0;
int firstPlace = 0;
int sedanPrice = 0;
int suvPrice = 0;
int truckPrice = 0;
int sedanCapacity = 0;
int suvCapacity = 0;
int truckCapacity = 0;
int initializationFlag = 0;
int preparationFlag = 0;
int sedanPriceFlag = 0;
int suvPriceFlag = 0;
int truckPriceFlag = 0;
int sedanCapacityFlag = 0;
int suvCapacityFlag = 0;
int truckCapacityFlag = 0;
int typeOfCustomerVehicle = 0;
int paymentStartedFlag = 0;
int t = 0;

int main(void)
{
	while(1)
	{
		if(total == 0){
			t = 0;
			t = 0x40 << (16);
			t += 0x40 << (8);
			t += 0x40;
			*displayBaseAddress = t;
		}
		usleep(100000);
		// Initialization Mode
		// Sets price to certain type of vehicle
		while(*switchAddress >= 512){
			usleep(100000);
			if(*buttonAddress == 2){
				unsigned int mask = 0xF;
				value = *switchAddress-512;
				int onesPlace = value & mask;
				int newValue = onesPlace;
				mask = 0xF0;
				int tensPlace = (value & mask) >> 4;
				newValue += 10 * tensPlace;
				sedanPrice = newValue;
				*displayBaseAddress = HEX3to0_display(newValue,table);
				sedanPriceFlag = 1;
				usleep(250000);
			}
			if(*buttonAddress == 4){
				unsigned int mask = 0xF;
				value = *switchAddress-512;
				int onesPlace = value & mask;
				int newValue = onesPlace;
				mask = 0xF0;
				int tensPlace = (value & mask) >> 4;
				newValue += 10 * tensPlace;
				suvPrice = newValue;
				*displayBaseAddress = HEX3to0_display(newValue,table);
				suvPriceFlag = 1;
				usleep(250000);
			}
			if(*buttonAddress == 8){
				unsigned int mask = 0xF;
				value = *switchAddress-512;
				int onesPlace = value & mask;
				int newValue = onesPlace;
				mask = 0xF0;
				int tensPlace = (value & mask) >> 4;
				newValue += 10 * tensPlace;
				truckPrice = newValue;
				*displayBaseAddress = HEX3to0_display(newValue,table);
				truckPriceFlag = 1;
				usleep(250000);
			}

			initializationFlag = 1;
			*displayBaseAddress = HEX3to0_display(NULL,table);
		}

		// Preparation Mode
		// Sets capacity to certain type of vehicle
		while(*switchAddress >= 256 && *switchAddress < 512 && initializationFlag == 1){
			if(*buttonAddress == 2 && sedanPriceFlag == 1){
				unsigned int mask = 0xF;
				value = *switchAddress-256;
				int onesPlace = value & mask;
				int newValue = onesPlace;
				mask = 0xF0;
				int tensPlace = (value & mask) >> 4;
				newValue += 10 * tensPlace;
				sedanCapacity = newValue;
				*displayBaseAddress = HEX3to0_display(sedanCapacity,table);
				sedanCapacityFlag = 1;
				usleep(250000);
			}
			if(*buttonAddress == 4 && suvPriceFlag == 1){
				unsigned int mask = 0xF;
				value = *switchAddress-256;
				int onesPlace = value & mask;
				int newValue = onesPlace;
				mask = 0xF0;
				int tensPlace = (value & mask) >> 4;
				newValue += 10 * tensPlace;
				suvCapacity = newValue;
				*displayBaseAddress = HEX3to0_display(suvCapacity,table);
				suvCapacityFlag = 1;
				usleep(250000);
			}
			if(*buttonAddress == 8 && truckPriceFlag == 1){
				unsigned int mask = 0xF;
				value = *switchAddress-256;
				int onesPlace = value & mask;
				int newValue = onesPlace;
				mask = 0xF0;
				int tensPlace = (value & mask) >> 4;
				newValue += 10 * tensPlace;
				truckCapacity = newValue;
				*displayBaseAddress = HEX3to0_display(truckCapacity,table);
				truckCapacityFlag = 1;
				usleep(250000);
			}

			preparationFlag = 1;

			// Display which types of vehicles have their prices set
			if (sedanPriceFlag == 1 && suvPriceFlag == 1 && truckPriceFlag == 1){
				t = 0;
				t = 0x4f << (16);
				t += 0x5b << (8);
				t += 0x06;
				*displayBaseAddress = t;
			}
			else if (sedanPriceFlag == 1 && suvPriceFlag == 1 && truckPriceFlag == 0){
				t = 0;
				t = 0x40 << (16);
				t += 0x5b << (8);
				t += 0x06;
				*displayBaseAddress = t;
			}
			else if (sedanPriceFlag == 1 && truckPriceFlag == 1 && suvPriceFlag == 0){
				t = 0;
				t = 0x4f << (16);
				t += 0x40 << (8);
				t += 0x06;
				*displayBaseAddress = t;
			}
			else if (suvPriceFlag == 1 && truckPriceFlag == 1 && sedanPriceFlag == 0){
				t = 0;
				t = 0x4f << (16);
				t += 0x5b << (8);
				t += 0x40;
				*displayBaseAddress = t;
			}
			else if(sedanPriceFlag == 1 && suvPriceFlag == 0 && truckPriceFlag == 0){
				t = 0;
				t = 0x40 << (16);
				t += 0x40 << (8);
				t += 0x06;
				*displayBaseAddress = t;
			}
			else if (suvPriceFlag == 1 && sedanPriceFlag == 0 && truckPriceFlag == 0){
				t = 0;
				t = 0x40 << (16);
				t += 0x5b << (8);
				t += 0x40;
				*displayBaseAddress = t;
			}
			else if (suvPriceFlag == 0 && truckPriceFlag == 0 && truckPriceFlag == 1){
				t = 0;
				t = 0x4f << (16);
				t += 0x40 << (8);
				t += 0x40;
				*displayBaseAddress = t;
			}
		}

		*displayBaseAddress = HEX3to0_display(NULL,table);

		// Payment Mode
		// Customer chooses their vehicle type
		if(initializationFlag == 1 && preparationFlag == 1 && *switchAddress < 256 && paymentStartedFlag == 0){
			//Calculate capacity of each type of each type of vehicle to display
				hundredsPlace = truckCapacity * 100;
				tensPlace = suvCapacity * 10;
				firstPlace = sedanCapacity;
				total = hundredsPlace + tensPlace + firstPlace;
				*displayBaseAddress = HEX3to0_display(total,table2);
				if(total == 0){
					t = 0;
					t = 0x40 << (16);
					t += 0x40 << (8);
					t += 0x40;
					*displayBaseAddress = t;
				}
				usleep(100000);

			if(typeOfCustomerVehicle == 0){
				if(*buttonAddress == 2 && sedanCapacity != 0){
					typeOfCustomerVehicle = 2;
					*displayBaseAddress = HEX3to0_display(sedanPrice,table);
					usleep(250000);
				}
				else if(*buttonAddress == 2 && sedanCapacity == 0){
					*displayBaseAddress = 0x3f;
					usleep(250000);
				}
				if(*buttonAddress == 4 && suvCapacity != 0){
					typeOfCustomerVehicle = 4;
					*displayBaseAddress = HEX3to0_display(suvPrice,table);
					usleep(250000);
				}
				else if(*buttonAddress == 4 && suvCapacity == 0){
					*displayBaseAddress = 0x3f;
					usleep(250000);
				}
				if(*buttonAddress == 8 && truckCapacity != 0){
					typeOfCustomerVehicle = 8;
					*displayBaseAddress = HEX3to0_display(truckPrice,table);
					usleep(250000);
				}
				else if(*buttonAddress == 8 && truckCapacity == 0){
					*displayBaseAddress = 0x3f;
					usleep(250000);
				}
			}
		}

		// Customer vehicle type has been set
		// Here is where they insert the bills
		if(typeOfCustomerVehicle == 2 && sedanCapacity != 0){
			int number = 0;
			while(*buttonAddress != 0){
				number = *buttonAddress;
				usleep(100000);
			}
			if(number != 0){
				tempBill = buttons_to_bills(number);
				paymentStartedFlag = 1;
				alt_alarm_stop(&alarm1);
				alt_alarm_start(&alarm1, 30 * alt_ticks_per_second(), alarm_callback, &myVar);
			}
			depositAmount += tempBill;
			*displayBaseAddress = HEX3to0_display(depositAmount,table);
			usleep(100000);
			number = 0;
			tempBill = 0;
			if(*switchAddress == 1 && depositAmount < sedanPrice){
				depositAmount = 0;
				*redLEDBaseAddress = 1;
				usleep(250000);
				*redLEDBaseAddress = 0;
				typeOfCustomerVehicle = 0;
				paymentStartedFlag = 0;
			}
			if(*switchAddress == 1 && depositAmount >= sedanPrice){
				changeDue = (depositAmount - sedanPrice);
				*displayBaseAddress = HEX3to0_display(changeDue,table);
				sedanCapacity--;
				*greenLEDBaseAddress = 128;
				usleep(250000);
				*greenLEDBaseAddress = 0;
				depositAmount = 0;
				typeOfCustomerVehicle = 0;
				paymentStartedFlag = 0;
			}
		}
		if(typeOfCustomerVehicle == 4 && suvCapacity != 0){
			int number = 0;
			while(*buttonAddress != 0){
				number = *buttonAddress;
				usleep(100000);
			}
			if(number != 0){
				tempBill = buttons_to_bills(number);
				paymentStartedFlag = 1;
				alt_alarm_stop(&alarm1);
				alt_alarm_start(&alarm1, 30 * alt_ticks_per_second(), alarm_callback, &myVar);
			}
			depositAmount += tempBill;
			*displayBaseAddress = HEX3to0_display(depositAmount,table);
			usleep(100000);
			number = 0;
			tempBill = 0;
			if(*switchAddress == 1 && depositAmount < suvPrice){
				depositAmount = 0;
				*redLEDBaseAddress = 1;
				usleep(250000);
				*redLEDBaseAddress = 0;
				typeOfCustomerVehicle = 0;
				paymentStartedFlag = 0;
			}
			if(*switchAddress == 1 && depositAmount >= suvPrice){
				changeDue = (depositAmount - suvPrice);
				*displayBaseAddress = HEX3to0_display(changeDue,table);
				suvCapacity--;
				*greenLEDBaseAddress = 128;
				usleep(250000);
				*greenLEDBaseAddress = 0;
				depositAmount = 0;
				typeOfCustomerVehicle = 0;
				paymentStartedFlag = 0;
			}
		}
		if(typeOfCustomerVehicle == 8 && truckCapacity != 0){
			int number = 0;
			while(*buttonAddress != 0){
				number = *buttonAddress;
				usleep(100000);
			}
			if(number != 0){
				tempBill = buttons_to_bills(number);
				paymentStartedFlag = 1;
				alt_alarm_stop(&alarm1);
				alt_alarm_start(&alarm1, 30 * alt_ticks_per_second(), alarm_callback, &myVar);
			}
			depositAmount += tempBill;
			*displayBaseAddress = HEX3to0_display(depositAmount,table);
			usleep(100000);
			number = 0;
			tempBill = 0;
			if(*switchAddress == 1 && depositAmount < truckPrice){
				depositAmount = 0;
				*redLEDBaseAddress = 1;
				usleep(250000);
				*redLEDBaseAddress = 0;
				typeOfCustomerVehicle = 0;
				paymentStartedFlag = 0;
			}
			if(*switchAddress == 1 && depositAmount >= truckPrice){
				changeDue = (depositAmount - truckPrice);
				*displayBaseAddress = HEX3to0_display(changeDue,table);
				truckCapacity--;
				*greenLEDBaseAddress = 128;
				usleep(250000);
				*greenLEDBaseAddress = 0;
				depositAmount = 0;
				typeOfCustomerVehicle = 0;
				paymentStartedFlag = 0;
			}
		}
	}
}

// Function to determine which bill is being inserted from certain combination of button presses
int buttons_to_bills(int number){
	if(number == 0){
		billInserted = 0;
	}
	else if(number == 2){
		billInserted = 1;
		*displayBaseAddress = HEX3to0_display(billInserted,table);
				usleep(250000);
	}
	else if(number == 4){
		billInserted = 2;
		*displayBaseAddress = HEX3to0_display(billInserted,table);
				usleep(250000);
	}
	else if(number == 6){
		billInserted = 5;
		*displayBaseAddress = HEX3to0_display(billInserted,table);
				usleep(250000);
	}
	else if(number == 8){
		billInserted = 10;
		*displayBaseAddress = HEX3to0_display(billInserted,table);
				usleep(250000);
	}
	else if(number == 10){
		billInserted = 20;
		*displayBaseAddress = HEX3to0_display(billInserted,table);
				usleep(250000);
	}
	else if(number == 12){
		billInserted = 50;
		*displayBaseAddress = HEX3to0_display(billInserted,table);
				usleep(250000);
	}
	else if(number == 14){
		billInserted = 100;
		*displayBaseAddress = HEX3to0_display(billInserted,table);
				usleep(250000);
	}
	alt_alarm_stop(&alarm1);
	alt_alarm_start(&alarm1, 30 * alt_ticks_per_second(), alarm_callback, &myVar);
	return billInserted;
}

// Function to display numbers on display
int HEX3to0_display (int value, unsigned char * table){
	int count = 0;
	int result = 0;
	while(value!=0){
		int remainder = value%10;
		int c = table[remainder] << (8*count);
		result += c;
		value = value/10;
		count++;
	}
	return result;
}

// Timer function for 30 seconds
alt_u32 alarm_callback(void *context){
    int *myVar = (int *) context;
    depositAmount = 0;
    *displayBaseAddress = HEX3to0_display(deposit_sum,table);
    *redLEDBaseAddress = 1;
	usleep(250000);
	*redLEDBaseAddress = 0;
	typeOfCustomerVehicle = 0;
	paymentStartedFlag = 0;
	alt_alarm_stop(&alarm1);
    return 30 * alt_ticks_per_second();
}

// Table of Buttons to Bills

//		U.S. Bill ($) | KEY 1 - KEY 3 | Decimal
//		1 			  | 001			  | 2
//		2 			  | 010			  |	4
//		5 			  | 011			  |	6
//		10 			  | 100			  |	8
//		20 			  | 101 		  |	10
//		50 			  | 110			  |	12
//		100 		  | 111			  |	14

