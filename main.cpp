#include <vector>
#include "geometrize.cpp"

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

int
main(int ArgCount, char **Args)
{
    char *Path = "sample_images/grapefruit.png";
    geometrize::Bitmap Bitmap = ReadImage(Path);

    int CandidateShapeCount = 256;
    int MaxFitCount = 100;
    geometrize::ShapeTypes ShapeType = geometrize::ELLIPSE;

    geometrize::ImageRunnerOptions Options;
    Options.alpha = 128;
    Options.maxShapeMutations = MaxFitCount;
    Options.shapeCount = CandidateShapeCount;
    Options.shapeTypes = ShapeType;
    Options.seed = 9001;

    geometrize::ImageRunner Runner(Bitmap);
    std::vector<geometrize::ShapeResult> ShapeData;

    for (int i=0; i<CandidateShapeCount; ++i) {
        std::vector<geometrize::ShapeResult> Shapes{Runner.step(Options)};
        for(int j=0; j<Shapes.size(); ++j) {
            printf("Added shape\n");
        }
        std::copy(Shapes.begin(), Shapes.end(), std::back_inserter(ShapeData));
    }

    FILE *F = fopen("test.svg", "w");
    std::string SVG(geometrize::exporter::exportSVG(ShapeData, Bitmap.getWidth(), Bitmap.getHeight()));

    fwrite(SVG.data(), 1, SVG.size(), F);
    fclose(F);

    return 0;
}
