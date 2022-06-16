#include <cstdio>
#include <spes/image.h>
#include <spes/imageio.h>
#include <memory>
#include <string>
#include <cstdlib>
#include <ft2build.h>
#include FT_FREETYPE_H
using namespace spes::image;
using namespace spes::image::io;
using namespace std;

void write_image(shared_ptr<image_t> im, const string& path)
{
	image_io::write(im, path.c_str(), IMAGE_FMT_PNG);
}

shared_ptr<image_t> as_image(const FT_Bitmap& bitmap)
{
	int wid = bitmap.width;
	int hgt = bitmap.rows;
	auto grey = make_shared<im_buff>(wid, hgt, IMP_FMT_GREY);
	memcpy(grey->buff, bitmap.buffer, wid * hgt);
	return image_transform(grey);
}

shared_ptr<image_t> as_image2(const FT_Bitmap& bitmap)
{
	int wid = bitmap.width;
	int hgt = bitmap.rows;
	auto im = make_shared<image_t>();
	im->init(wid, hgt);
	// default use RED stroke
	constexpr const int R = 255, G = 0, B = 0;
	int idx = 0;
	for(int y = 0; y < hgt; ++y)
	{
		for(int x = 0; x < wid; ++x)
		{
			color_t c(R, G, B, bitmap.buffer[idx]);
			im->set_pixel(x, y, c);
			++idx;
		}
	}
	return im;
}

shared_ptr<image_t> render(const string& path, FT_ULong cp)
{
	FT_Library lib;
	FT_Face face;
	FT_UInt idx;
	FT_GlyphSlot slot;
	FT_Error err;
	shared_ptr<image_t> rslt = nullptr;
	// 1. init library
	err = FT_Init_FreeType(&lib);
    if(err)
    {
        perror("failed init freetype.\n");
        return rslt;
    }
	// 2, load font
	err = FT_New_Face(lib,
					  path.c_str(),
					  0, // face index, in some format, a single file have several fonts
					  &face);
	if(err == FT_Err_Unknown_File_Format)
	{
		fprintf(stderr, "unknown file format [%s]\n", path.c_str());
		goto quit;
	}
	else if(err)
	{
		fprintf(stderr, "failed load font:%d [%s]\n", err, path.c_str());
		goto quit;
	}

	err = FT_Set_Pixel_Sizes(face, 512, 0); // wid, hgt
	
	// 4. got glyph index of a char
	idx = FT_Get_Char_Index(face, cp);

	// 5. load glyph
	err = FT_Load_Glyph(face, idx, FT_LOAD_DEFAULT);
	// 6. render
	err = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
	if (err)
	{
		fprintf(stderr, "render error: %d\n", err);
		goto quit;
	}
	// 7. use rendered data
	slot = face->glyph;
	rslt = as_image2(face->glyph->bitmap);
quit: // release resource here
	if(face)
		FT_Done_Face(face);
	if(lib)
		FT_Done_FreeType(lib);
	return rslt;
}

int main(int argc, char** argv)
{
	if (argc < 4)
	{
		fprintf(stderr, "ft2 font code output");
		return 1;
	}
	string font = argv[1];
	long code;
	string out = argv[3];
	sscanf(argv[2], "%lx", &code);

	printf("using font [%s] render [0x%lx] to [%s]\n", font.c_str(), code, out.c_str());
	
	auto im = render(font, code);
	write_image(im, out);
    return 0;
}
