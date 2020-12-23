#include "SDLContext.h"
#include "Interconnect.h"
#include "CPU.h"

#include "SDL2/SDL.h"

int main(int /*argc*/, char** /*argv[]*/)
{
   ePugStation::SDLContext sdlContext;
   auto interconnect = new ePugStation::Interconnect(&sdlContext);
   auto cpu = ePugStation::CPU(interconnect);

   bool isRunning = true;
   while (isRunning)
   {
      for (int i = 0; i < 100000; ++i)
      {
         cpu.runNextInstruction();
      }

      SDL_Event sdlEvent;
      if (SDL_PollEvent(&sdlEvent) != SDL_SUCCESS)
      {
         switch (sdlEvent.type)
         {
         case SDL_QUIT:
            isRunning = false;
            break;
         case SDL_KEYDOWN:
            if (sdlEvent.key.keysym.sym == SDLK_ESCAPE)
            {
               isRunning = false;
            }
            break;
         default:
            std::cout << "Unhandled event\n";
         }
      }
   }

   return -1;
}
