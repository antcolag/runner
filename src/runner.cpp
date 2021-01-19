#include<runner.hpp>

namespace runner {
	template <typename T>
	const String type<T>::id = unknown;

	NullStream NullStream::dev = NullStream();

	RUNNER_REGISTER_TYPE(Stream);

	RUNNER_REGISTER_TYPE(Command);

	template<>
	bool Entry<void>::verify(EntryBase * other) {
		return true;
	}

	template<>
	String Entry<Command>::info() {
		return EntryBase::info() + ((Command *) ref())->type();
	}

	int8_t Shell::run() {
		String cmd = input.available() ? input.readStringUntil('\n') : "";
		if(!cmd.length()){
			return 0;
		}
		int argsStart = cmd.indexOf(' ');
		int i = 0;
		String rawArgs = cmd.substring(argsStart + 1, cmd.length());
		while(i < rawArgs.length()){
			switch(rawArgs.charAt(i++)){
				// case '|':	// |50 alloca un buffer di 50 byte e...?
				case '<':
				case '>':
				case '&':
				i-=2;
				goto done;
			}
		}

		done:

		String args [] = {
			argsStart > -1? cmd.substring(0, argsStart) : cmd,
			argsStart > -1 ? cmd.substring(argsStart + 1, argsStart + i + 1) : empty
		};

		Stream * ioe[] = {
			&input,
			&output,
			&error
		};
		char ids[] = "<>&";
		for(int c = 0; c < 3; c++){
			int f = rawArgs.indexOf(ids[c]);
			if(f > -1 ){
				auto curr = rawArgs.substring(f+1);
				int end = curr.indexOf(' ');
				auto ff = curr.substring(0, end < 0 ? curr.length() : end);
				auto tmp = scope.find<Stream>(ff);
				if(tmp){
					ioe[c] = tmp->ref();
				} else {
					error.print(ff);
					error.println(" not found");
				}
			}
		}

		static int8_t last = 0;
		auto cmdPtr = scope.find<Command>(args[0]);
		if(cmdPtr) {
			return last = cmdPtr->ref()->run(&scope, args, *ioe[0], *ioe[1], *ioe[2]);
		}
		if(args[0].equals("?")) {
			output.println(last);
			return 0;
		}
		error.println(args[0] + " not found");
		return last = -1;
	}

	void InterfaceBase::fire(
		String cmd,
		Stream & i,
		Stream & o,
		Stream & e
	) {
		int argsStart = cmd.indexOf(' ');
		String args [] = {
			argsStart > -1? cmd.substring(0, argsStart) : cmd,
			argsStart > -1 ? cmd.substring(argsStart + 1, cmd.length()) : empty
		};
		EntryBase * current = nullptr;
		do {
			if(current = find<Command>(args[0], current)){
				((Entry<Command> *)current)->ref()->run(this, args, i, o, e);
				current = current->next;
			}
		} while(current);
	};
}