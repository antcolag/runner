/**
 * This example shows how to use the Arduino commands shipped
 * with the "ArduinoCmd.hpp" header file, from the runner library.
 * You can call the registered commands from the serial box of the Arduino IDE
 */


#include "runner.hpp"
#include "ArduinoCmd.hpp"

// Build an interface to the runner library
runner::Interface os = runner::Interface();

// Create a shell for the above interface.
// By default a shell is attached to the Serial stream,
// ie: os.shell(Serial, Serial, Serial);
runner::Shell shell = os.shell(); 

void setup() {
	Serial.begin(9600);

	// Run the shell on "loop" event
	shell.bind();

	// To call the following commands from the shell,
	// type in the serial box of the arduino IDE
	// [command-name] [arguments] [redirects]
	// 
	// ie
	// 
	//   pinMode 3 1
	//   analogRead 18 |4 analogWrite 3
	// 
	// The above command will read from A0 and then
	// write the value to the pin 3, using a 4 byte long pipe
	os.add("pinMode", new runner::cmd::PinMode());
	os.add("digitalRead", new runner::cmd::DigitalRead());
	os.add("digitalWrite", new runner::cmd::DigitalWrite());
	os.add("analogRead", new runner::cmd::AnalogRead());
	os.add("analogWrite", new runner::cmd::AnalogWrite());
	os.add("tone", new runner::cmd::Tone());
}

void loop() {
	// Trigger the "loop" event
	os.trigger(runner::loop);
}
