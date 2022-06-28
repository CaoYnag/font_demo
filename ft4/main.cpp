#include <cstdio>
#include <spes/image.h>
#include <spes/imageio.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <cstdlib>
#include <ft2build.h>
#include <map>
#include <functional>
#include FT_FREETYPE_H
using namespace spes::image;
using namespace spes::image::io;
using namespace std;


FT_Error err = 0;
FT_Library lib;
FT_Face face;



void render(long code)
{
	shared_ptr<image_t> rslt = nullptr;
	FT_UInt idx = FT_Get_Char_Index(face, code);

	err = FT_Load_Glyph(face, idx, FT_LOAD_DEFAULT);
	err = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);

	printf("FACE INFO:\n");
	printf("num face: %d, current face idx: %d fam: %s style: %s\n", face->num_faces, face->face_index, face->family_name, face->style_name);
	printf("num glyph: %d bbox: (%d, %d, %d, %d)\n", face->num_glyphs, face->bbox.xMin, face->bbox.yMin, face->bbox.xMax, face->bbox.yMax);
	printf("height: %d\n", face->height);

	auto* glyph = face->glyph;
	printf("\nGLYPH INFO:\n");
	printf("advance: (%d, %d)\n", glyph->advance.x, glyph->advance.y);
	printf("metrics size(%d, %d), hor (%d %d %d) vert(%d %d %d)\n",
		glyph->metrics.width, glyph->metrics.height,
		glyph->metrics.horiBearingX, glyph->metrics.horiBearingY, glyph->metrics.horiAdvance,
		glyph->metrics.vertBearingX, glyph->metrics.vertBearingY, glyph->metrics.vertAdvance);
	
	auto& bmp = glyph->bitmap;
	printf("\nBITMAP INFO:\n");
	printf("origin (%d, %d)\n", glyph->bitmap_left, glyph->bitmap_top);
	printf("size: (%d, %d) pitch %d num grey: %d\n", 
		bmp.width, bmp.rows, bmp.pitch, bmp.num_grays);
}

bool init(const string& path)
{
	lib = nullptr;
	err = FT_Init_FreeType(&lib);
	if(err) return false;

	face = nullptr;
	err = FT_New_Face(lib, path.c_str(), 0, &face);
	if(err) return false;
	printf("loaded nums: %d, fam: %s, style: %s\n", face->num_faces, face->family_name, face->style_name);
	err = FT_Set_Char_Size(face, 0, 32 << 6, 0, 0); // 32 * 64
	return true;
}
void release(){
	if(face) FT_Done_Face(face);
	if(lib) FT_Done_FreeType(lib);
}


int main(int argc, char** argv)
{
	if (argc < 3)
	{
		fprintf(stderr, "ft4 font code");
		return 1;
	}
	string font = argv[1];
	long code;
	sscanf(argv[2], "%lx", &code);

	if(init(font))
		render(code);
	
	release();
    return 0;
}
