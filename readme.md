Runner
===

This library provides an interface for handle commands and streams.

What problem does it resolve?
===
Sometimes, using Arduino, you feel that could be userfull to have a sort of shell executing some kind of command,
maybe handling input/output/error as stream lines like Serial (or some other serial stuff imported with a library like sd card reader of i2c).

This is the problem that this library solves.

It also implements a simple event interface based on the same command system,
so you can execute multiple commands when an event is triggered.

Usage
===

The following is an example of usage of this library along with some commands already defined in the header files provided by the library

```C++
#include "runner.hpp" // incudes Arduino.h for String and Serial
#include "ArduinoCmd.hpp" // Arduino functions mapped as commands
#include "utilsCmd.hpp" // FreeMemory, Info and other commands

runner::Interface os = runner::Interface();

runner::Shell shell = os.shell(); // input output and error are linked to Serial by default

void setup() {
	Serial.begin(9600);
	os.add("serial", &Serial); // register Serial as "serial"
	os.add("pm", new runner::cmd::PinModeCmd()); // add pinMode as command
	os.add("dw", new runner::cmd::DigitalWriteCmd()); // add digitalWrite as command
	os.add("free", new runner::cmd::FreeMemory()); // prints free memory info
	os.add("info", new runner::cmd::Info()); // prints all the added entries
	shell.bind(); // run the shell on an event (by defautl to the "loop" event)
	os.fire(runner::setup); // trigger the "setup" event
}

void loop() {
	os.fire(runner::loop); // trigger the "loop" event
}
```

When the above sketch is flashed on the Arduino board, it provides an interface on the Serial line the pm command for pin mode,
the dw command for digital write and a couple of utils commands to have some useful information about the system.

You can write on the serial interfce of Arduino ide `pm 13 1`, then `pm 13 1` and `pm 13 0` to make Arduino blink at your will, also you can write `free` to get info about free memory and `info` to have name and type of all the entries added to the the system

Define custom commands
---

In order to have a custom function you can extend the Command class or instanciate a FuncCommand class with a function reference as argument

```C++

#include "runner.hpp"

runner::Interface os = runner::Interface();

runner::Shell shell = os.shell(); // input output and error are linked to Serial by default

// extend the Command clsss
struct MyCmd : runner::Command {
	RUNNER_COMMAND(MyCmd) // adds String type() const
	int8_t run(
		runner::InterfaceBase * scope,
		String args[], // args[0] = cmd name, args[1] = argument string
		Stream & in,
		Stream & out,
		Stream & err
	){
		// do some stuff
		out.println("doing some cool stuff");
		return 0;
	}
};

MyCmd my{};

// use FuncCommand
auto my2 = new runner::FuncCommand([](
		runner::InterfaceBase * scope,
		String args[], // args[0] = cmd name, args[1] = argument string
		Stream & in,
		Stream & out,
		Stream & err
){
	out.println("doing some lambda stuff");
	return (int8_t) 0;
});

void setup() {
	Serial.begin(9600);

	os.add("foo", &my);
	os.add("bar", &my2);

	shell.bind(); // run the shell on an event (by defautl to the "loop" event)
	os.fire(runner::setup); // trigger the "setup" event
}

void loop() {
	os.fire(runner::loop); // trigger the "loop" event
}

// call foo and bar from Arduino ide
```
