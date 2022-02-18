#ifndef _THRIFT_TRANSPORT_TFIFO_H_
#define _THRIFT_TRANSPORT_TFIFO_H_ 1

#include <thrift/transport/TTransport.h>
#include <thrift/transport/TVirtualTransport.h>
#include <thrift/TNonCopyable.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <string_view>

namespace apache {
namespace thrift {
namespace transport {

/**
 * FIFO Transport
  */

class TFifo : public TVirtualTransport<TFifo> {
public:
  // Named fifo constructors -
  explicit TFifo(const std::string& base_path, std::shared_ptr<TConfiguration> config = nullptr);       

  // Destroys the fifo object, closing it if necessary.
  virtual ~TFifo();

  // Returns whether the fifo is open & valid.
  bool isOpen() const override;

  // Checks whether more data is available in the fifo.
  bool peek() override;

  // Creates and opens the named/anonymous fifo.
  void open() override;

  // Shuts down communications on the fifo.
  void close() override;

  // Reads from the fifo.
  virtual uint32_t read(uint8_t* buf, uint32_t len);

  // Writes to the fifo.
  virtual void write(const uint8_t* buf, uint32_t len);

  // Accessors
  const std::string& get_fifo_name() {return base_name_;}
private:
  std::string base_name_;

  int read_fd_;
  int write_fd_;
  bool owner_;

};//class

}// namespace transport
}// namespace thrift
}// namespace apache

#endif // #ifndef _THRIFT_TRANSPORT_TFIFO_H_
