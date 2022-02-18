#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdio>
#include "TFifoTransport.h"

namespace apache {
namespace thrift {
namespace transport {

TFifo::TFifo(const std::string& base_path, std::shared_ptr<TConfiguration> config)
    : base_name_ (base_path)
    , owner_ (false)
 {
     std::string read_name {base_name_};
     std::string write_name {base_name_};
     read_name += "_r";
     write_name += "_w";
     if (0 == ::open(read_name.c_str(), 'r')) {
         int result = ::mkfifo(read_name.c_str(), S_IRWXU);
         if (0 == result) {
             result = ::mkfifo(write_name.c_str(), S_IRWXU);
         }//endif happy
         if (0 != result) {
             throw(std::exception());
         }//endif unhappy
         owner_ = true;
     }//endif need to make pipes

}//ct       

// Destroys the fifo object, closing it if necessary.
TFifo::~TFifo() {
    if (true == owner_) {
         std::string read_name {base_name_};
         std::string write_name {base_name_};
        read_name += "_r";
        write_name += "_w";
        int result = std::remove(read_name.c_str());
        if (0 == result) {
            result = std::remove(write_name.c_str());
        }
        if (0 != result) {
            throw(std::exception());
        }//
    }//endif need to remove fifos
}//dt

  // Returns whether the fifo is open & valid.
  bool 
  TFifo::isOpen() const {
      return (0 != read_fd_) && (0 != write_fd_);
  }

  // Checks whether more data is available in the fifo.
  bool 
  TFifo::peek() {
      return true;
  }//peek

  // Creates and opens the named/anonymous fifo.
  void 
  TFifo::open() {
  std::string read_name {base_name_};
  std::string write_name {base_name_};
  read_name += "_r";
  write_name += "_w";
  if (true == owner_) {
        read_fd_ = ::open(read_name.c_str(), 'r');
        write_fd_ = ::open(write_name.c_str(), 'w');
    }//endif
    else {
        read_fd_ = ::open(write_name.c_str(), 'r');
        write_fd_ = ::open(read_name.c_str(), 'w');
    }//else
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
      return 0;
  }

  // Writes to the fifo.
  void 
  TFifo::write(const uint8_t* buf, uint32_t len) {
      
  }

}//ns transport
}//ns thrift
}//ns apache