#include <emscripten/emscripten.h>
#include <emscripten/key_codes.h>
#include <emscripten/html5.h>

#include <stdint.h>
#include <unistd.h>
#include <malloc.h>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#include "geometrize/commonutil.h"
#include "geometrize/shaperesult.h"
#include "geometrize/bitmap/bitmap.h"
#include "geometrize/bitmap/rgba.h"
#include "geometrize/exporter/shapejsonexporter.h"
#include "geometrize/exporter/svgexporter.h"
#include "geometrize/runner/imagerunner.h"
#include "geometrize/runner/imagerunneroptions.h"
#include "geometrize/shape/shape.h"
#include "geometrize/shape/rectangle.h"
#include "geometrize/shape/shapefactory.h"
#include "geometrize/shape/shapetypes.h"

extern "C" {
   void EMSCRIPTEN_KEEPALIVE GeometrizeImage(char *Path, int w, int h, unsigned char *Data);
//     void EMSCRIPTEN_KEEPALIVE BactorialUpdateWorld(float dt);
//     int EMSCRIPTEN_KEEPALIVE BactorialSelect(float minx, float miny, float maxx, float maxy);
//     void EMSCRIPTEN_KEEPALIVE BactorialDivide();
//     void EMSCRIPTEN_KEEPALIVE BactorialUnselect();
//     void EMSCRIPTEN_KEEPALIVE BactorialSpawnEnemy(float Distance, float Radius, float velocityx, float velocityy);
}

//
//
//

void
WasmConsoleLog(const char *String)
{
   EM_ASM(console.log(UTF8ToString($0)), String);
}

void
WasmPrintError(EMSCRIPTEN_RESULT Error)
{
   switch(Error) {
      case EMSCRIPTEN_RESULT_SUCCESS: {
        WasmConsoleLog("The operation succeeded.");
      } break;
      case EMSCRIPTEN_RESULT_DEFERRED: {
         WasmConsoleLog("The requested operation cannot be completed now for web security reasons, and has been deferred for completion in the next event handler.");
      } break;
      case EMSCRIPTEN_RESULT_NOT_SUPPORTED: {
         WasmConsoleLog("The given operation is not supported by this browser or the target element. This value will be returned at the time the callback is registered if the operation is not supported.");
      } break;
      case EMSCRIPTEN_RESULT_FAILED_NOT_DEFERRED: {
         WasmConsoleLog("The requested operation could not be completed now for web security reasons. It failed because the user requested the operation not be deferred.");
      } break;
      case EMSCRIPTEN_RESULT_INVALID_TARGET: {
         WasmConsoleLog("The operation failed because the specified target element is invalid.");
      } break;
      case EMSCRIPTEN_RESULT_UNKNOWN_TARGET: {
         WasmConsoleLog("The operation failed because the specified target element was not found.");
      } break;
      case EMSCRIPTEN_RESULT_INVALID_PARAM: {
         WasmConsoleLog("The operation failed because an invalid parameter was passed to the function.");
      } break;
      case EMSCRIPTEN_RESULT_FAILED: {
         WasmConsoleLog("Generic failure result message, returned if no specific result is available.");
      } break;
      case EMSCRIPTEN_RESULT_NO_DATA: {
         WasmConsoleLog("The operation failed because no data is currently available.");
      } break;
      default: {
         WasmConsoleLog("Unknown error");
      }

   }
}

geometrize::Bitmap
ReadImage(char *Path)
{
    int w,h,n = 0;
    unsigned char *Data = stbi_load(Path,&w,&h,&n,4);

    const std::vector<std::uint8_t> BitmapData{Data, Data + (w*h*4)};
    stbi_image_free(Data);

    const geometrize::Bitmap Bitmap(static_cast<std::uint32_t>(w), static_cast<std::uint32_t>(h), BitmapData);

    return Bitmap;
}


// EM_ASM(console.log("Key Down: " + $0 + " Index: " + $1), Event->Index, FirstUnusedEvent);

int main() {
   WasmConsoleLog("wasm app started");
}

void
GeometrizeImage(char *File, int w, int h, unsigned char *Data)
{
   char Buffer[1024];
   sprintf(Buffer, "Received image %s, %dx%d", File, w, h);
   WasmConsoleLog(Buffer);
   // EM_ASM(console.log("Recei: " + $0 + " Index: " + $1), Event->Index, FirstUnusedEvent);
}