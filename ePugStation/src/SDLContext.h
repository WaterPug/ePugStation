#ifndef E_PUG_STATION_SDL_CONTEXT
#define E_PUG_STATION_SDL_CONTEXT

#include "SDL2/SDL.h"
#include "glad/glad.h"
#include <stdexcept>

constexpr int SDL_SUCCESS = 0;

namespace ePugStation
{
    class SDLContext
    {
    public:
        SDLContext()
        {
            if (SDL_Init(SDL_INIT_VIDEO) != SDL_SUCCESS)
            {
                throw std::runtime_error("failed to initialize SDL video");
            }

            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

            m_window = SDL_CreateWindow("ePugStation", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 512, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
            m_glContext = SDL_GL_CreateContext(m_window);

            gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);

            glClearColor(0, 0, 0, 255);
            glClear(GL_COLOR_BUFFER_BIT);

            SDL_GL_SwapWindow(m_window);
        }

        ~SDLContext()
        {
            SDL_DestroyWindow(m_window);
            SDL_Quit();
        }

        SDL_Window* getWindow() const { return m_window; }

    private:
        SDL_Window* m_window;
        SDL_GLContext m_glContext;
    };
}

#endif