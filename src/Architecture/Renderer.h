#ifndef E_PUG_STATION_RENDERER
#define E_PUG_STATION_RENDERER

#include <cstdint>
#include <memory>

#include "glad/glad.h"
#include "SDL2/SDL.h"

// TODO when improving...

//namespace sdl2
//{
//    // Taken from : https://eb2.co/blog/2014/04/c--14-and-sdl2-managing-resources/
//
//    template<typename Creator, typename Destructor, typename... Arguments>
//    auto make_resource(Creator c, Destructor d, Arguments&&... args)
//    {
//        auto r = c(std::forward<Arguments>(args)...);
//        if (!r) { throw std::system_error(errno, std::generic_category()); }
//        return std::unique_ptr<std::decay_t<decltype(*r)>, decltype(d)>(r, d);
//    }
//
//    using window_ptr_t = std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)>;
//    using renderer_ptr_t = std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)>;
//
//    inline window_ptr_t makeWindow(const char* title, int x, int y, int w, int h, Uint32 flags)
//    {
//        return make_resource(SDL_CreateWindow, SDL_DestroyWindow, title, x, y, w, h, flags);
//    };
//
//    inline renderer_ptr_t makeRenderer(SDL_Window* window, int index, Uint32 flags)
//    {
//        return make_resource(SDL_CreateRenderer, SDL_DestroyRenderer, window, index, flags);
//    }
//}

namespace ePugStation
{
    constexpr uint32_t VERTEX_BUFFER_LENGTH = 64 * 1024;

    template <typename T>
    class Buffer
    {
        Buffer()
        {
            glGenBuffers(1, &m_bufferObject);
            glBindBuffer(GL_ARRAY_BUFFER, m_bufferObject);

            GLsizeiptr elementSize = static_cast<GLsizeiptr>(sizeof(T));
            GLsizeiptr bufferSize = elementSize * VERTEX_BUFFER_LENGTH;

            // Persistent mapping, not coherent
            auto access = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT; // Persisten bit found with vcpkg
            // TODO...
            


        }

    private:
        GLuint m_bufferObject;

    };

    class Renderer
    {
    public:
        Renderer()
        {
            SDL_Init(SDL_INIT_VIDEO);

            m_window = SDL_CreateWindow("ePugStation", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 512, SDL_WINDOW_OPENGL);
            m_renderer = SDL_CreateRenderer(m_window, -1 , 0);

            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

            m_glContext = SDL_GL_CreateContext(m_window);

            gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);

            glClearColor(0, 0, 0, 255);
            glClear(GL_COLOR_BUFFER_BIT);

            SDL_GL_SwapWindow(m_window);
        }

        ~Renderer()
        {
            SDL_DestroyWindow(m_window);
            SDL_Quit();
        }

    private:
        SDL_Window* m_window;
        SDL_Renderer* m_renderer;
        SDL_GLContext m_glContext;
    };
}
#endif