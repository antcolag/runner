#ifndef RUNNER_H
#define RUNNER_H

#include<Arduino.h>
#include<EEPROM.h>



namespace runner {
	const String setup = "setup";
	const String loop = "loop";
	const String empty = "";

	struct NullStream : public Stream {
		int available( ) {
			return 0;
		}

		void flush( ) {}

		int peek( ) {
			return 0;
		}

		int read( ){
			return 0;
		}

		size_t write( uint8_t u_Data ){
			return 0;
		}
	};

	Stream & Null = *new NullStream();

	template<typename T>
	struct NamedNode {
		static T tail;
		String const & name = empty;
		T & next = tail;
	};

	template<typename T>
	T NamedNode<T>::tail = T();

	struct Command {
		virtual const String type() const {
			return "";
		}

		virtual uint8_t run(
			String & args,
			Stream & i = Null,
			Stream & o = Null,
			Stream & e = Null
		) {
			return 0;
		}
		virtual void status(Stream & i) const {}
	};

	struct EEPROMStream : public NullStream {
		size_t current = 0;
		void reset() {
			current = 0;
		};
	};

	EEPROMStream eeprom = EEPROMStream();

	Stream & conf = eeprom;

	struct StreamEntry : NamedNode<StreamEntry> {

	};


	struct InterfaceBase {
		Command & CommandList = Command::tail;

		void add(const String & n, Command & cmd) const {
			cmd.name = n;
		};

		void add(const String & n, Command * cmd) {
			add(n, *cmd);
		};

		void add(const String & n, Stream & stream) const {};

		void add(const String & n, Stream * stream) {
			add(n, *stream);
		};

		int run(
			String cmd,
			Stream & i = Null,
			Stream & o = Null,
			Stream & e = Null
		) {
			//run first cmd
			return 1;
		};

		void trigger(
			String cmd,
			Stream & i = Null,
			Stream & o = Null,
			Stream & e = Null
		) {
			//run all cmd
		};
	};

	struct Shell {
		InterfaceBase & ctx;
		Stream & input;
		Stream & output;
		Stream & error;
		Shell(
			InterfaceBase & ctx,
			Stream & i = Null,
			Stream & o = Null,
			Stream & e = Null
		) :
			ctx(ctx),
			input(i),
			output(o),
			error(e)
		{};
		int run() {return 1;};
		void set(
			Stream & i = Null,
			Stream & o = Null,
			Stream & e = Null
		) {
			this->input = i;
			this->output = o;
			this->error = e;
		}

		void bind(const String evt = loop) {
			// ctx.add(evt, [&](String & s, Stream & i, Stream & o, Stream & e){
			// 	this->next();
			// });
		}
	};

	struct Interface : InterfaceBase {
		Shell shell(
			Stream & i = Null,
			Stream & o = Null,
			Stream & e = Null
		) {
			return Shell(*this, i, o, e);
		}
	};
}










#endif