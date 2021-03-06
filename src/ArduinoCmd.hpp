#include <runner.hpp>

namespace runner {
	namespace cmd {
		struct PinMode : Command {
			RUNNER_COMMAND(PinMode)

			int8_t run(
				Interface *,
				String args[],
				Stream &,
				Stream & o,
				Stream &
			){
				auto s = StringStream(args + 1);
				uint8_t pin = s.parseInt(), value = s.parseInt();
				pinMode(pin, value);
				return 0;
			}
		};

		struct DigitalRead : Command {
			RUNNER_COMMAND(DigitalRead)

			int8_t run(
				Interface *,
				String args[],
				Stream &,
				Stream & o,
				Stream &
			){
				auto s = StringStream(args + 1);
				uint8_t pin = s.parseInt();
				o.println(digitalRead(pin));
				return 0;
			}
		};

		struct DigitalWrite : Command {
			RUNNER_COMMAND(DigitalWrite)

			int8_t run(
				Interface *,
				String args[],
				Stream & i,
				Stream & o,
				Stream &
			){
				auto s = StringStream(args + 1);
				int pin = s.parseInt(), value;
				value = s.available() ? s.parseInt() : i.parseInt();
				digitalWrite(pin, value);
				return 0;
			}
		};

		struct AnalogRead : Command {
			RUNNER_COMMAND(AnalogRead)

			int8_t run(
				Interface *,
				String args[],
				Stream &,
				Stream & o,
				Stream &
			){
				auto s = StringStream(args + 1);
				int pin = s.parseInt();
				o.println(analogRead(pin));
				return 0;
			}
		};

		struct AnalogWrite : Command {
			RUNNER_COMMAND(AnalogWrite)

			int8_t run(
				Interface *,
				String args[],
				Stream & i,
				Stream & o,
				Stream &
			){
				auto s = StringStream(args + 1);
				int pin = s.parseInt(), value;
				value = s.available() ? s.parseInt() : i.parseInt();
				analogWrite(pin, value);
				o.println(value);
				return 0;
			}
		};

		struct Tone : Command {
			RUNNER_COMMAND(Tone)

			int8_t run(
				Interface *,
				String args[],
				Stream & i,
				Stream & o,
				Stream &
			){
				auto s = StringStream(args + 1);
				uint8_t pin = s.parseInt();
				unsigned frequency = s.parseInt(), duration;
				if(s.available()){
					duration = s.parseInt();
				} else {
					duration = frequency;
					frequency = i.parseInt();
				}
				tone(pin, frequency, duration);
				return 0;
			}
		};
	}
}