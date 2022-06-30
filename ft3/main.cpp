#include <cstdio>
#include <spes/image.h>
#include <spes/imageio.h>
#include <memory>
#include <string>
#include <cstdlib>
#include <ft2build.h>
#include <map>
#include <functional>
#include FT_FREETYPE_H
using namespace spes::image;
using namespace spes::image::io;
using namespace std;

template<typename T>
struct ResGuard{
	T res;
	function<void(T&)> td;
	ResGuard(T r, function<void(T&)> t) : res(r), td(t){}
	~ResGuard(){
		td(res);
	}
};

FT_Error err = 0;
shared_ptr<ResGuard<FT_Library>> libg;
map<int, shared_ptr<ResGuard<FT_Face>>> faces;


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

shared_ptr<image_t> render(FT_Face face, long code)
{
	shared_ptr<image_t> rslt = nullptr;
	FT_UInt idx = FT_Get_Char_Index(face, code);

	err = FT_Load_Glyph(face, idx, FT_LOAD_DEFAULT);
	err = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
	if (err) printf("failed render[0x%d] with face %s %s\n", code, face->family_name, face->style_name);
	else rslt = as_image2(face->glyph->bitmap);
	return rslt;
}

void render_and_write(FT_Face face, long code, const string& output)
{
	auto im = render(face, code);
	if(im) write_image(im, output);
}

void render_and_write(long code, const string& output)
{
	for(auto p : faces){
		auto face = p.second->res;
		char buff[256];
		sprintf(buff, "%s.%s%s.png", output.c_str(), face->family_name, face->style_name);
		render_and_write(face, code, buff);
	}
}

bool load_face(const string& path)
{
	FT_Library lib = nullptr;
	err = FT_Init_FreeType(&lib);
	if(err) return false;
	libg = make_shared<ResGuard<FT_Library>>(lib, [](FT_Library& l){
		if(l) FT_Done_FreeType(l);
	});
	
	// detect faces in this file
	FT_Face test = nullptr;
	err = FT_New_Face(lib, path.c_str(), -1, &test);
	printf("detect %d faces in file.\n", test->num_faces);
	
	for(int idx = 0; idx < test->num_faces; ++idx){
		FT_Face face = nullptr;
		err = FT_New_Face(lib, path.c_str(), idx, &face);
		if(err)
		{
			printf("load idx %d failed\n", idx);
			continue;
		}
		printf("loaded idx %d, fam: %s, style: %s\n", idx, face->family_name, face->style_name);
		err = FT_Set_Pixel_Sizes(face, 512, 0); // wid, hgt
		faces[idx] = make_shared<ResGuard<FT_Face>>(face, [](FT_Face& f){
			if(f) FT_Done_Face(f);
		});
	}
	FT_Done_Face(test);
	return true;
}


int main(int argc, char** argv)
{
	if (argc < 4)
	{
		fprintf(stderr, "ft3 font output code...");
		return 1;
	}
	string font = argv[1];
	if(!load_face(font))
	{
		fprintf(stderr, "failed init lib or load font face\n");
		return 1;
	}

	string out = argv[2];
	long code;
	char buff[256];
	for(int i = 3; i < argc; ++i){
		sscanf(argv[i], "%lx", &code);
		sprintf(buff, "%s.%lx", out.c_str(), code);
		printf("using font [%s] render [0x%lx] to [%s]\n", font.c_str(), code, buff);

		render_and_write(code, buff);
	}
    return 0;
}
