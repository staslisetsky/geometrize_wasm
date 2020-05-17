set ProjectDir="P:\work\2020\geometrize-wasm"

pushd build_wasm

@echo off
echo Building wasm DEBUG:
set Exported="['_GeometrizeLoadImage', '_GeometrizeStep', '_GeometrizeGetFullResultJson', '_GeometrizeGetStepResultJson', '_GeometrizeGetFullResultSvg', '_GeometrizeGetStepResultSvg', '_GeometrizeReset', '_main']"

rem call emcc -c -std=c++11 -O2 ^
rem     -s USE_PTHREADS=1 ^
rem     %ProjectDir%/geometrize/exporter/bitmapdataexporter.cpp ^
rem     %ProjectDir%/geometrize/exporter/bitmapexporter.cpp ^
rem     %ProjectDir%/geometrize/exporter/shapearrayexporter.cpp ^
rem     %ProjectDir%/geometrize/exporter/shapejsonexporter.cpp ^
rem     %ProjectDir%/geometrize/exporter/shapeserializer.cpp ^
rem     %ProjectDir%/geometrize/exporter/svgexporter.cpp ^
rem     %ProjectDir%/geometrize/commonutil.cpp ^
rem     %ProjectDir%/geometrize/core.cpp ^
rem     %ProjectDir%/geometrize/model.cpp ^
rem     %ProjectDir%/geometrize/state.cpp ^
rem     %ProjectDir%/geometrize/bitmap/bitmap.cpp ^
rem     %ProjectDir%/geometrize/bitmap/rgba.cpp ^
rem     %ProjectDir%/geometrize/runner/imagerunner.cpp ^
rem     %ProjectDir%/geometrize/rasterizer/rasterizer.cpp ^
rem     %ProjectDir%/geometrize/rasterizer/scanline.cpp ^
rem     %ProjectDir%/geometrize/shape/circle.cpp ^
rem     %ProjectDir%/geometrize/shape/ellipse.cpp ^
rem     %ProjectDir%/geometrize/shape/line.cpp ^
rem     %ProjectDir%/geometrize/shape/polyline.cpp ^
rem     %ProjectDir%/geometrize/shape/quadraticbezier.cpp ^
rem     %ProjectDir%/geometrize/shape/rectangle.cpp ^
rem     %ProjectDir%/geometrize/shape/rotatedellipse.cpp ^
rem     %ProjectDir%/geometrize/shape/rotatedrectangle.cpp ^
rem     %ProjectDir%/geometrize/shape/shapefactory.cpp ^
rem     %ProjectDir%/geometrize/shape/shapemutator.cpp ^
rem     %ProjectDir%/geometrize/shape/shapetypes.cpp ^
rem     %ProjectDir%/geometrize/shape/triangle.cpp

call emcc ^
    %ProjectDir%\wasm_main.cpp -o main.js --pre-js %ProjectDir%\web\wasm_loader.js ^
    -O2 -std=c++11 -Wno-null-dereference -Wno-writable-strings -Wno-format ^
    -s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=1 -s ASSERTIONS=1 -s WASM=1 ^
    -s NO_EXIT_RUNTIME=1 ^
    -s EXPORTED_FUNCTIONS=%Exported% ^
    -s USE_PTHREADS=1 ^
    -s TOTAL_MEMORY=268435456 ^
    -s EXTRA_EXPORTED_RUNTIME_METHODS="['ccall', 'cwrap', 'getValue']" ^
    bitmap.o bitmapdataexporter.o bitmapexporter.o circle.o commonutil.o core.o ellipse.o imagerunner.o line.o model.o polyline.o quadraticbezier.o rasterizer.o rectangle.o rgba.o rotatedellipse.o rotatedrectangle.o scanline.o shapearrayexporter.o shapefactory.o shapejsonexporter.o shapemutator.o shapeserializer.o shapetypes.o state.o svgexporter.o triangle.o

popd

copy build_wasm\main.js web /y
copy build_wasm\main.wasm web /y
rem copy build_wasm\main.js.mem web /y
rem copy build_wasm\main.wasm.map web /y
rem copy build_wasm\main.wast web /y
copy build_wasm\main.worker.js web /y