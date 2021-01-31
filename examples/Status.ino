/**
 * Runner Status Example
 * 
 * In this file is shown how to make a command that
 * stores its status on the EEPROM
 * 
 * To store the status run
 * 
 *   status > [Stream]
 * 
 * To restore the status run
 * 
 *   sh < [Stream]
 * 
 * The implementation of the eeprom stream in this example, is very simple.
 * It is a mixin of Stream and EEPROM from Arduino.
 * The arduino eeprom could containd data that can not de parsed,
 * to avoid errors you can use some other utils commands like Wipe and Flush
 * Wipe => write 0 on a stream while available
 * Flush => run the flush method of a stream
 * 
 * If you want to store the status, it could be useful to run this sequence
 * 
 *   flush eeprom
 *   status > eeprom
 *   wipe eeprom
 * 
 * To restore the status, use this sequence
 * 
 *   flush eeprom
 *   sh < eeprom
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

// custom command with the status method implemented
struct MyCmd : runner::Command {
	RUNNER_COMMAND(MyCmd) // adds String type() const method

	String last;
	int8_t run(
		runner::Interface * scope,
		String args[], // args[0] = cmd name, args[1] = argument string
		Stream & in,
		Stream & out,
		Stream & err
	){
		// store the status
		last = args[1];
		out.println("doing some cool stuff");
		return 0;
	}

	virtual void status(const String & name, Stream & o) const {
		// print the command sequene to restore current status
		if(last.length()){
			o.println(name + " " + last);
		}
	}
};

MyCmd my{};

struct EasyEeprom;

void setup() {
	Serial.begin(9600);

	// run the shell on "loop" event
	shell.bind();
	os.add("my", my);
	os.add("eeprom", new EasyEeprom());
	os.add("wipe", new runner::cmd::Flush());
	os.add("wipe", new runner::cmd::Wipe());
	os.add("sh", new runner::cmd::Shell());
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