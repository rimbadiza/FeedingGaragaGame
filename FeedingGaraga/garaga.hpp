#include <SDL.h>
#include <deque>
#include "audio.hpp"

class garaga
{
public:
  garaga();
  ~garaga();
  int exec();
  bool tick();
  void draw();
  static const auto HeadOpenMouth = 0;
  static const auto Tail = 1;
  static const auto Turn = 2;
  static const auto Straight = 3;
  static const auto Head = 4;
  static const auto food = 5;
  const static auto Width = 1280;
  const static auto Height = 720;
private:
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *sprites;
  std::deque<std::pair<int, int> > segmentsList;
  unsigned ticks = 0;
  int dx = 1;
  int dy = 0;
  int foodX;
  int foodY;
  void generateFood();
  Audio effect;
};
