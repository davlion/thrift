#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdio>
#include "TFifoTransport.h"
#include <sys/stat.h>
#include <iostream>

namespace apache {
namespace thrift {
namespace transport {

using namespace std;

bool 
TFifo::exists(const char* filePath)
{
	//The variable that holds the file information
	struct stat fileAtt; //the type stat and function stat have exactly the same names, so to refer the type, we put struct before it to indicate it is an structure.

	//Use the stat function to get the information
	if (stat(filePath, &fileAtt) != 0) //start will be 0 when it succeeds
        return false;
	  
	//S_ISREG is a macro to check if the filepath referers to a file. If you don't know what a macro is, it's ok, you can use S_ISREG as any other function, it 'returns' a bool.
	return S_ISFIFO(fileAtt.st_mode);
}

TFifo::TFifo(const std::string& base_path, bool is_server, std::shared_ptr<TConfiguration> config)
    : base_name_ (base_path)
    , is_server_ (is_server)
 {
    read_name_ = base_name_ + "_r";
    write_name_ = base_name_ + "_w";
    if (false == exists(read_name_.c_str())) {
        errno = 0;
        int result = ::mkfifo(read_name_.c_str(), 0666);
        if (0 != result) {
            cerr << "mkfifo failed " << read_name_.c_str() << result;
            throw FifoException("failed to mkfifo read");
        }//endif
    }//endif need to create read fifo
    if (false == exists(write_name_.c_str())) {
        int result = ::mkfifo(write_name_.c_str(), 0666);
        if (0 != result) {
            cerr << "mkfifo failed " << write_name_.c_str() << result;
            throw FifoException("mkfifo write failed");
        }//endif
    }//endif need to create write fifo
    open();
}//ct       

// Destroys the fifo object, closing it if necessary.
TFifo::~TFifo() {
    std::string doomed (read_name_);
    if (false == is_server_) {
        doomed = write_name_;
    }//endif client tries to remove write fifo
    int result = std::remove(doomed.c_str());
    if (0 != result) {
        throw(FifoException(doomed.c_str()));
    }//endif
}//dt

  // Returns whether the fifo is open & valid.
  bool 
  TFifo::isOpen() const {
      return ((0 != read_fd_) && (0 != write_fd_));
  }

  // Checks whether more data is available in the fifo.
  bool 
  TFifo::peek() {
      return true;
  }//peek

  // Creates and opens the named/anonymous fifo.
  void 
  TFifo::open() {
  if (true == is_server_) {
      if (0 == read_fd_) {
        read_fd_ = ::open(read_name_.c_str(), O_RDONLY);
        cout << "server opened read " << read_name_.c_str() << " fd is " << read_fd_ << endl;
      }//endif need to open read fifo for reading
      if (0 == write_fd_) {
        write_fd_ = ::open(write_name_.c_str(), O_WRONLY | O_NONBLOCK);
        cout << "server opened write " << write_name_.c_str() << " fd is " << write_fd_ << endl;
      }//endif need to open write fifo for writing
    }//endif we're the server
    else {
        if (0 == write_fd_) {
            write_fd_ = ::open(read_name_.c_str(), O_WRONLY | O_NONBLOCK);
            cout << "client opened write " << read_name_.c_str() << " fd is " << write_fd_ << endl;
        }// endif need to open read fifo for writing
        if (0 == read_fd_) {
            read_fd_ = ::open(write_name_.c_str(), O_RDONLY);
            cout << "client opened read " << write_name_.c_str() << " fd is " << read_fd_ << endl;
        }//endif need to open write fifo for reading
    }//else we're the client
  }//open

  // Shuts down communications on the fifo.
  void 
  TFifo::close() {
    if (0 != read_fd_) {
        ::close(read_fd_);
    }//endif need to close
    if (0 != write_fd_) {
        ::close(write_fd_);
    }//endif
  }//close

  // Reads from the fifo.
  uint32_t 
  TFifo::read(uint8_t* buf, uint32_t len) {
    if (!isOpen()) {
        open();
    }
    ssize_t bytes_read = ::read(read_fd_, buf, len);
    return bytes_read;
  }//read

  // Writes to the fifo.
  void 
  TFifo::write(const uint8_t* buf, uint32_t len) {
    if (!isOpen()) {
        open();
    }
    ssize_t bytes_written = ::write(write_fd_, buf, len);
  }//write


/**
 * Server transport framework. A server needs to have some facility for
 * creating base transports to read/write from.  The server is expected
 * to keep track of TTransport children that it creates for purposes of
 * controlling their lifetime.
 */
TServerFifo::TServerFifo(const std::string &base_path) 
    : base_path_ (base_path)
{
}   

bool 
TServerFifo::isOpen() const {
    return (NULL != fifo_.get());
}


void 
TServerFifo::close() {
    fifo_.get()->close();
    fifo_.reset();
}

std::shared_ptr<TTransport> 
TServerFifo::acceptImpl() {
    fifo_.reset(new TFifo(base_path_, true /*is server*/ ));
    return fifo_;
}

TServerFifo::~TServerFifo() {
    cout << "TServerFifo::dt" << endl;
}


}//ns transport
}//ns thrift
}//ns apache