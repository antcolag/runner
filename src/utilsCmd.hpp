#include <Arduino.h>
#include <runner.hpp>

namespace runner
{
	namespace cmd {


		struct StreamDump : Command {
			RUNNER_COMMAND(StreamDump)

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
			RUNNER_COMMAND(FreeMemory)

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
			RUNNER_COMMAND(Echo)

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
				out.println();
				return 0;
			}
		};

		struct Cat : Command {
			RUNNER_COMMAND(Cat)

			int8_t run(
				InterfaceBase * scope,
				String args[],
				Stream & in,
				Stream & out,
				Stream &
			){
				Stream * d = &in;
				if(args[1].length()) {
					auto s = scope->find<Stream>(args[1]);
					if(s == nullptr){
						return -1;
					}
					d = s->ref();
				}
				while (d->available())
				{
					out.write(d->read());
				}
				return 0;
			}
		};

		struct Info : Command {
			RUNNER_COMMAND(Info)

			int8_t run(
				InterfaceBase * scope,
				String args[],
				Stream & in,
				Stream & out,
				Stream & err
			){
				if(args[1].length()) {
					auto ptr = scope->find(args[1]);
					if(ptr == nullptr){
						return -1;
					}
					out.println(ptr->info());
				} else {
					for(auto ptr = scope->modules; ptr != nullptr; ptr = ptr->next){
						out.println(ptr->info());
					}
				}
				return 0;
			}
		};

		struct Status : Command {
			RUNNER_COMMAND(Status)

			int8_t run(
				InterfaceBase * scope,
				String args[],
				Stream & in,
				Stream & out,
				Stream & err
			){
				if(args[1].length()) {
					auto ptr = scope->find<Command>(args[1]);
					if(ptr == nullptr){
						return -1;
					}
					ptr->ref()->status(*(ptr->name), out);
				} else {
					for(auto ptr = scope->modules; ptr != nullptr; ptr = ptr->next){
						if(Entry<Command>::verify(ptr)) {
							((Entry<Command> *) ptr)->ref()->status(*(ptr->name), out);
						}
					}
				}
				return 0;
			}
		};

		struct Trigger : Command {
			RUNNER_COMMAND(Trigger)

			int8_t run(
				InterfaceBase * scope,
				String args[],
				Stream & in,
				Stream & out,
				Stream & err
			){
				scope->fire(args[1], in, out, err);
				return 0;
			}
		};

		struct Flush : Command {
			RUNNER_COMMAND(Flush)

			int8_t run(
				InterfaceBase * scope,
				String args[],
				Stream & in,
				Stream & out,
				Stream & err
			){
				auto s = scope->find<Stream>(args[1]);
				if(s == nullptr){
					err.println(args[1] + " not found");
					return -1;
				}
				s->ref()->flush();
				return 0;
			}
		};
	}
}