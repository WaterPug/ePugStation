#ifndef E_PUG_STATION_RENDERER
#define E_PUG_STATION_RENDERER

#include <cstdint>
#include <iostream>
#include <stdexcept>

#include "Constants.h"
#include "Utils.h"
#include "Types.h"

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
   public:
      Buffer();
      ~Buffer();

      void set(uint32_t index, T value);

   private:
      GLuint m_bufferObject;
      T* m_memory;
   };

   template <typename T>
   Buffer<T>::Buffer()
   {
      // Gen buffer object
      glGenBuffers(1, &m_bufferObject);
      // Bind it
      glBindBuffer(GL_ARRAY_BUFFER, m_bufferObject);

      GLsizeiptr elementSize = static_cast<GLsizeiptr>(sizeof(T));
      GLsizeiptr bufferSize = elementSize * VERTEX_BUFFER_LENGTH;

      // Persistent mapping
      GLbitfield access = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT;

      // Allocater buffer memory
      glBufferStorage(GL_ARRAY_BUFFER, bufferSize, 0, access);

      // Remap buffer
      m_memory = (T*)glMapBufferRange(GL_ARRAY_BUFFER, 0, bufferSize, access);

      // Reset array to 0
      for (int i = 0; i < VERTEX_BUFFER_LENGTH; ++i)
      {
         m_memory[i] = T();
      }
   }

   template <typename T>
   Buffer<T>::~Buffer()
   {
      glBindBuffer(GL_ARRAY_BUFFER, m_bufferObject);
      glUnmapBuffer(GL_ARRAY_BUFFER);
      glDeleteBuffers(1, &m_bufferObject);
   }

   template <typename T>
   void Buffer<T>::set(uint32_t index, T value)
   {
      if (index >= VERTEX_BUFFER_LENGTH)
      {
         throw std::runtime_error("buffer overflow");
      }

      m_memory[index] = value;
   }

   struct Position
   {
      Position() : value(0) {}
      Position(uint32_t reg) : value(reg) {}
      Position(uint16_t in_x, uint16_t in_y) : value(0) { bit.x = in_x; bit.y = in_y; }
      Position(const Position& pos) : value(0) { bit.x = pos.bit.x; bit.y = pos.bit.y; }
      uint32_t getValue() { return value; }
      union {
         unsigned value;
         struct {
            GLshort x : 16;
            GLshort y : 16;
         }bit;
      };
   };

   struct Color
   {
      Color() : value(0) {}
      Color(uint32_t reg) : value(reg) {}
      Color(uint8_t in_r, uint8_t in_g, uint8_t in_b) : value(0) { bit.r = in_r; bit.g = in_g; bit.b = in_b; }
      Color(const Color& color) : value(0) { bit.r = color.bit.r; bit.g = color.bit.g; bit.b = color.bit.b; }
      uint32_t getValue() { bit.other = 0;  return value; }
      union {
         unsigned value;
         struct {
            GLubyte r : 8;
            GLubyte g : 8;
            GLubyte b : 8;
            GLubyte other : 8;
         }bit;
      };
   };

   // TODO: Refactor this, works for now, or else values get shifted with Color...
   struct CustomColor
   {
      CustomColor() : r(0), g(0), b(0) {};
      CustomColor(Color color) : r(color.bit.r), g(color.bit.g), b(color.bit.b) {};

      GLubyte r;
      GLubyte g;
      GLubyte b;
   };

   struct CustomTexCoord
   {
      CustomTexCoord() : x(0), y(0) {};
      CustomTexCoord(TexCoord texCoord) : x(texCoord.coord.xPos), y(texCoord.coord.yPos) {};
      CustomTexCoord(GLubyte xPos, GLubyte yPos) : x(xPos), y(yPos) {};

      GLubyte x;
      GLubyte y;
   };

   class Renderer
   {
   public:
      Renderer() = delete;
      Renderer(SDLContext* context)
         : m_window(context->getWindow())
      {
         // Load shaders
         m_vertexShader = compileShader(getFileContent(PATH_TO_VERTEX_SHADER), GL_VERTEX_SHADER);
         m_fragmentShader = compileShader(getFileContent(PATH_TO_FRAGMENT_SHADER), GL_FRAGMENT_SHADER);

         linkProgram();

         glUseProgram(m_openglProgram);

         // Generate vertex attribute object for vertex attributes
         m_vao = 0;
         glGenVertexArrays(1, &m_vao);
         glBindVertexArray(m_vao);

         // TODO: bad... until I properly understand all this
         m_positions = new Buffer<Position>();
         GLint index = findProgramAttribute("vertexPosition");
         glVertexAttribIPointer(index, 2, GL_SHORT, 0, nullptr);
         glEnableVertexAttribArray(index);

         m_colors = new Buffer<CustomColor>();
         index = findProgramAttribute("vertexColor");
         glVertexAttribIPointer(index, 3, GL_UNSIGNED_BYTE, 0, nullptr);
         glEnableVertexAttribArray(index);

         m_texCoord = new Buffer<CustomTexCoord>();
         index = findProgramAttribute("aTexCoord");
         glVertexAttribIPointer(index, 2, GL_UNSIGNED_BYTE, 0, nullptr);
         glEnableVertexAttribArray(index);

         // Uniforms
         m_uniformOffset = findProgramUniform("offset");
         glUniform2i(m_uniformOffset, 0, 0);

         m_clut4Location = findProgramUniform("clut4");
         GLint clut4[16] = { 0 };
         glUniform1iv(m_clut4Location, 16, clut4);

         m_colorDepthLocation = findProgramUniform("colorDepth");
         glUniform1i(m_colorDepthLocation, 16);
      }

      ~Renderer()
      {
         glDeleteVertexArrays(1, &m_vao);
         glDeleteShader(m_vertexShader);
         glDeleteShader(m_fragmentShader);
         glDeleteProgram(m_openglProgram);

         delete m_positions;
         delete m_colors;
         delete m_texCoord;
      }

      template<uint8_t numberOfVertices>
      void pushMonochromePolygon(Position* positions, Color* colors)
      {
         // For now...
         pushShadedPolygon<numberOfVertices>(positions, colors);
      }

      template<uint8_t numberOfVertices>
      void pushShadedPolygon(Position* positions, Color* colors)
      {
         constexpr uint8_t totalNumberOfVertices = (numberOfVertices == 3) ? 3 : 6;
         if ((m_numberOfVertices + totalNumberOfVertices) > VERTEX_BUFFER_LENGTH)
         {
            std::cout << "Vertex attribute buffers full, forcing draw\n";
            draw();
         }
         for (int i = 0; i < 3; ++i)
         {
            m_positions->set(m_numberOfVertices, positions[i]);
            m_colors->set(m_numberOfVertices, CustomColor(colors[i]));
            m_texCoord->set(m_numberOfVertices, CustomTexCoord(69, 69));
            ++m_numberOfVertices;
         }
         if constexpr (numberOfVertices == 4)
         {
            for (int i = 1; i < 4; ++i)
            {
               m_positions->set(m_numberOfVertices, positions[i]);
               m_colors->set(m_numberOfVertices, CustomColor(colors[i]));
               m_texCoord->set(m_numberOfVertices, CustomTexCoord(69, 69));
               ++m_numberOfVertices;
            }
         }
      }

      template<uint8_t numberOfVertices>
      void pushTexturedPolygon(Position* positions, CustomTexCoord* texCoord)
      {
         constexpr uint8_t totalNumberOfVertices = (numberOfVertices == 3) ? 3 : 6;
         if ((m_numberOfVertices + totalNumberOfVertices) > VERTEX_BUFFER_LENGTH)
         {
            std::cout << "Vertex attribute buffers full, forcing draw\n";
            draw();
         }
         for (int i = 0; i < 3; ++i)
         {
            m_positions->set(m_numberOfVertices, positions[i]);
            m_colors->set(m_numberOfVertices, CustomColor());
            m_texCoord->set(m_numberOfVertices, texCoord[i]);
            ++m_numberOfVertices;
         }
         if constexpr (numberOfVertices == 4)
         {
            for (int i = 1; i < 4; ++i)
            {
               m_positions->set(m_numberOfVertices, positions[i]);
               m_colors->set(m_numberOfVertices, CustomColor());
               m_texCoord->set(m_numberOfVertices, texCoord[i]);
               ++m_numberOfVertices;
            }
         }
      }

      void display()
      {
         draw();
         SDL_GL_SwapWindow(m_window);
      }

      void setDrawOffset(int16_t x, int16_t y)
      {
         draw();
         glUniform2i(m_uniformOffset, static_cast<GLint>(x), static_cast<GLint>(y));
      }

      void setClut4(GLint data[16])
      {
         glUniform1iv(m_clut4Location, 16, data);
      }

      void setColorDepth(GLint colorDepth)
      {  
         glUniform1i(m_colorDepthLocation, colorDepth);
      }

   private:
      SDL_Window* m_window;

      GLuint m_vertexShader;
      GLuint m_fragmentShader;
      GLuint m_openglProgram;
      GLuint m_vao;
      GLuint m_uniformOffset;
      GLint m_clut4Location;
      GLint m_colorDepthLocation;
      Buffer<Position>* m_positions;
      Buffer<CustomColor>* m_colors;
      Buffer<CustomTexCoord>* m_texCoord;
      uint32_t m_numberOfVertices;

      GLuint compileShader(const std::string& content, GLenum shaderType)
      {
         GLuint shader = glCreateShader(shaderType);

         const GLchar* source = (const GLchar*)content.c_str();
         glShaderSource(shader, 1, &source, nullptr);
         glCompileShader(shader);

         // Error checking
         GLint status = GL_FALSE;
         glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
         if (status != GL_TRUE)
         {
            throw std::runtime_error("Shader compilation failed");
         }
         return shader;
      }

      void linkProgram()
      {
         m_openglProgram = glCreateProgram();

         glAttachShader(m_openglProgram, m_vertexShader);
         glAttachShader(m_openglProgram, m_fragmentShader);

         glLinkProgram(m_openglProgram);

         // Error checking
         GLint status = GL_FALSE;
         glGetProgramiv(m_openglProgram, GL_LINK_STATUS, &status);
         if (status != GL_TRUE)
         {
            throw std::runtime_error("OpenGL program linking failed");
         }
      }

      GLuint findProgramUniform(const std::string& attribute)
      {
         const GLchar* source = (const GLchar*)attribute.c_str();
         GLint index = glGetUniformLocation(m_openglProgram, source);

         if (index < 0)
         {
            throw std::runtime_error("Uniform attribute not found in program");
         }
         return index;
      }

      GLuint findProgramAttribute(const std::string& attribute)
      {
         const GLchar* source = (const GLchar*)attribute.c_str();
         GLint index = glGetAttribLocation(m_openglProgram, source);

         if (index < 0)
         {
            throw std::runtime_error("Attribute not found in program");
         }
         return index;
      }

      // TODO: Inefficient, CPU is stalling... Could use double or triple buffering (for later)
      void draw()
      {
         // Make sure persistent mapping data is flushed to the buffer
         glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);
         glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_numberOfVertices));

         // Wait for GPU to complete
         auto sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
         bool complete = false;
         while (!complete)
         {
            auto result = glClientWaitSync(sync, GL_SYNC_FLUSH_COMMANDS_BIT, 10000000);
            complete = (result == GL_ALREADY_SIGNALED || result == GL_CONDITION_SATISFIED);
         }
         m_numberOfVertices = 0;
      }
   };
}
#endif