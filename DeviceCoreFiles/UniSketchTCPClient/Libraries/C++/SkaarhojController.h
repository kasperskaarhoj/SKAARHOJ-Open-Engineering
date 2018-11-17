/* SkaarhojController.h: Skaarhoj controller base class.
 * Create a child class of SkaarhojController to implement
 * functionality. Use that child class as template argument for
 * SkaarhojServer. 
 * Philippe.Bekaert@azilpix.com - 20180608-18, 20180909-12 */

#pragma once

#include <math.h>
#include <iostream>

// Using ASIO networking library, without boost.
// Download from https://think-async.com/
#define ASIO_STANDALONE
#include "asio.hpp"

class SkaarhojController {
 public:
  //////////////////////////////////////////////////////////////////
  // implement functionality by overriding the following functions
  // in your SkaarhojController child class.

  // Set panel state.
  // This function gets called in response to list messages from panel,
  // about once per minute.
  virtual void setState(void)
  {
  }

  // Handle map messages from panel. Use to find out what HWC
  // are available on the panel and to initialize them right after
  // connection is established. Graphics and text should be uploaded
  // first by this function. The serverHWC correcponds with the HWC
  // id of other functions, and is the one you need. The clientHWC
  // is currently not of much interest, but may be in future, according
  // to the skaarhoj dummy panel TCP interface documentation.
  virtual void map(unsigned clientHWC, unsigned serverHWC)
  {
  }

  // Handle HWC messages from panel.
  virtual void hwc(unsigned id, unsigned mask, const std::string& value)
  {
  }

  // posted by SkaarhojServer::sync() and scheduled among networking calls,
  // so no synchronisation issues.
  virtual void sync(void)
  {
  }

  // invoked periodically by the SkaarhojServer, if so requested during
  // SkaarhojServer construction. Again executed by the same thread as
  // the other functions here, so no synchronisation issues.
  virtual void tick(void)
  {
  }

  //////////////////////////////////////////////////////////////////
  // Commands to panel:
  
  // Set HWC status On/Off/Dimmed.
  // id: HWC id
  // status:
  // . bits 0-3 : 0=Off, 2=On (Red), 3=On (Green), 4=On (On color: white or yellow), 5 = Dimmed
  // . bit 4 : blink flag for monochrome button
  // . bit 5 : output bit (let follow On status: so, 1 if bits 0-3 are 2,3 or 4, and 0 otherwise)
  // . bit 8-11 : blink-bits. Determine blinking frequency and pattern.
  // mask (optional): 1,2,4,8 corresponding with four-way button edge, resp Up, Left, Down, Right.
  // Note: rack fusion live seems to refuse setting On status if mask is not 0.
  void setStatus(unsigned id, unsigned status, unsigned mask =0)
  {
    send("HWC#" + std::to_string(id) + (mask>0 ? "." + std::to_string(mask) : "") + "=" + std::to_string(status));
  }
  
  // More practical:
  enum class Status : unsigned {
    off    =0,
    red    =2,
    green  =3,
    on     =4,
    dimmed =5    
  };
  void setStatus(unsigned id, enum Status status, unsigned blinkbits =0, unsigned mask =0)
  {
    // let output follow status
    bool output = status==Status::red || status==Status::green || status==Status::on;
    // let monoblink follow blinkbits
    bool monoblink = blinkbits!=0;
    setStatus(id, (blinkbits<<8) | (output<<5) | (monoblink<<4) | ((unsigned)status&15), mask);
  }

  // Common cases:
  void setOff   (unsigned id) { setStatus(id, Status::off   ); }
  void setDimmed(unsigned id) { setStatus(id, Status::dimmed); }
  void setOn    (unsigned id) { setStatus(id, Status::on    ); }

  //////////////////////////////////////////////////////////////////
  // Set externally imposed button color: index or rrggbb.
  // id: HWC id
  // value:
  // . bit 7: enable bit (use this color if 1, use default of 0)
  // . bit 6: interpretation of bits 0-5: if 1, interpret as rrggbb, if 0,
  // interpret as preset color index.
  // . bits 0-5 : rrggbb value or color index value (depending on bit 6)
  void setColor(unsigned id, unsigned value)
  {
    send("HWCc#" + std::to_string(id) + "=" + std::to_string(value));
  }

  // More practical:
  void setColorIndex(unsigned id, unsigned index)
  {
    setColor(id, (1<<7) | (index&63));
  }
  void setColorRGB(unsigned id, unsigned rrggbb)
  {
    setColor(id, (1<<7) | (1<<6) | (rrggbb&63));
  }
  void setColorDefault(unsigned id)
  {
    setColor(id, 0); 
  }

  //////////////////////////////////////////////////////////////////
  // Display text. tokenized string:
  // Basic command, you will probably not want to use this directly.
  void setText(unsigned id, const std::string& value)
  {
    send("HWCt#" + std::to_string(id) + "=" + value.substr(0,63));
  }

  // More practical: composes 'value' for above:
  // value formats: defines how value is formatted
  enum class TextValueFormat : unsigned {
      integer  =0,
      float2   =1,
      percent  =2,
      decibel  =3,
      frames   =4,
      reciproc =5,
      kelvin   =6,
      hidden   =7,
      float3   =8
  };

  // values pair: 4 variations of boxing of value pairs
  enum class TextValuesPair : unsigned {
      nobox   =1,
      box1    =2,
      box2    =3,
      boxboth =4
  };
  
  // One value, with optional title, label and scale
  void setText(unsigned id,
	       short value,
	       TextValueFormat format =TextValueFormat::integer,
	       bool fine =false,
	       const std::string& title ="",
	       bool isLabel =false,
	       const std::string& label ="",
	       bool smallLabel =false,
	       short scale =0, short scaleRangeLow =0, short scaleRangeHigh =1,
	       short scaleLimitLow =0, short scaleLimitHigh =1)
  {
    setText(id,
	    std::to_string(value) +
	    "|" + std::to_string((unsigned)format) +
	    "|" + ((title.size()>0) ? std::to_string(fine) : "") +
	    "|" + title +
	    "|" + ((title.size()>0) ? std::to_string(isLabel) : "") +
	    "|" + label +
	    "||" + (smallLabel ? "0" : "") + "||" +
	    ((scale>0) ? (std::to_string(scale) + "|" + std::to_string(scaleRangeLow) + "|" + std::to_string(scaleRangeHigh) + "|" + std::to_string(scaleLimitLow) + "|" + std::to_string(scaleLimitHigh)) : ""));
  }

  // Two values, with optional title, labels, and scale
  void setText(unsigned id,
	       short value1, short value2,
	       TextValueFormat format =TextValueFormat::integer,
	       bool fine =false,
	       const std::string& title ="",
	       bool isLabel =false,
	       const std::string& label1 ="",
	       const std::string& label2 ="",
	       TextValuesPair boxing =TextValuesPair::nobox,
	       short scale =0, short scaleRangeLow =0, short scaleRangeHigh =1,
	       short scaleLimitLow =0, short scaleLimitHigh =1)
  {
    setText(id,
	    std::to_string(value1) +
	    "|" + std::to_string((unsigned)format) +
	    "|" + ((title.size()>0) ? std::to_string(fine) : "") +
	    "|" + title +
	    "|" + ((title.size()>0) ? std::to_string(isLabel) : "") +
	    "|" + label1 +
	    "|" + label2 +
	    "|" + std::to_string(value2) +
	    "|" + std::to_string((unsigned)boxing) +
	    ((scale>0) ? ("|" + std::to_string(scale) + "|" + std::to_string(scaleRangeLow) + "|" + std::to_string(scaleRangeHigh) + "|" + std::to_string(scaleLimitLow) + "|" + std::to_string(scaleLimitHigh)) : ""));
  }

  // Displays system stored media graphic file
  // Note: does not seem to do much on our rack fusion live
  void setSystemGraphic(unsigned id, unsigned img)
  {
    setText(id, "||||||||||||||" + std::to_string(img));
  }

  // Common cases:
  // Integer value, with optional title
  void setInteger(unsigned id, short value, const std::string& title ="", bool fine =false, bool isLabel =false)
  {
    setText(id, value, TextValueFormat::integer, fine, title, isLabel);
  }
  // Integer percent, dB, frames, reciprocal, Kelvin, with optional title
  void setInteger(unsigned id, short value, TextValueFormat format, const std::string& title ="", bool fine =false, bool isLabel =false)
  {
    setText(id, value, format, fine, title, isLabel);
  }
  // Floating point value, 2 decimal points, with optional title
  void setFloat2(unsigned id, float value, const std::string& title ="", bool fine =false, bool isLabel =false)
  {
    setText(id, std::lround(value*1000), TextValueFormat::float2, fine, title, isLabel);
  }
  // Floating point value, 3 decimal points, with optional title
  void setFloat3(unsigned id, float value, const std::string& title ="", bool fine =false, bool isLabel =false)
  {
    setText(id, std::lround(value*1000), TextValueFormat::float3, fine, title, isLabel);
  }
  // Just a title
  void setTitle(unsigned id, const std::string& title, bool fine =false, bool isLabel =false)
  {
    setText(id, 0, TextValueFormat::hidden, fine, title, isLabel);
  }
  // One label, with optional title
  void setLabel(unsigned id, const std::string& label, bool smallLabel =false, const std::string& title ="", bool fine =false, bool isLabel =false)
  {
    setText(id, 0, TextValueFormat::hidden, fine, title, isLabel, label, smallLabel);
  }

  //////////////////////////////////////////////////////////////////
  // Display graphic, 64x32 pixels
  // id: HWC id
  // encodedImage: 3 base64 strings, representing 86, 86 and 84 bits
  // of the 64x32 bitmap respectively.
  void setGraphic(unsigned id,
		     const std::string& encodedImagePart1,
		     const std::string& encodedImagePart2,
		     const std::string& encodedImagePart3)	     
  {
    send("HWCg#" + std::to_string(id) + "=0:" + encodedImagePart1);
    send("HWCg#" + std::to_string(id) + "=1:" + encodedImagePart2);
    send("HWCg#" + std::to_string(id) + "=2:" + encodedImagePart3);
  }

  // Same, but encoding a raw bitmap of 64x32 pixels.
  // bitmap: 64x32 bitmap, 256 (=64*32/8) bytes long.
  void setGraphic(unsigned id, const unsigned char bitmap [256])
  {
    send("HWCg#" + std::to_string(id) + "=0:" + base64(bitmap+  0, 86));
    send("HWCg#" + std::to_string(id) + "=1:" + base64(bitmap+ 86, 86));
    send("HWCg#" + std::to_string(id) + "=2:" + base64(bitmap+172, 84));
  }

  // utility: base64 encoding for bitmaps
  const std::string base64(const unsigned char* bytes, int nrbytes)
  {
    const char map[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    // Output consists of n groups of 4 bytes, n being nrbytes rounded
    // up to a multiple of 3. The last group is padded with '=' in case
    // nrbytes is not a multiple of 3.
    std::string result((nrbytes+2)/3*4, '=');
    char *out = &result[0];

    // Convert the first nrbytes/3 groups of 3 bytes
    const unsigned char* end = bytes+nrbytes/3*3;
    while (bytes<end) {
      out[0] = map[bytes[0]>>2];
      out[1] = map[(bytes[0]&0x03)<<4|bytes[1]>>4];
      out[2] = map[(bytes[1]&0x0f)<<2|bytes[2]>>6];
      out[3] = map[bytes[2]&0x3f];
      bytes += 3;
      out += 4;
    }

    // Convert the remaining bytes (0, 1 or 2)
    const int rest=nrbytes%3;
    if (rest == 1) {
      out[0] = map[bytes[0]>>2];
      out[1] = map[(bytes[0]&0x03)<<4];
    } else if (rest == 2) {
      out[0] = map[bytes[0]>>2];
      out[1] = map[(bytes[0]&0x03)<<4|bytes[1]>>4];
      out[2] = map[(bytes[1]&0x0f)<<2];
    }

    return result;
  }

  //////////////////////////////////////////////////////////////////
  // The following is called from the SkaarhojServer only:

  SkaarhojController(const SkaarhojController&) =delete;
  SkaarhojController& operator=(const SkaarhojController&) =delete;
  
  SkaarhojController(asio::io_context& io_context, asio::ip::tcp::socket socket_, bool verbose_)   
    : socket(std::move(socket_))
    , busy_timer(io_context)
    , verbose(verbose_)
  {
    info("controller connected");
  }

  virtual ~SkaarhojController()
  {
    stop();
  }
  
  void start(void)
  {
    read();   // start reading messages from the controller
  }
  
  void stop(void)
  {
    socket.close();
  }
    
  // Print messages for each command or request received or sent if true.
  bool verbose {false};

  // for printing info about this connection
  const std::string remoteAddress(void)
  {
    try {
      return socket.remote_endpoint().address().to_string();
    } catch (std::system_error) {
      return std::string("<diconnected>");
    }
  }
  
 protected:
  void read(void)
  {
    asio::async_read_until(socket, asio::dynamic_buffer(strbuf), '\n',
      [this](const asio::error_code& ec, std::size_t bytes_read)
      {
	if (ec) {
	  error("asio::async_read_until error: " + ec.message());
	  return;      // stop receiving messages from this connection
	}
	
	process(strbuf.substr(0, bytes_read-1));  // strip newline and process
	strbuf.erase(0, bytes_read);              // consume line
	
	read();
      });
  }

  // newline added automatically to end of str by send()
  asio::steady_timer busy_timer;
  std::string held_msg;
  void send(const std::string& str)
  {
    if (busy) {
      info("busy.");
#ifdef UNTESTED
      busy_timer.expires_after(std::chrono::milliseconds(100));
      held_msg = str;
      busy_timer.async_wait(
	[this](const asio::error_code& ec) {
	  if (ec)
	    error("asio::steady_timer error: " + ec.message());
	  else {
	    info("trying to send '" + held_msg + "' again.");
	    send(held_msg);
	  }
	});
      return;
#endif /*UNTESTED*/      
    }
    
    debug("sending " + str +  "...");
    socket.async_send(asio::buffer(str + "\n"),
      [this](const asio::error_code& ec, int n)
      {
	if (ec)
	  error("asio::async_send error: " + ec.message());
	debug("sent " + std::to_string(n) + " bytes");
      });
  }

  virtual void process(const std::string& line)
  {    
    debug("got '" + line + "'");
    if (line == "list") {
      send("ActivePanel=1");
      setState();
    } else if (line == "BSY") {
      busy = true;
    } else if (line == "RDY") {
      busy = false;
    } else if (line == "ping") {
      busy = false;
      send("ack");
    } else if (line.substr(0,3) == "map") {
      unsigned clientHWC, serverHWC;
      if (sscanf(line.c_str(), "map=%u:%u", &clientHWC, &serverHWC) == 2)
	map(clientHWC, serverHWC);
      else
	error("Malformed map message '" + line + "'.");
    } else if (line.substr(0,4) == "HWC#") {
      unsigned hwcid=0, mask=0, n=0;
      if (sscanf(line.c_str(), "HWC#%u.%u=%n", &hwcid, &mask, &n) == 2 ||
	  sscanf(line.c_str(), "HWC#%u=%n", &hwcid, &n) == 1) {
	hwc(hwcid, mask, line.substr(n));
      } else
      error("malformed HWC# message '" + line + "'.");
    } else {
      error("unrecognized message '" + line + "'.");
    }
  }

  asio::ip::tcp::socket socket;
  std::string strbuf;
  bool busy {false};
  
  ////////////////////////////////////////////////////////////////////////////
  virtual void error(const std::string& msg)
  {
    std::string cmsg = "SkaarhojController@" + remoteAddress() + ": " + msg + "\n";
#ifdef _ERROR_H_
    Error(cmsg.c_str());
#else
    std::cerr << cmsg;
#endif    
  }

  virtual void info(const std::string& msg)
  {
    std::string cmsg = "SkaarhojController@" + remoteAddress() + ": " + msg + "\n";
#ifdef _ERROR_H_
    Info(cmsg.c_str());
#else
    std::cerr << cmsg;
#endif
  }

  virtual void debug(const std::string& msg)
  {
    std::string cmsg = "SkaarhojController@" + remoteAddress() + ": " + msg + "\n";
#ifdef _ERROR_H_
    Debug(cmsg.c_str());
#else
    std::cerr << cmsg;
#endif    
  }
};
