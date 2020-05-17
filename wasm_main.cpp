/*

Webassembly wrapper for Geometrize library
(https://github.com/Tw1ddle/geometrize-lib)

Made by Stas Lisetsky in 2020
stas.lisetsky@gmail.com
https://twitter.com/stas_lisetsky

*/

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
   void EMSCRIPTEN_KEEPALIVE GeometrizeLoadImage(unsigned char *Data, int Size, unsigned int ShapeType, int ShapeCount, int MaxMutations, int Alpha);
   void EMSCRIPTEN_KEEPALIVE GeometrizeStep();
   void EMSCRIPTEN_KEEPALIVE GeometrizeReset();
   void * EMSCRIPTEN_KEEPALIVE GeometrizeGetFullResultJson();
   void * EMSCRIPTEN_KEEPALIVE GeometrizeGetStepResultJson();
   void * EMSCRIPTEN_KEEPALIVE GeometrizeGetFullResultSvg(std::uint32_t W, std::uint32_t H);
   void * EMSCRIPTEN_KEEPALIVE GeometrizeGetStepResultSvg(std::uint32_t W, std::uint32_t H);
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

struct state {
    std::vector<geometrize::ShapeResult> Result;
    std::vector<geometrize::ShapeResult> StepResult;

    geometrize::ImageRunnerOptions Options {};
    geometrize::Bitmap *Bitmap = nullptr;
    geometrize::ImageRunner *Runner = nullptr;

    int ImageW = 0;
    int ImageH = 0;

    volatile bool WorkerRunning = false;
    int CurrentStep = 0;

    std::string TextResult = "";
};

state State {};

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

const geometrize::Bitmap
LoadImage(unsigned char *Data, int Size)
{
    int w,h,n = 0;
    unsigned char *LoadedImage = stbi_load_from_memory(Data, Size, &w, &h, &n, 4);

    State.ImageW = w;
    State.ImageH = h;

    char Buffer[1024];
    sprintf(Buffer, "stb: image loaded %dx%d, unpacked size: %d bytes", w, h, w*h*4);
    WasmConsoleLog(Buffer);

    const std::vector<std::uint8_t> BitmapData{LoadedImage, LoadedImage + (w*h*4)};
    const geometrize::Bitmap Bitmap(static_cast<std::uint32_t>(w), static_cast<std::uint32_t>(h), BitmapData);

    return Bitmap;
}

void
ThreadProc()
{
    WasmConsoleLog("started geometrize thread");

    State.StepResult.clear();

    if (State.CurrentStep < State.Options.shapeCount) {
        State.StepResult = State.Runner->step(State.Options);
        std::copy(State.StepResult.begin(), State.StepResult.end(), std::back_inserter(State.Result));
        ++State.CurrentStep;
    } else {
        WasmConsoleLog("already at max shape count");
    }

    State.WorkerRunning = false;
    emscripten_run_script("postMessage({cmd: \"objectTransfer\", msg: \"step_done\"})");
}

void
GeometrizeReset()
{
    State.Result.clear();

    if (State.Bitmap) {
        delete State.Bitmap;
    }
    if (State.Runner) {
        delete State.Runner;
    }

    State.Bitmap = nullptr;
    State.Runner = nullptr;
    State.CurrentStep = 0;
}

void
GeometrizeLoadImage(unsigned char *Data, int Size, unsigned int ShapeType, int ShapeCount, int MaxMutations, int Alpha)
{
    GeometrizeReset();

    State.Options.alpha = Alpha;
    State.Options.maxShapeMutations = MaxMutations;
    State.Options.shapeCount = ShapeCount;
    State.Options.shapeTypes = (geometrize::ShapeTypes)ShapeType;
    State.Options.seed = 9001;

    State.Bitmap = new geometrize::Bitmap { LoadImage(Data, Size) };
    State.Runner = new geometrize::ImageRunner(*State.Bitmap);
}

void
GeometrizeStep()
{
    if (!State.WorkerRunning) {
        State.WorkerRunning = true;
        std::thread WorkerThread {ThreadProc};
        WorkerThread.detach();
    } else {
        WasmConsoleLog("worker already running");
    }
}

void *
GeometrizeGetFullResultJson()
{
    static char *Response = 0;

    if (!Response) {
        free(Response);
    }

    std::string JSON = geometrize::exporter::exportShapeJson(State.Result);
    Response = (char *)malloc(JSON.size() + 1);
    Response[JSON.size()] = 0;
    memcpy(Response, JSON.c_str(), JSON.size());

    return (void *)Response;
}

void *
GeometrizeGetStepResultJson()
{
    static char *Response = 0;

    if (!Response) {
        free(Response);
    }

    std::string JSON = geometrize::exporter::exportShapeJson(State.StepResult);
    Response = (char *)malloc(JSON.size() + 1);
    Response[JSON.size()] = 0;
    memcpy(Response, JSON.c_str(), JSON.size());

    return (void *)Response;
}

void *
GeometrizeGetFullResultSvg(std::uint32_t W, std::uint32_t H)
{
    static char *Response = 0;

    if (!Response) {
        free(Response);
    }

    std::string JSON = geometrize::exporter::exportSVG(State.Result, W, H);
    Response = (char *)malloc(JSON.size() + 1);
    Response[JSON.size()] = 0;
    memcpy(Response, JSON.c_str(), JSON.size());

    return (void *)Response;
}

void *
GeometrizeGetStepResultSvg(std::uint32_t W, std::uint32_t H)
{
    static char *Response = 0;

    if (!Response) {
        free(Response);
    }

    std::string JSON = geometrize::exporter::exportSVG(State.StepResult, W, H);
    Response = (char *)malloc(JSON.size() + 1);
    Response[JSON.size()] = 0;
    memcpy(Response, JSON.c_str(), JSON.size());

    return (void *)Response;
}

int
main() {
    WasmConsoleLog("wasm app started");
}