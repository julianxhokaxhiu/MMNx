document.ready = function() {
  // calling native method defined in main.cpp
  const thisWindow = Window.this;
  document.body.innerText = thisWindow.frame.nativeMessage();
}
