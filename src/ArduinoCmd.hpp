#include <runner.hpp>

namespace runner {
	namespace cmd {
		struct PinModeCmd : Command {

			RUNNER_COMMAND(PinModeCmd)

			int8_t run(
				InterfaceBase *,
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

		struct DigitalReadCmd : Command {

			RUNNER_COMMAND(DigitalReadCmd)

			int8_t run(
				InterfaceBase *,
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

		struct DigitalWriteCmd : Command {

			RUNNER_COMMAND(DigitalWriteCmd)

			int8_t run(
				InterfaceBase *,
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

		struct AnalogReadCmd : Command {

			RUNNER_COMMAND(AnalogReadCmd)

			int8_t run(
				InterfaceBase *,
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

		struct AnalogWriteCmd : Command {

			RUNNER_COMMAND(AnalogWriteCmd)

			int8_t run(
				InterfaceBase *,
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
	}
}