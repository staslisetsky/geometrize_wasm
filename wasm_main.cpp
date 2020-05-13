#include <emscripten/emscripten.h>
#include <emscripten/key_codes.h>
#include <emscripten/html5.h>

#include <stdint.h>
#include <unistd.h>
#include <malloc.h>
#include <vector>
#include <string>
#include <thread>

// EM_ASM(console.log("Recei: " + $0 + " Index: " + $1), Event->Index, FirstUnusedEvent);

extern "C" {
   void EMSCRIPTEN_KEEPALIVE GeometrizeLoadImage(unsigned char *Data, int Size, int ShapeCount, int MaxMutations, int Alpha);
   void EMSCRIPTEN_KEEPALIVE GeometrizeStep();
   void * EMSCRIPTEN_KEEPALIVE GetResultSvg();
}

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

// EM_ASM(console.log("Key Down: " + $0 + " Index: " + $1), Event->Index, FirstUnusedEvent);

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

static std::vector<geometrize::ShapeResult> Result;
static int ImageW = 0;
static int ImageH = 0;
static std::string Test("");

const geometrize::Bitmap
LoadImage(unsigned char *Data, int Size)
{
    int w,h,n = 0;
    unsigned char *LoadedImage = stbi_load_from_memory(Data, Size, &w, &h, &n, 4);

    ImageW = w;
    ImageH = h;

    char Buffer[1024];
    sprintf(Buffer, "stb: image loaded %dx%d, unpacked size: %d bytes", w, h, w*h*4);
    WasmConsoleLog(Buffer);

    const std::vector<std::uint8_t> BitmapData{LoadedImage, LoadedImage + (w*h*4)};
    const geometrize::Bitmap Bitmap(static_cast<std::uint32_t>(w), static_cast<std::uint32_t>(h), BitmapData);

    return Bitmap;
}

geometrize::ImageRunnerOptions Options{};
geometrize::Bitmap *Bitmap;
geometrize::ImageRunner *Runner;

static bool Done = false;
static int CurrentStep = 0;

void
ThreadProc()
{
    WasmConsoleLog("started geometrize thread");

    if (CurrentStep < Options.shapeCount) {
        std::vector<geometrize::ShapeResult> Shapes{Runner->step(Options)};

        for (int j=0; j<Shapes.size(); ++j) {
            WasmConsoleLog("shape added");
        }

        std::copy(Shapes.begin(), Shapes.end(), std::back_inserter(Result));
        ++CurrentStep;
    } else {
        WasmConsoleLog("already at max shape count");
    }
}

void
GeometrizeLoadImage(unsigned char *Data, int Size, int ShapeCount, int MaxMutations, int Alpha)
{
    Options.alpha = Alpha;
    Options.maxShapeMutations = MaxMutations;
    Options.shapeCount = ShapeCount;
    Options.shapeTypes = geometrize::ELLIPSE;
    Options.seed = 9001;

    Bitmap = new geometrize::Bitmap{LoadImage(Data, Size)};
    Runner = new geometrize::ImageRunner(*Bitmap);
}

void
GeometrizeStep()
{
    std::thread WorkerThread{ThreadProc};
    WorkerThread.detach();
}

void *
GetResultSvg()
{
    Test = geometrize::exporter::exportSVG(Result, ImageW, ImageH);
    // if (Done) {
    // } else {
    //     Test = "not done yet";
    // }

    const char *C = Test.c_str();
    return (void *)C;
}

int
main() {
    WasmConsoleLog("wasm app started");
}