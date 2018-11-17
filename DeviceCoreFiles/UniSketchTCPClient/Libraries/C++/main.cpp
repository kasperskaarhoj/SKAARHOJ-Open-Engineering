/* Skaarhoj/main.cppS
 * 20180608,10,12 - Philippe.Bekaert@uhasselt.be */

#define DEMO

#include "SkaarhojController.h"
#include "SkaarhojServer.h"

static const int nrimgs =7;
const std::string imgs[nrimgs*3] = {
  //# Button: Stop
  "AAAAAAAAAAAP////////8DAAAAAAAAAMJ////////+RP////////8l//+AAAf//6X//wAAA///pf//AAAD//+l//8AAAP//6X//wAAA///pf//AAAD8=",  
  "//pf//AAAD//+l//8AAAP//6X//wAAA///pf//AAAD//+l//8AAAP//6X//wAAA///pf//AAAD//+l//8AAAP//6X//wAAA///pf//AAAD//+l//8AA=",
  "AD//+l//8AAAP//6X//wAAA///pf//AAAD//+l//8AAAP//6X//4AAB///pP////////8if////////kMAAAAAAAAAwP////////8AAAAAAAAAAA",
  //# Button: Pause
  "AAAAAAAAAAAP////////8DAAAAAAAAAMJ////////+RP////////8l///4/x///6X///B+D///pf//4H4H//+l///gfgf//6X//+B+B///pf//4H4H8=",
  "//pf//4H4H//+l///gfgf//6X//+B+B///pf//4H4H//+l///gfgf//6X//+B+B///pf//4H4H//+l///gfgf//6X//+B+B///pf//4H4H//+l///gc=",
  "4H//+l///gfgf//6X//+B+B///pf//4H4H//+l///wfg///6X///j/H///pP////////8if////////kMAAAAAAAAAwP////////8AAAAAAAAAAA",
  //# Button: Repeat
  "AAAAAAAAAAAP////////8DAAAAAAAAAMJ////////+RP////////8l/////////6X////h////pf///wA///+l///+AB///6X///weH///pf//+H+/8=",
  "//pf//8P////+l///x//f//6X//+H/8///pf//4//h//+l///j/8H//6X//+P/wP//pf//4/+A//+l///h/4B//6X///H/4///pf//8P/H//+l///4c=",
  "+H//+l///8Hg///6X///4AH///pf///wA///+l////4f///6X/////////pP////////8if////////kMAAAAAAAAAwP////////8AAAAAAAAAAA",
  //# Button: Rew
  "AAAAAAAAAAAP////////8DAAAAAAAAAMJ////////+RP////////8l///8f/j//6X///h/8P//pf//8H/g//+l///gf8D//6X//8D/gf//pf//gf8D8=",
  "//pf//A/4H//+l//4H/A///6X//A/4H///pf/4H/A///+l//g/8H///6X/+D/wf///pf/4H/A///+l//wP+B///6X//gf8D///pf//A/4H//+l//+B8=",
  "8D//+l///A/4H//6X//+B/wP//pf//8H/g//+l///4f/D//6X///x/+P//pP////////8if////////kMAAAAAAAAAwP////////8AAAAAAAAAAA",
  //# Button: Connect
  "AAAAAAAAAAAP////////8DAAAAAAAAAMJ////////+RP////////8l/////x///6X////8D///pf////wH//+l/////Af//6X////8B///pf////AH8=",
  "//pf///8AH//+l///jBx///6X//8Af////pf//gH////+l//+A/////6X//4D/////pf//gH////+l///AH////6X//+MHH///pf///8AH//+l////8=",
  "AH//+l/////Af//6X////8B///pf////wH//+l/////A///6X/////H///pP////////8if////////kMAAAAAAAAAwP////////8AAAAAAAAAAA",
  //# Button: Play
  "AAAAAAAAAAAP////////8DAAAAAAAAAMJ////////+RP////////8l///5/////6X///B/////pf//8D////+l///wD////6X///AD////pf//8AH/8=",
  "//pf//8AB///+l///wAB///6X///AAB///pf//8AAD//+l///wAAD//6X///AAAP//pf//8AAD//+l///wAAf//6X///AAH///pf//8AB///+l///wA=",
  "H///+l///wA////6X///AP////pf//8D////+l///wf////6X///n/////pP////////8if////////kMAAAAAAAAAwP////////8AAAAAAAAAAA",
  //# Button: Delete
  "AAAAAAAAAAAP////////8DAAAAAAAAAMJ////////+RP////////8l///H/8f//6X//4P/g///pf//gf8D//+l//+A/gP//6X//4B8A///pf//wDgH8=",
  "//pf//4BAP//+l///wAB///6X///gAP///pf///AB///+l///+AP///6X///4A////pf///AB///+l///4AD///6X///AAH///pf//wDgH//+l//+Ac=",
  "wD//+l//+A/gP//6X//wH/Af//pf//A/+B//+l//8H/8H//6X//4//4///pP////////8if////////kMAAAAAAAAAwP////////8AAAAAAAAAAA"
};

class MySkaarhojController: public SkaarhojController {
protected:
  int nrHWCs {0};

public:
  MySkaarhojController(asio::io_context& io_context, asio::ip::tcp::socket socket_, bool verbose_)
    : SkaarhojController(io_context, std::move(socket_), verbose_)
  {
  }

  void setState(void)
  {
    std::cerr << "setState: " << nrHWCs << " HWCs.\n";
#ifdef DEMO    
    unsigned char bitmap [256];
    memset(bitmap, 0x55, 256);
    for (int id=1; id<nrHWCs; id++) {
      setGraphic(id, bitmap);
    }
#endif /*DEMO*/    
  }

  void map(unsigned clientHWC, unsigned serverHWC)
  {
    std::cerr << "map " << clientHWC << ":" << serverHWC << "\n";
    if (serverHWC > nrHWCs) nrHWCs = serverHWC;
#ifdef DEMO    
    if (clientHWC<=37) {
      unsigned char bitmap [256];
      memset(bitmap, 0x55, 256);
      setGraphic(serverHWC, bitmap);
    }
#endif /*DEMO*/        
  }

  void hwc(unsigned id, unsigned mask, const std::string& value)
  {
    std::cerr << "HWC id " << id << ", mask " << mask << ", value='" << value << "'\n";
#ifdef DEMO    
    if (id<42) {
      setColorIndex(id, mask<<1);
      setLabel(id, value, false, "HWC#" + std::to_string(id) + (mask>0 ? "." + std::to_string(mask) : ""));
      if (value == "Down")
	setOn(id);
      else if (value == "Up")
	setOff(id);
      else if (value == "Press")
	setDimmed(id);
    }

    if (value == "Up" && id>=10 && id<16) {
      static unsigned c =0;
      c = (c+1) % 4;
      setBars(c);
    }

    if (id==25) {
      int v;
      if (sscanf(value.c_str(), "Abs:%d", &v) != 1)
	error("Unexpected HWC25 value '" + value + "'");
      setStatus(24, Status::on, std::lround(15.*(float)v/1000.));
      setColorIndex(24, std::lround(15.*(float)v/1000.));
    }
#endif /*DEMO*/
  }

  void setBars(unsigned c)
  {
    unsigned col[6] = { (c<<4|   0|   0), (0   |c<<2|   0), (0   |   0|c<<0),
			(c<<4|c<<2|   0), (0   |c<<2|c<<0), (c<<4|   0|c<<0) };
    for (int i=0; i<6; i++) {
      setColorRGB(16+i, col[i]);
      setOn(16+i);
    }
  }
};

static void StartSkaarhojServer(void)
{
  SkaarhojServer<MySkaarhojController> server(true /*verbose*/);
  server.run();   // returns only when server is done, i.o.w. probably never
}

int main(int argc, char **argv)
{
  // In a real application, you will probably want to start to server in a
  // detached thread, so it runs concurrently with other tasks, instead of
  // waiting here until the server finishes (it never finishes unless there
  // are errors):
  //
  // std::thread t(StartSkaarhojServer);
  // t.detach();
  // ... do other work until program needs to close ...
  
  StartSkaarhojServer();
  
  return 0;
}
 
