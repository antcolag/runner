#include<Arduino.h>

namespace runner {
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

	struct StringStream : public Stream, public String {
		int i = 0;
		StringStream(String * s) : String(*s) {}

		template<typename ...T>
		StringStream(T ...s) : String(s...) {}

		virtual int available( ) {
			return this->length() - i;
		}

		void flush( ) {}

		virtual int peek( ) {
			return this->charAt(i);
		}

		virtual int read( ){
			return this->charAt(i++);
		}

		virtual size_t write( uint8_t u_Data ){
			this->setCharAt(i++, u_Data);
			return 1;
		}
	};

	struct PipeBuffer : Stream {
		bool reading = false;
		int r = 0;
		int w = 0;
		int size;
		char * buffer;

		PipeBuffer(char * b, int s) : buffer(b), size(s) {}
		
		virtual int available( ) {
			return reading? w - r : size - w;
		}

		void flush( ) {
			r = 0;
			w = 0;
			auto s = 0;
			while(s - size) {
				buffer[s++] = 0;
			}
		}

		virtual int peek( ) {
			return buffer[r];
		}

		virtual int read( ){
			return buffer[r++];
		}

		virtual size_t write( uint8_t u_Data ){
			return w < size ? (buffer[w++] = u_Data), 1 : 0;
		}
	};
}