#include <SDL2/SDL.h>
#include <iostream>
#include <cassert>
#include <string>
#include <sstream>

inline auto test_sdl_text() -> void {
  SDL_Init(SDL_INIT_VIDEO);
  auto string = std::string();
  auto running = true;
  auto window = SDL_CreateWindow("TestWindow", 500, 500, 970, 520, SDL_WINDOW_SHOWN);
  SDL_SetWindowBordered(window, SDL_TRUE);
  SDL_StartTextInput();
  while(running) {
    auto event = SDL_Event();
    while(SDL_PollEvent(&event)) {
      switch(event.type) {
        case SDL_QUIT: running = false; break;
        case SDL_KEYDOWN:
          if(event.key.keysym.sym == SDLK_BACKSPACE) {
            if(!string.empty()) string.pop_back();
            std::cout << string << std::endl;
          }
          break;
        case SDL_TEXTINPUT:
          string = string + std::string(event.text.text);
          std::cout << string << std::endl;
          break;  
        case SDL_TEXTEDITING:
          std::cout << string << std::endl;
          break;
      }
    }
  }

  SDL_Quit();
}