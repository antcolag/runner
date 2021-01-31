#include "runner.hpp"
#include "ArduinoCmd.hpp"

// build an interface to the runner library
runner::Interface os = runner::Interface();

// create a shell for that interface
// by default is binded to the Serial stream,
// ie: os.shell(Serial, Serial, Serial);
runner::Shell shell = os.shell(); 

void setup() {
	Serial.begin(9600);
	shell.bind();

	// to call the following commands from the shell
	// type in the serial box of the arduino IDE
	// [command-name] [arguments] [redirects]
	// ie
	// analogRead 18 |4 analogWrite 3
	// the above command will read from A0 and write the value to the pin 3, using a 4 byte pipe
	os.add("pinMode", new runner::cmd::PinMode());
	os.add("digitalRead", new runner::cmd::DigitalRead());
	os.add("digitalWrite", new runner::cmd::DigitalWrite());
	os.add("analogRead", new runner::cmd::AnalogRead());
	os.add("analogWrite", new runner::cmd::AnalogWrite());
	os.add("tone", new runner::cmd::Tone());
}

void loop() {
	os.trigger(runner::loop);
}
