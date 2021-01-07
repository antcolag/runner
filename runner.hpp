#ifndef RUNNER_H
#define RUNNER_H

#include<Arduino.h>

extern char __heap_start;
extern void *__brkval;

#define RUNNER_REGISTER_TYPE(T) \
template <> const String runner::type<T>::name = String(#T)

#define IOE_ARGS_ON(O) \
Stream & i = O,\
Stream & o = O,\
Stream & e = O

namespace runner {
	const String setup = "setup";
	const String loop = "loop";
	const String unknown = "unknown";
	const String empty = "";


	template <typename T>
	struct type {
		static const String name;
	};

	template <typename T>
	const String type<T>::name = unknown;

	RUNNER_REGISTER_TYPE(Stream);

	struct NullStream : Stream {
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

		static NullStream dev;
	};

	NullStream NullStream::dev = NullStream();

	struct StringStream : Stream {
		String * ptr;
		int i = 0;
		StringStream(String * s) : ptr(s) {}

		int available( ) {
			return ptr->length() - i;
		}

		void flush( ) {}

		int peek( ) {
			return ptr->charAt(i);
		}

		int read( ){
			return ptr->charAt(i++);
		}

		size_t write( uint8_t u_Data ){
			ptr->setCharAt(i++, u_Data);
		}
	};

	struct EntryBase {
		String const * name;
		EntryBase * next = nullptr;
		EntryBase(String const * name) : name(name) {}

		virtual const String * type() const {
			return &unknown;
		}
	};

	template<typename T>
	struct Entry : EntryBase {
		Entry(
			String const * name, T * payload
		) : EntryBase(name), payload(payload) {}

		T * payload;

		T * ref(){
			return payload;
		}

		const String * type() const {
			return &runner::type<T>::name;
		}

		static bool verify(EntryBase * other){
			return runner::type<T>::name.equals(*other->type());
		}
	};

	template<>
	bool Entry<void>::verify(EntryBase * other) {
		return true;
	}

	struct InterfaceBase;

	struct Command {
		virtual int8_t run (
			InterfaceBase *,
			String args [],
			IOE_ARGS_ON(NullStream::dev)
		) {
			return 0;
		}
		virtual void status(String & name, Stream & o) const {}
	};

	RUNNER_REGISTER_TYPE(Command);

	struct FuncCommand : Command {
		int(*ptr) (InterfaceBase *, String[], Stream &, Stream &, Stream &);
		FuncCommand(int(ptr) (InterfaceBase *, String[], Stream &, Stream &, Stream &)) : ptr(ptr) {}
		int8_t run (
			InterfaceBase * scope,
			String args [],
			IOE_ARGS_ON(NullStream::dev)
		){
			return (*ptr)(scope, args, i, o, e);
		}
	};

	struct InterfaceBase {
		EntryBase * modules = nullptr;

		void add(const String * name, Command * entry) {
			add(new Entry<Command>(name, entry));
		};

		void add(const String * name, Stream * entry) {
			add(new Entry<Stream>(name, entry));
		};

		template<typename T>
		void add(const char * name, T && entry) {
			add(new String(name), entry);
		};

		void add(EntryBase * entry) {
			EntryBase * head = modules;
			modules = entry;
			modules->next = head;
		};

		void fire(
			String cmd,
			IOE_ARGS_ON(NullStream::dev)
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

		template<typename T = void>
		Entry<T> * find(String name, EntryBase * entry = nullptr) const {
			entry = entry == nullptr ? modules : entry;

			while(entry != nullptr){
				if(entry->name->equals(name) && Entry<T>::verify(entry)) {
					break;
				} else {
					entry = entry->next;
				}
			}
			return (Entry<T> *) entry;
		}
	};

	struct Shell {
		InterfaceBase & scope;
		Stream & input;
		Stream & output;
		Stream & error;


		struct ShellCommand : Command
		{
			Shell & shell;
			ShellCommand(Shell & shell) : shell(shell) {

			}
			int8_t run (
				InterfaceBase *,
				String args [],
				IOE_ARGS_ON(NullStream::dev)
			){
				return shell.run();
			}
		};
		

		Shell(
			InterfaceBase & scope,
			IOE_ARGS_ON(NullStream::dev)
		) :
			scope(scope),
			input(i),
			output(o),
			error(e)
		{};
		int8_t run() {
			String cmd = input.available() ? input.readStringUntil('\n') : "";
			if(!cmd.length()){
				return 0;
			}
			int argsStart = cmd.indexOf(' ');
			String args [] = {
				argsStart > -1? cmd.substring(0, argsStart) : cmd,
				argsStart > -1 ? cmd.substring(argsStart + 1, cmd.length()) : empty
			};
			static int8_t last = 0;
			auto cmdPtr = scope.find<Command>(args[0]);
			if(cmdPtr) {
				return last = cmdPtr->ref()->run(&scope, args, this->input, this->output, this->error);
			}
			if(args[0].equals("?")) {
				this->output.println(last);
				return 0;
			}
			this->output.println(args[0] + " not found");
			return last = -1;
		};
		void set(
			IOE_ARGS_ON(NullStream::dev)
		) {
			this->input = i;
			this->output = o;
			this->error = e;
		}

		void bind(const String * evt = &runner::loop) {
			scope.add(evt, new ShellCommand(*this));
		}
	};

	struct Interface : InterfaceBase {
		Shell shell(
			IOE_ARGS_ON(NullStream::dev)
		) {
			return Shell(*this, i, o, e);
		}
	};
}


#endif