#include<runner.hpp>

namespace {
	int getStartIoe(String & rawcmd, char ids[]){
		int min = rawcmd.length();
		for(; *ids; ids++){
			int c = rawcmd.indexOf(*ids);
			min = c > 0 && c < min ? c : min;
		}
		return min;
	}

	bool fillIoe(
		String & ioeArgs,
		runner::Interface & scope,
		Stream & error,
		char ids[],
		Stream * ioe[]
	) {
		for(int c = 0; ids[c]; c++){
			int f = ioeArgs.indexOf(ids[c]);
			if(f > -1 ){
				auto curr = ioeArgs.substring(f + 1, ioeArgs.length());
				curr.trim();
				int end = curr.indexOf(' ');
				auto ff = curr.substring(0, end < 0 ? curr.length() : end);
				auto tmp = scope.find<Stream>(ff);
				if(tmp){
					ioe[c] = tmp->ref();
				} else {
					error.print(ff);
					error.println(F(" not found"));
					return false;
				}
			}
		}
		return true;
	}

	int8_t pipeline(
		runner::Interface & scope,
		String & cmd,
		Stream * i,
		Stream * o,
		Stream * e
	){
		auto pipestart = cmd.indexOf('|');
		if(pipestart >= 0){
			pipestart++;
			auto s = cmd.substring(pipestart).toInt();
			if(s > runner::freeMem()){
				e->print(F("not enough memory"));
				return -1;
			}
			char buffer[s];
			runner::PipeBuffer pipe(buffer, s);
			pipe.flush();
			auto curr = cmd.substring(0, pipestart - 1);
			scope.run(curr, *i, pipe, *e);
			pipe.reading = true;
			cmd = cmd.substring(pipestart + String(s).length());
			cmd.trim();
			return pipeline(scope, cmd, &pipe, o, e);
		} else {
			return scope.run(cmd, *i, *o, *e);
		}
	}
}

namespace runner {
	template <typename T>
	const String type<T>::id = unknown;

	NullStream NullStream::dev = NullStream();

	RUNNER_REGISTER_TYPE(Stream);

	RUNNER_REGISTER_TYPE(Command);

	template<typename T>
	bool Entry<T>::verify(EntryBase * other) {
		return runner::type<T>::id.equals(*other->type());
	}

	template<>
	bool Entry<void>::verify(EntryBase * other) {
		return true;
	}

	template<>
	String Entry<Command>::info() {
		return EntryBase::info() + ((Command *) ref())->type();
	}

	void Interface::trigger(
		String cmd,
		Stream & i,
		Stream & o,
		Stream & e
	) {
		cmd.trim();
		int argsStart = cmd.indexOf(' ');
		String args [] = {
			argsStart > -1? cmd.substring(0, argsStart) : cmd,
			argsStart > -1 ? cmd.substring(argsStart + 1, cmd.length()) : empty
		};
		return trigger(args, i, o, e);
	};

	void Interface::trigger(
		String args[],
		Stream & i,
		Stream & o,
		Stream & e
	) {
		EntryBase * current = nullptr;
		do {
			if(current = find<Command>(args[0], current)){
				((Entry<Command> *)current)->ref()->run(this, args, i, o, e);
				current = current->next;
			}
		} while(current);
	}

	int8_t Interface::run(
		String const & cmd,
		Stream & i,
		Stream & o,
		Stream & e
	) {
		int argsStart = cmd.indexOf(' ');
		String args [] = {
			argsStart > -1 ? cmd.substring(0, argsStart) : cmd,
			argsStart > -1 ? cmd.substring(argsStart + 1, cmd.length()) : empty
		};
		return run(args, i, o, e);
	};

	int8_t Interface::run(
		String args[],
		Stream & i,
		Stream & o,
		Stream & e
	) {
		EntryBase * current = nullptr;
		if(current = find<Command>(args[0], current)){
			return ((Entry<Command> *)current)->ref()->run(this, args, i, o, e);
		}
		e.println(args[0] + F(" not found"));
		return -1;
	};

	Shell Interface::shell(
		Stream & i,
		Stream & o,
		Stream & e
	) {
		return Shell(*this, i, o, e);
	}

	int freeMem() {
		int v = (int) ((int) &v - (int) (__brkval ?: &__heap_start));
		return v;
	}

	int8_t Shell::run() {
		while(input.available() && input.peek()) {
			String rawcmd = input.readStringUntil('\n');
			if(!rawcmd.length()){
				return 0;
			}
			char ids[] = "<>&";
			int i = ::getStartIoe(rawcmd, ids);
			String cmd = rawcmd.substring(0, i);
			String ioeArgs = rawcmd.substring(i, rawcmd.length());
			Stream * ioe[] = {
				&input,
				&output,
				&error
			};
			if(!::fillIoe(ioeArgs, scope, error, ids, ioe)) {
				return -1;
			}

			if(cmd.equals(F("?"))) {
				ioe[1]->println(last);
				return 0;
			}
			

			last = ::pipeline(scope, cmd, ioe[0], ioe[1], ioe[2]);
			output.print(this->endSequence);
		}

		return last;
	}
}