/**
 * Runner StreamDump command Example
 * 
 * In this file it is shown the StreamDump command.
 * StreamDump prints the content of a stream in an hexadecimal table,
 * with the correspondig characters on the right side of each row.
 * 
 * This exapmple contains a very simple implementation of a stream from
 * the EEPROM.
 * It is a mixin of Stream and EEPROMClass from Arduino.
 * 
 * You can run
 * 
 *   dump eeprom
 * 
 * to print the content of the EEPROM
 */

#include "runner.hpp"
#include "utilsCmd.hpp"
#include "StreamEeprom.h"

// Build an interface to the runner library
runner::Interface os = runner::Interface();

// Create a shell for the above interface.
// By default a shell is attached to the Serial stream,
// ie: os.shell(Serial, Serial, Serial);
runner::Shell shell = os.shell();

struct EasyEeprom;

void setup() {
	Serial.begin(9600);

	// Run the shell on "loop" event
	shell.bind();

	os.add("eeprom", new EasyEeprom());
	os.add("dump", new runner::cmd::StreamDump());
}

void loop() {
	// Trigger the "loop" event
	os.trigger(runner::loop);
}

