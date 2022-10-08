#ifndef RUNNER_H
#define RUNNER_H

#include<Arduino.h>
#include<runner.streams.hpp>

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

	struct Version {
		uint8_t major;
		uint8_t minor;
		uint16_t patch;
	} const version {
		0, 2, 0
	};


	template <typename T>
	struct type {
		static const String id;
	};

	struct Interface;

	struct Command {
		virtual int8_t run (
			Interface *,
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

		static bool verify(EntryBase * other);

		String info(){
			return EntryBase::info() + *type();
		}
	};

	struct FuncCommand : Command {
		const String name;
		int8_t(*ptr) (
			Interface *,
			String[],
			Stream &,
			Stream &,
			Stream &
		);
		FuncCommand(
			int8_t(ptr) (
				Interface *,
				String[],
				Stream &,
				Stream &,
				Stream &
			),
			String name = "FuncCommand"
		) : ptr(ptr), name(name) {}
		int8_t run (
			Interface * scope,
			String args [],
			IOE_ARGS_ON(NullStream::dev)
		){
			return (*ptr)(scope, args, i, o, e);
		}

		String type() const {
			return name;
		}
	};

	struct Shell;

	struct Interface {
		EntryBase * modules = nullptr;

		void add(const String * name, Command * entry) {
			add(new Entry<Command>(name, entry));
		};

		void add(const String * name, Stream * entry) {
			add(new Entry<Stream>(name, entry));
		};

		void add(const String * name, Command & entry) {
			add(new Entry<Command>(name, &entry));
		};

		void add(const String * name, Stream & entry) {
			add(new Entry<Stream>(name, &entry));
		};

		template<typename T>
		void add(String & name, T && entry) {
			add(&name, entry);
		};

		template<typename T>
		void add(String && name, T && entry) {
			add(new String(name), entry);
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

		void trigger(
			String cmd,
			IOE_ARGS_ON(NullStream::dev)
		);

		void trigger(
			String cmd[],
			IOE_ARGS_ON(NullStream::dev)
		);

		template<typename T>
		void trigger(
			T cmd,
			String i,
			String o,
			String e
		) {
			Entry<Stream> * _i = find<Stream>(i);
			Entry<Stream> * _o = find<Stream>(o);
			Entry<Stream> * _e = find<Stream>(e);
			return trigger(
				cmd,
				_i ? *_i->ref() : NullStream::dev,
				_o ? *_o->ref() : NullStream::dev,
				_e ? *_e->ref() : NullStream::dev
			);
		}

		int8_t run(
			String const & cmd,
			IOE_ARGS_ON(NullStream::dev)
		);

		int8_t run(
			String const && cmd,
			IOE_ARGS_ON(NullStream::dev)
		) {
			return run(cmd, i, o, e);
		};

		int8_t run(
			String cmd[],
			IOE_ARGS_ON(NullStream::dev)
		);

		template<typename T>
		int8_t run(
			T cmd,
			String i,
			String o,
			String e
		) {
			Entry<Stream> * _i = find<Stream>(i);
			Entry<Stream> * _o = find<Stream>(o);
			Entry<Stream> * _e = find<Stream>(e);
			return run(
				cmd,
				_i ? *_i->ref() : NullStream::dev,
				_o ? *_o->ref() : NullStream::dev,
				_e ? *_e->ref() : NullStream::dev
			);
		}

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

		Shell shell(
			IOE_ARGS_ON(Serial)
		);

		Shell shell(
			void(*endSequence)(Shell *),
			IOE_ARGS_ON(Serial)
		);

		Shell shell(
			void(*startSequence)(Shell *),
			void(*endSequence)(Shell *),
			IOE_ARGS_ON(Serial)
		);
	};

	struct Shell {
		using ShellHandler = void (*)(Shell *);

		static ShellHandler defaultStartSequence;
		static ShellHandler defaultEndSequence;

		int8_t last = 0;
		Interface & scope;
		Stream & input;
		Stream & output;
		Stream & error;
		ShellHandler onStartCommand;
		ShellHandler onEndCommand;

		unsigned int count = 0;

		struct ShellRuntime : Command
		{
			RUNNER_COMMAND(ShellRuntime)

			Shell & shell;
			ShellRuntime(Shell & shell) : shell(shell) {}
			int8_t run (
				Interface *,
				String args [],
				IOE_ARGS_ON(NullStream::dev)
			){
				return shell.run();
			}
		};
		

		Shell(
			Interface & scope,
			ShellHandler startSequence = runner::Shell::defaultStartSequence,
			ShellHandler endSequence = runner::Shell::defaultEndSequence,
			IOE_ARGS_ON(NullStream::dev)
		) :
			scope(scope),
			onStartCommand(startSequence),
			onEndCommand(endSequence),
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

	int freeMem();
}
#endif
