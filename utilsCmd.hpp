#include <Arduino.h>
#include <runner.hpp>

namespace runner
{
	struct StreamDump : Command {
		int8_t run(
			InterfaceBase * scope,
			String args[],
			Stream & input,
			Stream & out,
			Stream & err
		){
			Stream * in = &input;
			if(args[1].length()) {
				const auto link = scope->find<Stream>(args[1]);
				if(!link){
					err.println("argument not valid");
					return 1;
				}
				in = link->ref();
			}

			if(!in || in == &NullStream::dev){
				err.println("input not valid");
				return 1;
			}

			int current = 0;
			while(in->available()){
				char idx[9];
				sprintf(idx, "%6d: ", current);
				current+=8;
				out.print(idx);
				uint8_t data[9];
				data[8] = '\0';
				for(int i = 0; i < 8; i++){
					if(!in->available()){
						goto endloop;
					}
					char buff[5];
					data[i] = in->read();
					sprintf(buff, "%2x  ", (int) data[i]);
					out.print(buff);
				}
				out.println((char *) data);
			}
			if(!current){
				err.println("input not available");
			}
			endloop:
			return 0;
		}
	};

	struct FreeMemory : Command {
		int8_t run(
			InterfaceBase * scope,
			String args[],
			Stream & input,
			Stream & out,
			Stream &
		){
			int v = (int) ((int) &v - (int) (__brkval ?: &__heap_start));
			out.print(v);
			out.println('B');
			return v > 0;
		}
	};

	struct Echo : Command {
		int8_t run(
			InterfaceBase * scope,
			String args[],
			Stream & in,
			Stream & out,
			Stream &
		){
			auto s = StringStream(args + 1);
			while (s.available())
			{
				out.write(s.read());
			}
			return 0;
		}
	};

	struct Cat : Command {
		int8_t run(
			InterfaceBase * scope,
			String args[],
			Stream &,
			Stream & out,
			Stream &
		){
			auto s = scope->find<Stream>(args[1]);
			if(s == nullptr){
				return 1;
			}
			auto d = s->ref();
			while (d->available())
			{
				out.write(d->read());
			}
			return 0;
		}
	};	
}