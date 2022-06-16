#include <cstdio>
#include <ft2build.h>
#include FT_FREETYPE_H

int main()
{
    FT_Library lib;
	FT_Face face;
	FT_UInt idx;
	FT_GlyphSlot slot;
	FT_Error err;
	// 1. init library
	err = FT_Init_FreeType(&lib);
    if(err)
    {
        perror("failed init freetype.\n");
        return 1;
    }
	// 2, load font
	err = FT_New_Face(lib, "fonts/luculent.ttf",
					  0, // face index, in some format, a single file have several fonts
					  &face);
	if(err == FT_Err_Unknown_File_Format)
	{
		fprintf(stderr, "unknown file format\n");
		goto quit;
	}
	else if(err)
	{
		fprintf(stderr, "failed load font\n");
		goto quit;
	}

	// load succ, print meta
	printf("num glyph: %d\n", face->num_glyphs);

	// 3. set custom pixel size
	err = FT_Set_Char_Size(
		face,
		0,		// char wid in 1/64 of points
		16*64,	// char hgt in 1/64 of points
		300,	// dev hor resolution
		300		// dev ver resolution
		);

	// or set pixel directly:
	// err = FT_Set_Pixel_Sizes(face, 0, 16); // wid, hgt
	
	// 4. got glyph index of a char
	idx = FT_Get_Char_Index(face, 0x41);

	// 5. load glyph
	err = FT_Load_Glyph(face, idx, FT_LOAD_DEFAULT);
	// 6. render
	err = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
	// optional, transform
	// err = FT_Set_Transform(face, &matrix, &delta);

	// 7. use rendered data
	slot = face->glyph;
	printf("advanced: (%d, %d) bitmap: (%dx%d, %d)\n", slot->advance.x, slot->advance.y, slot->bitmap.width, slot->bitmap.rows, slot->bitmap.pixel_mode);
quit: // release resource here
	if(face)
		FT_Done_Face(face);
	if(lib)
		FT_Done_FreeType(lib);
    return 0;
}
