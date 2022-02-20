#ifndef _THRIFT_TRANSPORT_TFIFO_H_
#define _THRIFT_TRANSPORT_TFIFO_H_ 1

#include <thrift/transport/TTransport.h>
#include <thrift/transport/TServerTransport.h>
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
  explicit TFifo(const std::string& base_path, bool is_server, std::shared_ptr<TConfiguration> config = nullptr);       

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
  std::string read_name_;
  std::string write_name_;

  bool exists(const char* filePath);
  int read_fd_;
  int write_fd_;
  bool is_server_;

};//class TFifo


/**
 * Server transport framework. A server needs to have some facility for
 * creating base transports to read/write from.  The server is expected
 * to keep track of TTransport children that it creates for purposes of
 * controlling their lifetime.
 */
class TServerFifo : public TServerTransport {
public:
  TServerFifo(const std::string &base_path);
  virtual ~TServerFifo() = default;

  virtual bool isOpen() const;

  std::shared_ptr<TTransport> accept() {
    std::shared_ptr<TTransport> result = acceptImpl();
    if (!result) {
      throw TTransportException("accept() may not return nullptr");
    }
    return result;
  }

  virtual void close();

protected:
  TServerFifo() = default;

  virtual std::shared_ptr<TTransport> acceptImpl();

private:
  std::string base_path_;
  std::shared_ptr<TFifo> fifo_;
}; //class TServerFifo



}// namespace transport
}// namespace thrift
}// namespace apache

#endif // #ifndef _THRIFT_TRANSPORT_TFIFO_H_
