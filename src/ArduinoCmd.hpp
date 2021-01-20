#include <runner.hpp>

namespace runner {
	namespace cmd {
		struct PinMode : Command {
			RUNNER_COMMAND(PinModeCmd)

			int8_t run(
				Interface *,
				String args[],
				Stream &,
				Stream & o,
				Stream &
			){
				auto s = StringStream(args + 1);
				int a = s.parseInt(), b = s.parseInt();
				pinMode(a, b);
				return 0;
			}
		};

		struct DigitalRead : Command {
			RUNNER_COMMAND(DigitalReadCmd)

			int8_t run(
				Interface *,
				String args[],
				Stream &,
				Stream & o,
				Stream &
			){
				auto s = StringStream(args + 1);
				int a = s.parseInt(), b = s.parseInt();
				o.println(digitalRead(a));
				return 0;
			}
		};

		struct DigitalWrite : Command {
			RUNNER_COMMAND(DigitalWriteCmd)

			int8_t run(
				Interface *,
				String args[],
				Stream &,
				Stream & o,
				Stream &
			){
				auto s = StringStream(args + 1);
				int a = s.parseInt(), b = s.parseInt();
				digitalWrite(a, b);
				return 0;
			}
		};

		struct AnalogRead : Command {
			RUNNER_COMMAND(AnalogReadCmd)

			int8_t run(
				Interface *,
				String args[],
				Stream &,
				Stream & o,
				Stream &
			){
				auto s = StringStream(args + 1);
				int a = s.parseInt();
				o.println(analogRead(a));
				return 0;
			}
		};

		struct AnalogWrite : Command {
			RUNNER_COMMAND(AnalogWriteCmd)

			int8_t run(
				Interface *,
				String args[],
				Stream &,
				Stream & o,
				Stream &
			){
				auto s = StringStream(args + 1);
				int a = s.parseInt(), b = s.parseInt();
				analogWrite(a, b);
				return 0;
			}
		};

		struct Tone : Command {
			RUNNER_COMMAND(AnalogWriteCmd)

			int8_t run(
				Interface *,
				String args[],
				Stream &,
				Stream & o,
				Stream &
			){
				auto s = StringStream(args + 1);
				int a = s.parseInt(), b = s.parseInt(), c = s.parseInt();
				tone(a, b, c);
				return 0;
			}
		};
	}
}