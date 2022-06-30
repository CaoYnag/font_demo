#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>
#include <memory>
#include <spes/image.h>
#include <spes/imageio.h>
#include <filesystem>
using namespace std;
using namespace spes::image;
using namespace spes::image::io;
using namespace std::filesystem;

void cvt(path inf, path of){
    int id, w, h, l, t, a, imw, imh;
    sscanf(inf.stem().c_str(), "%d_%d_%d_%d_%d_%d_%d_%d",
        &id, &w, &h, &l, &t, &a, &imw, &imh);
    char buff[256];
    sprintf(buff, "%s/%03d.png", of.c_str(), id);
    
    FILE* fp = fopen(inf.c_str(), "r");
    if(!fp){
        printf("failed open %s\n", inf.c_str());
        return;
    }
    auto sz = file_size(inf);
    if(sz != (imw * imh)){
        printf("bad file data %d for size(%dx%d)\n", sz, imw, imh);
        return;
    }
    if(sz == 0) return; // empty data
    char data[sz];
    fread(data, 1, sz, fp);
    fclose(fp);
    auto grey = make_shared<im_buff>(imw, imh, IMP_FMT_GREY, (u8*)data);
    auto im = image_transform(grey);
    image_io::write(im, buff);
}

int main(int argc, char** argv)
{
    if(argc < 3) {
        printf("cvt_glyph inp_dir out_dir\n");
        return 1;
    }
    path inp = argv[1];
    path out = argv[2];

    if(!is_directory(inp)) return 0;
    if(!exists(out)) create_directories(out);
    for(const auto& item : directory_iterator{inp}){
        if(is_regular_file(item))
            cvt(item.path(), out);
    }
    return 0;
}