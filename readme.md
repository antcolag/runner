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

Commands
===

Shipped commands
---

### Arduino

this commands maps the corresponding Arduino function

- **`PinMode`**
- **`DigitalRead`**
- **`DigitalWrite`**
- **`AnalogRead`**
- **`AnalogWrite`**
- **`Tone`**

### Utils

- **`StreamDump`** prints an hex dump of a stream, can be userfull for sd or eeprom inspection
- **`FreeMemory`** prints the amount of free memory
- **`Echo`** prints the argument
- **`Cat`** prints the content of a file
- **`Info`** prints the streams and commands registered in the system
- **`Status`** prints the sequence of commands for restore the current status*
- **`Trigger`** calls all commands with given name
- **`Flush`** invoke flush method on a stream

*Is available for custom commands that implements the `void status(const String &, Stream &) const` method. All the above methods are stateless

The following is an example of the whole command library included in a sketch

```C++
#include "runner.hpp"
#include "ArduinoCmd.hpp"
#include "utilsCmd.hpp"

runner::Interface os = runner::Interface();

runner::Shell mainShell = os.shell();

void setup() {
	Serial.begin(9600);
	os.add("serial", &Serial);
	os.add("pm", new runner::cmd::PinMode());
	os.add("dr", new runner::cmd::DigitalRead());
	os.add("dw", new runner::cmd::DigitalWrite());
	os.add("ar", new runner::cmd::AnalogRead());
	os.add("aw", new runner::cmd::AnalogWrite());
	os.add("to", new runner::cmd::Tone());
	os.add("dump", new runner::cmd::StreamDump());
	os.add("free", new runner::cmd::FreeMemory());
	os.add("echo", new runner::cmd::Echo());
	os.add("cat", new runner::cmd::Cat());
	os.add("info", new runner::cmd::Info());
	os.add("status", new runner::cmd::Status());
	os.add("trigger", new runner::cmd::Trigger());
	os.add("flush", new runner::cmd::Flush());
	os.fire(runner::setup);
	mainShell.bind();
}

void loop() {
	os.fire(runner::loop);
}
```

Anyway on an atmega328p system the above sketch is enough to fill the whole memory leaving only a couple of houndred of bytes free, is recommended indeed to use at least an Arduino mega for large sketch

Custom commands
---

In order to have a custom function you can extend the Command class or instanciate a FuncCommand class with a function reference as argument

```C++

#include "runner.hpp"

runner::Interface os = runner::Interface();

runner::Shell shell = os.shell(); // input output and error are linked to Serial by default

// extend the Command clsss
struct MyCmd : runner::Command {
	RUNNER_COMMAND(MyCmd) // adds String type() const

	String last;
	int8_t run(
		runner::Interface * scope,
		String args[], // args[0] = cmd name, args[1] = argument string
		Stream & in,
		Stream & out,
		Stream & err
	){
		// do some stuff
		last = args[1];
		out.println("doing some cool stuff");
		return 0;
	}

	void status(const String & name, Stream & o) const {
		o.println(name + " " + last); // how to restore current status
	}
};

MyCmd my{};

// use FuncCommand
auto my2 = new runner::FuncCommand([](
		runner::Interface * scope,
		String args[], // args[0] = cmd name, args[1] = argument string
		Stream & in,
		Stream & out,
		Stream & err
){
	out.println("doing some lambda stuff");
	return (int8_t) 0;
}, "MyFuncCmd");

void setup() {
	Serial.begin(9600);

	os.add("foo", &my);
	os.add("bar", &my2);

	// adding an instance of the following command will allows 
	// invokation from the shell of the status method of MyCmd
	os.add("status", new runner::cmd::Status());

	shell.bind(); // run the shell on an event (by defautl to the "loop" event)
	os.fire(runner::setup); // trigger the "setup" event
}

void loop() {
	os.fire(runner::loop); // trigger the "loop" event
}

// call foo and bar from Arduino ide
```

For `FuncCommand`s is not possible to save the status

Redirect input, output and error, and pipeline
---

Input, output and error redirection can be made from the shell using 
`<` for input, `>` for output and `&` for error.
For example it is possible to invoke a command from the shell in this way
```sh
echo hello world > serial
```

Is it possible to combine all redirection
```sh
cat > serial < serial2 & eeprom
```

It is also possible to combine multiple commands in a sequential pipeline using the symbol `|` followed by the size of the buffer shared among the commands

```sh
echo hello world |12 cat > serial
```

Contribution
===

Feel free to contribute by adding requests and pull requests, I will appreciate!