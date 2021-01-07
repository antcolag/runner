#include <runner.hpp>

namespace runner {
	struct PinModeCmd : Command {
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