/**
 * Runner Status Example
 * 
 * In this file is shown how to make a command that
 * stores its status on the EEPROM
 * 
 * The example contains a custom command, MyCmd, that can have a status,
 * its status is the argument passed when the command is invoked,
 * ie
 * 
 *   my foo
 * 
 * will store the string foo in its myState property.
 * The status method of the MyCmd class will print the name of the command as
 * long as the myState string, concatenated, with a space between them,
 * that is the same `my foo` from above,
 * so that the current state can be saved and restored
 * 
 * To print the status on a Stream
 * 
 *   status > [Stream]
 * 
 * To restore the previous status
 * 
 *   sh < [Stream]
 * 
 * The implementation of the EEPROM stream in this example is very simple.
 * It is a mixin of Stream and EEPROMClass from Arduino.
 * The Arduino EEPROM could contain data that can not be parsed,
 * to avoid errors some utils like Wipe and Flush are registered
 * Wipe => write 0 on a stream while available
 * Flush => run the flush method of a stream
 * 
 * To store the status, run the following sequence
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
#include "runner.utils.hpp"
#include<Arduino.h>
#include<EEPROM.h>

// Build an interface to the runner library
runner::Interface os = runner::Interface();

// Create a shell for the above interface.
// By default a shell is attached to the Serial stream,
// ie: os.shell(Serial, Serial, Serial);
runner::Shell shell = os.shell();

// Custom command with the status method implemented
struct MyCmd : runner::Command {
	RUNNER_COMMAND(MyCmd) // adds String type() const method

	String myState;
	int8_t run(
		runner::Interface * scope,
		String args[], // args[0] = cmd name, args[1] = argument string
		Stream & in,
		Stream & out,
		Stream & err
	){
		// Store the status
		myState = args[1];
		out.println("doing some cool stuff");
		return 0;
	}

	void status(const String & name, Stream & o) const {
		// Print the command sequene to restore current status
		if(myState.length()){
			o.println(name + " " + myState);
		}
	}
};

MyCmd my{};


// This is a very simple mixin of the Stream class and the EEPROMClass
struct StreamEeprom : Stream, EEPROMClass {
	unsigned current = 0;
	int available( ) {
		int free = (int) EEPROMClass::length() - (int) current;
		return free > 0 ? free : 0;
	}

	virtual void flush( ) {
		// to reset the pointer, run
		// 
		//   flush eeprom
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


void setup() {
	Serial.begin(9600);

	// Run the shell on "loop" event
	shell.bind();
	os.add("my", my);
	os.add("eeprom", new StreamEeprom());
	os.add("flush", new runner::cmd::Flush());
	os.add("wipe", new runner::cmd::Wipe());
	os.add("status", new runner::cmd::Status());
	os.add("sh", new runner::cmd::Shell());
}

void loop() {
	// Trigger the "loop" event
	os.trigger(runner::loop);
}
