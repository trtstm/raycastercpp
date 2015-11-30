// g++ --std=c++11 main.cpp -o raycaster -lSDL2

#include <array>
#include <cmath>
#include <iostream>

#include <SDL2/SDL.h>

#include "Angle.hpp"

constexpr unsigned int SCREEN_X = 320;
constexpr unsigned int SCREEN_Y = 200;
constexpr unsigned int TILE_SIZE = 32;
constexpr Angle FOV = 60_deg;
constexpr double PROJECTION_DISTANCE = (SCREEN_X / 2.0) / std::tan(FOV.getRad() / 2.0) / TILE_SIZE;
constexpr Angle COLUMN_ANGLE = FOV / SCREEN_X;

SDL_Renderer* renderer;

struct Intersection {
  char type;
  double x;
  double y;
  unsigned int tilex;
  unsigned int tiley;
  double distance;
};

std::ostream& operator<<(std::ostream& os, const Intersection& i)
{
  os << "type: " << i.type << std::endl;
  os << "x: " << i.x << ", y: " << i.y << std::endl;
  os << "tilex: " << i.tilex << ", tiley: " << i.tiley << std::endl;
  os << "distance: " << i.distance;
  return os;
}

std::array<std::array<int, 16>, 16> map{{
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,1,1,1,1,1,0,0,0,0,0,0,1},
  {1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,1,1,1,1,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,1,0,0,0,1,1,1,0,1},
  {1,0,0,0,1,1,1,1,0,0,0,0,0,1,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1},
  {1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
}};

int raysCasted = 0;

extern const unsigned int ITERATIONS = 20;

bool doIterations(double ax, double ay, double dx, double dy, double x, double y, Intersection& intersection)
{
  for(unsigned int i = 0; i < ITERATIONS; i++) {
    double ix = ax + i * dx;
    double iy = ay + i * dy;
    if(ix < 0 || iy < 0 || ix >= map[0].size() || iy >= map.size()) {
      break;
    }
    
    unsigned int tileX = std::floor(ix);
    unsigned int tileY = std::floor(iy);

    if(intersection.type == 'h') {
      bool topWall = false;
      bool bottomWall = true;
      if(map[tileY][tileX] != 0) {
	topWall = true;
	bottomWall = false;
      }

      if(bottomWall) {
	tileY -= 1;
      }
    }

    if(intersection.type == 'v') {
      bool leftWall = true;
      bool rightWall = false;
      if(tileX > 0 && map[tileY][tileX - 1] != 0) {
	leftWall = false;
	rightWall = true;
      }

      if(rightWall) {
	tileX -= 1;
      }
    }

    if(map[tileY][tileX] != 0) {
      intersection.x = ix;
      intersection.y = iy;
      intersection.tilex = tileX;
      intersection.tiley = tileY;
      intersection.distance = std::sqrt(std::pow(x - ix, 2) + std::pow(y - iy, 2));
      return true;
    }
  }

  return false;
}

bool getHorizontalIntersection(Angle a, double x, double y, Intersection& intersection)
{
  intersection.type = 'h';
  
  double dx;
  double dy;
  // First intersection
  double ax;
  double ay;
  
  switch(a.quadrant()) {
  case Angle::Quadrant::FIRST:
    // north east
    ay = std::floor(y);
    ax = x + (y - ay) / std::tan(a.getRad());
    dx = 1.0 / std::tan(a.getRad());
    dy = -1;
    break;
    
  case Angle::Quadrant::FIRST_SECOND:
    // north
    ax = x;
    ay = std::floor(y);
    dx = 0;
    dy = -1;
    break;
    
  case Angle::Quadrant::SECOND:
    // north west
    ay = std::floor(y);
    ax = x + (y - ay) / std::tan(a.getRad());
    dx = 1.0 / std::tan(a.getRad());
    dy = -1;
    break;
    
  case Angle::Quadrant::SECOND_THIRD:
    // west
    return false;
    /*
      ax = std::floor(x);
      ay = y;
      dx = -1;
      dy = 0;
    */
    break;
    
  case Angle::Quadrant::THIRD:
    // south west
    ay = std::ceil(y);
    ax = x + (y - ay) / std::tan(a.getRad());
    dx = -1 / std::tan(a.getRad());
    dy = 1;
    break;
    
  case Angle::Quadrant::THIRD_FOURTH:
    // south
    ax = x;
    ay = std::ceil(y);
    dx = 0;
    dy = 1;
    break;
    
  case Angle::Quadrant::FOURTH:
    // south east
    ay = std::ceil(y);
    ax = x + (y - ay) / std::tan(a.getRad());
    dx = -1 / std::tan(a.getRad());
    dy = 1;
    break;
    
  case Angle::Quadrant::FOURTH_FIRST:
    // east
    return false;
    /*ax = std::ceil(x);
      ay = y;
      dx = 1;
      dy = 0;*/
    break;
  }


  return doIterations(ax, ay, dx, dy, x, y, intersection);
  /*for(unsigned int i = 0; i < ITERATIONS; i++) {
    double ix = ax + i * dx;
    double iy = ay + i * dy;
    if(ix < 0 || iy < 0 || ix >= map[0].size() || iy >= map.size()) {
    break;
    }
    
    unsigned int tileX = std::floor(ix);
    unsigned int tileY = std::floor(iy);

    bool topWall = false;
    bool bottomWall = true;
    if(map[tileY][tileX] != 0) {
    topWall = true;
    bottomWall = false;
    }

    if(bottomWall) {
    tileY -= 1;
    }

    if(map[tileY][tileX] != 0) {
    intersection.x = ix;
    intersection.y = iy;
    intersection.tilex = tileX;
    intersection.tiley = tileY;
    intersection.distance = std::sqrt(std::pow(x - ix, 2) + std::pow(y - iy, 2));
    raysCasted += intersection.distance;
    return true;
    }


    }*/

  return false;
}


bool getVerticalIntersection(Angle a, double x, double y, Intersection& intersection)
{
  intersection.type = 'v';
  
  double dx;
  double dy;
  // First intersection
  double ax;
  double ay;
  
  switch(a.quadrant()) {
  case Angle::Quadrant::FIRST:
    // north east
    ax = std::ceil(x);
    ay = y - (ax - x) * std::tan(a.getRad());
    dx = 1;
    dy = -std::tan(a.getRad());
    break;
    
  case Angle::Quadrant::FIRST_SECOND:
    // north
    return false;
    /*ax = x;
      ay = std::floor(y);
      dx = 0;
      dy = -1;*/
    break;
    
  case Angle::Quadrant::SECOND:
    // north west
    ax = std::floor(x);
    ay = y - (ax - x) * std::tan(a.getRad());
    dx = -1;
    dy = std::tan(a.getRad());
    break;
    
  case Angle::Quadrant::SECOND_THIRD:
    // west
    ax = std::floor(x);
    ay = y;
    dx = -1;
    dy = 0;
    break;
    
  case Angle::Quadrant::THIRD:
    // south west
    ax = std::floor(x);
    ay = y - (ax - x) * std::tan(a.getRad());
    dx = -1;
    dy = std::tan(a.getRad());
    break;
    
  case Angle::Quadrant::THIRD_FOURTH:
    // south
    return false;
    /*ax = x;
      ay = std::ceil(y);
      dx = 0;
      dy = 1;*/
    break;
    
  case Angle::Quadrant::FOURTH:
    // south east
    ax = std::ceil(x);
    ay = y - (ax - x) * std::tan(a.getRad());
    dx = 1;
    dy = -std::tan(a.getRad());
    break;
    
  case Angle::Quadrant::FOURTH_FIRST:
    // east
    ax = std::ceil(x);
    ay = y;
    dx = 1;
    dy = 0;
    break;
  }

  
  return doIterations(ax, ay, dx, dy, x, y, intersection);

  /*for(unsigned int i = 0; i < ITERATIONS; i++) {
    double ix = ax + i * dx;
    double iy = ay + i * dy;
    if(ix < 0 || iy < 0 || ix >= map[0].size() || iy >= map.size()) {
    break;
    }
    
    unsigned int tileX = std::floor(ix);
    unsigned int tileY = std::floor(iy);

    bool leftWall = true;
    bool rightWall = false;
    if(tileX > 0 && map[tileY][tileX - 1] != 0) {
    leftWall = false;
    rightWall = true;
    }

    if(rightWall) {
    tileX -= 1;
    }

    if(map[tileY][tileX] != 0) {
    intersection.x = ix;
    intersection.y = iy;
    intersection.tilex = tileX;
    intersection.tiley = tileY;
    intersection.distance = std::sqrt(std::pow(x - ix, 2) + std::pow(y - iy, 2));
    raysCasted += intersection.distance;
    return true;
    }
    }*/

  return false;
}

bool castRay(Angle a, double x, double y, Intersection& intersection)
{
  Intersection hi;
  Intersection vi;
  bool hasHi = getHorizontalIntersection(a, x, y, hi);
  bool hasVi = getVerticalIntersection(a, x, y, vi);

  if(hasHi && hasVi) {
    if(hi.distance <= vi.distance) {
      intersection = hi;
    } else {
      intersection = vi;
    }

    return true;
  }

  if(hasHi) {
    intersection = hi;
    return true;
  }

  if(hasVi) {
    intersection = vi;
    return true;
  }

  return false;
}

void doRaycast(Angle a, double x, double y)
{
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  
  auto angle = a + FOV / 2;
  for(unsigned int i = 0; i < SCREEN_X; i++) {
    Intersection intersection;
    if(castRay(angle, x, y, intersection) == false) {
      continue;
    }

    double normalizedDistance = intersection.distance * std::cos((a - angle).getRad());

    double pHeight = (1.0 / normalizedDistance) * PROJECTION_DISTANCE;

    SDL_RenderDrawLine(renderer,
      i,
      SCREEN_Y / 2.0 - pHeight * TILE_SIZE / 2,
      i,
      SCREEN_Y / 2.0 + pHeight * TILE_SIZE / 2);
    
    angle -= COLUMN_ANGLE;
  }
}

Angle angle = 220_deg;
double dt = 1.0/30.0;
double x = 10;
double y = 4;

int main()
{ 
      SDL_Init(SDL_INIT_VIDEO);
      SDL_Window* window = SDL_CreateWindow("Raycaster",
      SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED,
      SCREEN_X,
      SCREEN_Y,
      SDL_WINDOW_SHOWN);

      SDL_Surface* surface = SDL_CreateRGBSurface(0,SCREEN_X, SCREEN_Y,32,0,0,0,0);

      renderer = SDL_CreateRenderer(window, -1, 0);

      while(true) {
      SDL_PumpEvents();
      const Uint8* keys = SDL_GetKeyboardState(0);

      if(keys[SDL_SCANCODE_LEFT]) {
      angle += 45_deg * dt;
      }
    
      if(keys[SDL_SCANCODE_RIGHT]) {
      angle -= 45_deg * dt;
      }
    
      if(keys[SDL_SCANCODE_UP]) {
      x += std::sin((angle + 90_deg).getRad()) * dt;
      y += std::cos((angle + 90_deg).getRad()) * dt;
      }

      if(keys[SDL_SCANCODE_DOWN]) {
      x -= std::sin((angle + 90_deg).getRad()) * dt;
      y -= std::cos((angle + 90_deg).getRad()) * dt;
      }

      if(keys[SDL_SCANCODE_ESCAPE]) {
      break;
      }
    
      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
      SDL_RenderClear(renderer);

      doRaycast(angle, x, y);

      SDL_RenderPresent(renderer);
      SDL_Delay(dt * 1000);
      }


      SDL_FreeSurface(surface);
      SDL_DestroyWindow(window);
      SDL_Quit();
  
  
}
