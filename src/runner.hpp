#ifndef RUNNER_H
#define RUNNER_H

#include<Arduino.h>

extern char __heap_start;
extern void *__brkval;

#define RUNNER_REGISTER_TYPE(T) \
template <> const String runner::type<T>::id = String(#T)

#define IOE_ARGS_ON(O) \
Stream & i = O,\
Stream & o = O,\
Stream & e = O


#define RUNNER_COMMAND(T) \
String type() const { \
	return #T; \
}

namespace runner {
	const String setup = "setup";
	const String loop = "loop";
	const String unknown = "unknown";
	const String empty = "";


	template <typename T>
	struct type {
		static const String id;
	};

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

	struct InterfaceBase;

	struct Command {
		virtual int8_t run (
			InterfaceBase *,
			String args [],
			IOE_ARGS_ON(NullStream::dev)
		) {
			return 0;
		}

		virtual void status(const String & name, Stream & o) const {}

		virtual RUNNER_COMMAND(unknown)
	};


	struct EntryBase {
		String const * name;
		EntryBase * next = nullptr;
		EntryBase(String const * name) : name(name) {}

		virtual const String * type() const {
			return &unknown;
		}

		virtual String info(){
			return *name  + '\t';
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
			return &runner::type<T>::id;
		}

		static bool verify(EntryBase * other){
			return runner::type<T>::id.equals(*other->type());
		}

		String info(){
			return EntryBase::info() + *type();
		}
	};

	struct FuncCommand : Command {
		RUNNER_COMMAND(FuncCommand)
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
		);

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


		struct ShellRuntime : Command
		{
			RUNNER_COMMAND(ShellRuntime)

			Shell & shell;
			ShellRuntime(Shell & shell) : shell(shell) {

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

		int8_t run();

		void set(
			IOE_ARGS_ON(NullStream::dev)
		) {
			input = i;
			output = o;
			error = e;
		}

		void bind(const String * evt = &runner::loop) {
			scope.add(evt, new ShellRuntime(*this));
		}
	};

	struct Interface : InterfaceBase {
		Shell shell(
			IOE_ARGS_ON(Serial)
		) {
			return Shell(*this, i, o, e);
		}
	};
}
#endif
