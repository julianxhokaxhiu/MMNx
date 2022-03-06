#include <filesystem>
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
  sciter::string  nativeMessage() { return WSTR("Hello World"); }
};

int uimain(std::function<int()> run ) {
  // Window ui file path
  std::filesystem::path uiPath = "file:///" / std::filesystem::current_path() / "ui/main.html";

  // Sciter window constructor
  sciter::om::hasset<frame> pWin = new frame();
  pWin->load(uiPath.wstring().c_str());
  pWin->expand();

  return run();
}
