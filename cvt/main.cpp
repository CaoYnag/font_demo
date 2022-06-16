#include <cstdio>
#include <mapbox/glyph_foundry_impl.hpp>
#include <string>
using namespace std;


struct ft_library_guard {
    ft_library_guard(FT_Library lib) :
        library_(lib) {}

    ~ft_library_guard()
    {
        if (library_) FT_Done_FreeType(library_);
    }

    FT_Library library_;
};

struct ft_face_guard {
    ft_face_guard(FT_Face f) :
        face_(f) {}

    ~ft_face_guard()
    {
        if (face_) FT_Done_Face(face_);
    }

    FT_Face face_;
};

string render(const string& ft_path, FT_ULong cp) {
    FT_Library library = nullptr;
    ft_library_guard library_guard(library);
    FT_Error error = FT_Init_FreeType(&library);
    if (error) {
        fprintf(stderr,"could not open FreeType library");
        return "";
    }

    FT_Face ft_face = 0;
    ft_face_guard face_guard(ft_face);
    FT_Error face_error = FT_New_Face(library, ft_path.c_str(), 0, &ft_face);
    if (face_error) {
        fprintf(stderr,"could not open font [%s]", ft_path.c_str());
        return "";
    }

    const double scale_factor = 1.0;

    // Set character sizes.
    double size = 24 * scale_factor;
    FT_Set_Char_Size(ft_face,0,(FT_F26Dot6)(size * (1<<6)),0,0);

    sdf_glyph_foundry::glyph_info glyph;

    // Get FreeType face from face_ptr.
    FT_UInt char_index = FT_Get_Char_Index(ft_face, cp);

    if (!char_index) {
        fprintf(stderr,"could not find a glyph for this code point");
        return "";
    }

    glyph.glyph_index = char_index;
    sdf_glyph_foundry::RenderSDF(glyph, 24, 3, 0.25, ft_face);
    return glyph.bitmap;
}

void write(const string& data, const string& path)
{
    FILE* fp = fopen(path.c_str(), "w+");
    if(!fp)
    {
        fprintf(stderr, "failed open output path [%s]", path.c_str());
        return;
    }
    fwrite(data.c_str(), 1, data.length(), fp);
    fclose(fp);
}

int main(int argc, char** argv)
{
    if(argc < 3)
    {
        printf("Usage: gen font_path code_point [output]\n");
        return 1;
    }
    string path = argv[1];
    int cp = atoi(argv[2]);
    string out = "out";
    if(argc == 4)
        out = argv[3];
    
    write(render(path, cp), out);

    return 0;
}