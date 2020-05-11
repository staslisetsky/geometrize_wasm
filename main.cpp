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

// Helper function to read an image file as RGBA8888 pixel data
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

// bool writeImage(const geometrize::Bitmap& bitmap, const std::string& filePath)
// {
//     const char* path{filePath.c_str()};
//     const void* data{bitmap.getDataRef().data()};
//     return stbi_write_png(path, static_cast<int>(bitmap.getWidth()), static_cast<int>(bitmap.getHeight()), 4, data, static_cast<int>(bitmap.getWidth()) * 4) != 0;
// }

// Helper function to convert a string into the Geometrize shape types that the given string names
// e.g. "circle rotated_rectangle" returns the ShapeTypes for those two types of shape (bitwise-or'd together into a single ShapeTypes instance)
// geometrize::ShapeTypes shapeTypesForNames(const std::string& str)
// {
//     // Split string into words based on whitespace
//     std::istringstream iss(str);
//     const std::vector<std::string> shapeNames(std::istream_iterator<std::string>{iss},
//                                      std::istream_iterator<std::string>());

//     std::vector<geometrize::ShapeTypes> shapeTypes;

//     // Convert the shape names into ShapeTypes
//     for(const std::string& shapeName : shapeNames) {
//         for(const std::pair<geometrize::ShapeTypes, std::string>& p : geometrize::shapeTypeNames) {
//             if(p.second == shapeName) {
//                 shapeTypes.push_back(p.first);
//             }
//         }
//     }

//     if(shapeTypes.empty()) {
//         std::cout << "Bad shape names provided, defaulting to ellipses \n";
//         return geometrize::ELLIPSE;
//     }

//     // Combine shape types together
//     std::underlying_type<geometrize::ShapeTypes>::type combinedShapeTypes = 0;
//     for (const auto& shapeType : shapeTypes) {
//         combinedShapeTypes |= shapeType;
//     }
//     return geometrize::ShapeTypes(combinedShapeTypes);
// }

// // Helper function to convert a Geometrize shape type to a human-readable string
// std::string shapeNameForType(const geometrize::ShapeTypes type)
// {
//     for(const std::pair<geometrize::ShapeTypes, std::string>& p : geometrize::shapeTypeNames) {
//         if(p.first == type) {
//             return p.second;
//         }
//     }
//     return "unknown_shape";
// }

int
main(int ArgCount, char **Args)
{
    char *Path = "sample_images/grapefruit.png";
    geometrize::Bitmap Bitmap = ReadImage(Path);

    int ShapeCount = 10;
    int MaxShapeMutations = 1;
    geometrize::ShapeTypes ShapeType = geometrize::ELLIPSE;

    geometrize::ImageRunnerOptions Options;
    Options.alpha = true;
    Options.maxShapeMutations = MaxShapeMutations;
    Options.shapeCount = ShapeCount;
    Options.shapeTypes = ShapeType;

    geometrize::ImageRunner Runner(Bitmap);
    std::vector<geometrize::ShapeResult> ShapeData;

    for (int i=0; i<ShapeCount; ++i) {
        std::vector<geometrize::ShapeResult> Shapes{Runner.step(Options)};
        for(int j=0; j<Shapes.size(); ++j) {
            printf("Added shape\n");
        }

        std::copy(Shapes.begin(), Shapes.end(), std::back_inserter(ShapeData));
    }

    // Save the geometrized shape data (use the extension to figure out the file type)
    // const std::string outFilePath{outputFileFlag.Get()};

    // const auto endsWith = [](const std::string& str, const std::string& ending) {
    //     if (str.length() >= ending.length()) {
    //         return (0 == str.compare (str.length() - ending.length(), ending.length(), ending));
    //     }
    //     return false;
    // };

    // const auto writeStringToFile = [](const std::string& outFilePath, const std::string& data) {
    //     std::ofstream outFile(outFilePath);
    //     outFile << data;
    //     return outFile.is_open();
    // };

    // if(endsWith(outFilePath, ".svg")) {
    //     if(!writeStringToFile(outFilePath, geometrize::exporter::exportSVG(shapeData, bitmap.getWidth(), bitmap.getHeight()))) {
    //         std::cout << "Failed to write SVG file to: " << outFilePath;
    //         return 2;
    //     }
    // } else if(endsWith(outFilePath, ".json")) {
    //     if(!writeStringToFile(outFilePath, geometrize::exporter::exportShapeJson(shapeData))) {
    //         std::cout << "Failed to write JSON file to: " << outFilePath;
    //         return 2;
    //     }
    // } else {
    //     if(!writeImage(runner.getCurrent(), outFilePath)) {
    //         std::cout << "Failed to write image to: " << outFilePath;
    //         return 3;
    //     }
    // }

    // std::cout << "Wrote output to: " << outFilePath;

    return 0;
}


// Hack to add a single background rectangle as the initial shape
// const auto shape = geometrize::create(geometrize::ShapeTypes::RECTANGLE);
// geometrize::Rectangle* rect = dynamic_cast<geometrize::Rectangle*>(shape.get());
// rect->m_x1 = 0;
// rect->m_y1 = 0;
// rect->m_x2 = static_cast<float>(runner.getCurrent().getWidth());
// rect->m_y2 = static_cast<float>(runner.getCurrent().getHeight());
// shapeData.emplace_back(geometrize::ShapeResult{0, geometrize::commonutil::getAverageImageColor(bitmap), shape });