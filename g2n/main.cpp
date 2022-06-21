#include <cstdio>
#include <spes/image.h>
#include <spes/imageio.h>
using namespace std;
using namespace spes::image;
using namespace spes::image::io;

int main(int argc, char** argv)
{
	if(argc < 2) return 1;
	const char* file = argv[1];
	int wid, hgt;
	sscanf(file, "%dx%d", &wid, &hgt);

	long sz = wid * hgt;
	auto grey = make_shared<im_buff>(wid, hgt, IMP_FMT_GREY);

	FILE* fp = fopen(file, "r");
	if(fread(grey->buff, 1, sz, fp) < sz)
		printf("read err\n");
	fclose(fp);

	char out[256];
	sprintf(out, "%s.png", file);
	image_io::write(image_transform(grey), out, IMAGE_FMT_PNG);
    return 0;
}
