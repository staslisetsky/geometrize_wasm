@echo off

set ProjectDir="P:\work\2020\geometrize-wasm"
set Common=-TP -MT -FC -Zi -nologo -wd4700 -wd4312 -wd4311 -wd4530 -Od

pushd build

rem cl -c /TP /std:c++14 %Common% ^
rem     %ProjectDir%\geometrize\bitmap\*.cpp ^
rem     %ProjectDir%\geometrize\exporter\*.cpp ^
rem     %ProjectDir%\geometrize\rasterizer\*.cpp ^
rem     %ProjectDir%\geometrize\runner\*.cpp ^
rem     %ProjectDir%\geometrize\shape\*.cpp ^
rem     %ProjectDir%\geometrize\*.cpp

cl /TP /std:c++14 /Femain ^
    %Common% ^
    %ProjectDir%\main.cpp ^
    /link bitmap.obj bitmapdataexporter.obj bitmapexporter.obj circle.obj commonutil.obj core.obj ellipse.obj imagerunner.obj line.obj model.obj polyline.obj quadraticbezier.obj rasterizer.obj rectangle.obj rgba.obj rotatedellipse.obj rotatedrectangle.obj scanline.obj shapearrayexporter.obj shapefactory.obj shapejsonexporter.obj shapemutator.obj shapeserializer.obj shapetypes.obj state.obj svgexporter.obj triangle.obj
popd