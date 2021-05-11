#include "garaga.hpp"
#include "sdl_check.hpp"

garaga::garaga()
{
    
  auto res = SDL_Init(SDL_INIT_EVERYTHING);
  SDL_Init(SDL_INIT_EVERYTHING);
  SDL_CHECK(res == 0, "SDL_Init");
  SDL_CreateWindowAndRenderer(Width, Height, SDL_WINDOW_BORDERLESS, &window, &renderer);
  SDL_CHECK(window, "SDL_CreateWindowAndRenderer");
  SDL_CHECK(renderer, "SDL_CreateWindowAndRenderer");
  SDL_SetWindowPosition(window, 65, 126);
  auto surface = SDL_LoadBMP("sprites.bmp");
  SDL_CHECK(surface, "SDL_LoadBMP(\"sprites.bmp\")");
  sprites = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_CHECK(sprites, "SDL_CreateTextureFromSurface");
  SDL_FreeSurface(surface); 
  effect.load("garaga.wav");
  effect.play();
  effect.load("eat.wav");

  segmentsList.push_back(std::make_pair(5, 5));
  segmentsList.push_back(std::make_pair(5, 6));
  segmentsList.push_back(std::make_pair(4, 6));
  generateFood();
}

void garaga::generateFood()
{
  auto done = false;
  do
  {
    
    foodX = rand() % (Width / 64);
    foodY = rand() % (Height / 64);
    done = true;
    for (const auto &segment: segmentsList)
    {
      if (foodX == segment.first && foodY == segment.second)
      {
        done = false;
        break;
      }
    }effect.play();
  } while (!done);
}


garaga::~garaga()
{
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

int garaga::exec()
{
  auto oldTick = SDL_GetTicks();
  for (auto done = false; !done;)
  {
    SDL_Event e;
    if (SDL_PollEvent(&e))
    {
      switch (e.type)
      {
      case SDL_KEYDOWN:
        {
          switch (e.key.keysym.sym)
          {
          case SDLK_UP:
            dx = 0;
            dy = -1;
            break;
          case SDLK_RIGHT:
            dx = 1;
            dy = 0;
            break;
          case SDLK_DOWN:
            dx = 0;
            dy = 1;
            break;
          case SDLK_LEFT:
            dx = -1;
            dy = 0;
            break;
          }
        }
        break;
      case SDL_QUIT:
        done = true;
        break;
      }
    }
    SDL_SetRenderDrawColor(renderer, 30, 40, 50, 255);
    SDL_RenderClear(renderer);
    auto currentTick = SDL_GetTicks();
    for (auto t = oldTick; t < currentTick; ++t)
      if (!tick())
        return 1;
    oldTick = currentTick;
    draw();
    SDL_RenderPresent(renderer);
  }
  return 0;
}

bool garaga::tick()
{
  if (ticks++ % 250 == 0)
  {
    auto p = segmentsList.front();
    p.first += dx;
    p.second += dy;
    if (p.first < 0 || p.first >= Width / 64 ||
        p.second < 0 || p.second >= Height / 64)
      return false;
    for (const auto &segment: segmentsList)
      if (p == segment)
        return false;
    segmentsList.push_front(p);
    if (p.first != foodX || p.second != foodY)
      segmentsList.pop_back();
    else
      generateFood();
  }
  return true;
}

void garaga::draw()
{
  SDL_Rect src;
  src.x = 0;
  src.y = 0;
  src.w = 64;
  src.h = 64;
  SDL_Rect dest;
  dest.w = 64;
  dest.h = 64;
  int ds[][3] = {
    { -1, 0, 0 },
    { 0, -1, 90 },
    { 1, 0, 180 },
    { 0, 1, -90 },
  };
  for (auto iter = std::begin(segmentsList); iter != std::end(segmentsList); ++iter)
  {
    float direction = 0;
    const auto &segment = *iter;
    if (iter == std::begin(segmentsList))
    {
      if (iter->first + dx == foodX && iter->second + dy == foodY)
        src.x = HeadOpenMouth * 64;
      else
        src.x = Head * 64;
      if (iter + 1 != std::end(segmentsList))
      {
        const auto &nextSegment = *(iter + 1);
        for (const auto &d: ds)
        {
          if (segment.first + d[0] == nextSegment.first && segment.second + d[1] == nextSegment.second)
          {

            direction = d[2];
            break;
          }
        }
      }
    }
    else if (iter + 1 == std::end(segmentsList))
    {
      src.x = Tail * 64;
      const auto &prevSegment = *(iter - 1);
      for (const auto &d: ds)
      {
        if (segment.first == prevSegment.first + d[0] && segment.second == prevSegment.second + d[1])
        {
          direction = d[2];
          break;
        }
      }
    }
    else
    {
      const auto &nextSegment = *(iter + 1);
      const auto &prevSegment = *(iter - 1);
      if (nextSegment.first == prevSegment.first)
      {
        src.x = Straight * 64;
        direction = 90;
      }
      else if (nextSegment.second == prevSegment.second)
      {
        src.x = Straight * 64;
        direction = 0;
      }
      else
      {
        src.x = Turn * 64;
        bool up = false;
        if (segment.first == nextSegment.first && segment.second - 1 == nextSegment.second)
          up = true;
        if (segment.first == prevSegment.first && segment.second - 1 == prevSegment.second)
          up = true;
        bool right = false;
        if (segment.first + 1 == nextSegment.first && segment.second == nextSegment.second)
          right = true;
        if (segment.first + 1 == prevSegment.first && segment.second == prevSegment.second)
          right = true;
        bool down = false;
        if (segment.first == nextSegment.first && segment.second + 1 == nextSegment.second)
          down = true;
        if (segment.first == prevSegment.first && segment.second + 1 == prevSegment.second)
          down = true;
        bool left = false;
        if (segment.first - 1 == nextSegment.first && segment.second == nextSegment.second)
          left = true;
        if (segment.first - 1 == prevSegment.first && segment.second == prevSegment.second)
          left = true;
        if (up && right)
          direction = 0;
        else if (right && down)
          direction = 90;
        else if (down && left)
          direction = 180;
        else if (left && up)
          direction = 270;
      }
    }
    
    dest.x = 64 * segment.first;
    dest.y = 64 * segment.second;
    SDL_RenderCopyEx(renderer, sprites, &src, &dest, direction, nullptr, SDL_FLIP_NONE);
  }
  src.x = food * 64;
  dest.x = foodX * 64;
  dest.y = foodY * 64;
  SDL_RenderCopyEx(renderer, sprites, &src, &dest, 0, nullptr, SDL_FLIP_NONE);
}

