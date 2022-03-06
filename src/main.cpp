#include <sciterjs/sciter-x.h>
#include <sciterjs/sciter-x-window.hpp>
#include <sciterjs/sciter-win-main.cpp>

class frame: public sciter::window {
public:
  frame() : window(SW_TITLEBAR | SW_RESIZEABLE | SW_CONTROLS | SW_MAIN | SW_ENABLE_DEBUG) {}

  // passport - lists native functions and properties exposed to script under 'frame' interface name:
  SOM_PASSPORT_BEGIN(frame)
    SOM_FUNCS(
      SOM_FUNC(nativeMessage)
    )
  SOM_PASSPORT_END

  // function expsed to script:
  sciter::string  nativeMessage() { return WSTR("Hello C++ World"); }

};

#include "resources.cpp" // resources packaged into binary blob.

int uimain(std::function<int()> run ) {

  sciter::archive::instance().open(aux::elements_of(resources)); // bind resources[] (defined in "resources.cpp") with the archive

  sciter::om::hasset<frame> pwin = new frame();

  // note: this:://app URL is dedicated to the sciter::archive content associated with the application
  pwin->load( WSTR("this://app/main.html") );
  //or use this to load UI from
  //  pwin->load( WSTR("file:///home/andrew/Desktop/Project/res/main.htm") );

  pwin->expand();

  return run();
}
