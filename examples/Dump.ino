/**
 * Runner Dump StreamDump Example
 * 
 * In this file is shown the StreamDump command.
 * StreamDump prints the content of a stream in an hexadecimal table,
 * with the correspondig characters on the right side of each row.
 * 
 * This exapmple contains a very simple implementation of a stream from
 * the eeprom.
 * It is a mixin of Stream and EEPROM from Arduino.
 * 
 * You can run
 * 
 *   dump eeprom
 * 
 * to print the content of the eeprom
 */

#include "runner.hpp"
#include "utilsCmd.hpp"
#include<Arduino.h>
#include<EEPROM.h>

// build an interface to the runner library
runner::Interface os = runner::Interface();

// create a shell for the above interface
// by default is binded to the Serial stream,
// ie: os.shell(Serial, Serial, Serial);
runner::Shell shell = os.shell();

struct EasyEeprom;

void setup() {
	Serial.begin(9600);

	// run the shell on "loop" event
	shell.bind();

	os.add("eeprom", new EasyEeprom());
	os.add("dump", new runner::cmd::StreamDump());
}

void loop() {
	// trigger the "loop" event
	os.trigger(runner::loop);
}



// This is a very simple mixin of the Stream class and the EEPROMClass
struct StreamEeprom : Stream, EEPROMClass {
	unsigned current = 0;
	int available( ) {
		int free = (int) EEPROMClass::length() - (int) current;
		return free > 0 ? free : 0;
	}

	virtual void flush( ) {
		// to reset the pointr run
		// flush eeprom
		current = 0;
	}

	int peek( ) {
		return EEPROMClass::read(current);
	}

	int read( ){
		if(available( )){
			return EEPROMClass::read(current++);
		}
	}

	size_t write( uint8_t u_Data ){
		if(available( )){
			EEPROMClass::update(current++, u_Data);
			return 1;
		}
		return 0;
	}
};