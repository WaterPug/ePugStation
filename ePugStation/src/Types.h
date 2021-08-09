
#ifndef E_PUG_STATION_TYPES
#define E_PUG_STATION_TYPES

namespace ePugStation
{
   struct TexCoord
   {
      TexCoord() : value(0) {}
      TexCoord(uint32_t reg) : value(reg) {}
      union {
         uint32_t value;
         struct {
            unsigned xPos : 8;
            unsigned yPos : 8;

         }coord;
         struct {
            unsigned empty : 16;
            unsigned x : 6;
            unsigned y : 9;
            unsigned unused : 1;

         }palette;
         struct {
            unsigned empty : 16;
            unsigned xBase : 4; // *64
            unsigned yBase : 1; // 0 or 256
            unsigned semiTrans : 2;
            unsigned texPageColors : 2; // 4, 8, 15 bit

         }texPage;
      };
   };
}

#endif