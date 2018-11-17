/* SkaarhojServer.h: TCP server waiting from incoming connections
 * from Skaarhoj interface panels. For each incoming connection,
 * a SkaarhojController object is created and start()ed.
 * The SkaarhojController template class argument shall be a
 * child class of the SkaarhojController base class declared
 * in SkaarhojController.h, implementing desired functionality.
 * Philippe.Bekaert@azilpix.com - 20180608-18,20180910,11 */

#pragma once

#include <iostream>
#include <set>

// Using ASIO networking library, without boost.
// Download from https://think-async.com/
#define ASIO_STANDALONE
#include "asio.hpp"

//#define WITH_TIMER

template<class SkaarhojController>
class SkaarhojServer {
 public:
  SkaarhojServer(const SkaarhojServer&) =delete;
  SkaarhojServer& operator=(const SkaarhojServer&) =delete;

 // Constructor:
 // If verbose is true, various informational messages are generated.
 // If tick_msec > 0.0, a timer is started that ticks at the
 // indicated interval,  expressed in milliseconds. At every tick, the tick()
 // member function of all connected SkaarhojControllers is called. This
 // can be used, e.g., to synchronise controller with program state.
 // Note: the sync() function below provides a non-periodic-polling way to
 // sync controller and program state.
 SkaarhojServer(bool verbose_ =false, int tick_msec_ =0)
   : io_context(1)
    , acceptor(io_context)
    , tick_msec(tick_msec_)
    , timer(io_context)
    , verbose(verbose_)
  {
    // listen for incoming connections on any ethernet interface, port 9923.
    asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), 9923);
    acceptor.open(endpoint.protocol());
    acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
    acceptor.non_blocking(true);
    acceptor.bind(endpoint);
    acceptor.listen();    
    accept();

    if (tick_msec > 0) {
      timer.expires_after(std::chrono::milliseconds(tick_msec));
      tick();
    }
  }

  ~SkaarhojServer()
  {
    stop();
  }

  // call this to start waiting for incoming controller connections
  // and processing requests.
  void run(void)
  {
    io_context.run();    
  }

  // calls sync member function on all controllers, as to sync
  // controller displays with program state, e.g. after a state
  // change not through the controller(s). The controller sync()
  // member functions are called from the same thread that performs
  // networking, so there shall be no concurrency issues.
  void sync(void)
  {
    asio::post(
      [this]()
      {
	for (auto c: controllers)
	  c->sync();
      });
  }

  // set before accepting connections (that is: before run)
  bool verbose {false};
  
 protected:
  void accept(void)
  {
    if (verbose) info("SkaarhojServer waiting for incoming connections ...");
    acceptor.async_accept(
      [this](std::error_code ec, asio::ip::tcp::socket socket)
      {
	// Check whether the server was stopped by a signal before this
	// completion handler had a chance to run.
	if (!acceptor.is_open())
	  return;

	if (ec)
	  error("asio::accept error: " + ec.message());
	else
	  start(new SkaarhojController(io_context, std::move(socket), verbose));
	
	accept();
      });
  }

  void tick(void)
  {
    timer.async_wait(
      [this](const asio::error_code& ec)
      {
	if (ec)
	  error("asio::steady_timer error: " + ec.message());
	else
	  for (auto c : controllers)
	    c->tick();

	timer.expires_at(timer.expiry() + std::chrono::milliseconds(tick_msec));
	tick();
      });
  }

  asio::io_context io_context;
  asio::ip::tcp::acceptor acceptor;
  asio::steady_timer timer;
  int tick_msec {0};

  // accepted controller connections
  std::set<SkaarhojController*> controllers;

  void start(SkaarhojController* c)
  {
    controllers.insert(c);
    c->start();
  }
  
  void stop(void)
  {
    timer.cancel();
    acceptor.close();
    for (auto c: controllers) {
      delete c;
    }
    controllers.clear();
  }

  void info(const std::string& msg)
  {
#ifdef _ERROR_H_
    std::string cmsg = msg + "\n";
    Info(cmsg.c_str());
#else    
    std::cerr << msg << "\n";
#endif    
  }
  
  void error(const std::string& msg)
  {
#ifdef _ERROR_H_
    std::string cmsg = msg + "\n";
    Error(cmsg.c_str());
#else    
    std::cerr << msg << "\n";
#endif    
  }
};

