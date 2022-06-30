#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>
#include <memory>
#include <spes/image.h>
#include <spes/imageio.h>
using namespace std;
using namespace spes::image;
using namespace spes::image::io;
static const double INF = 1e20;

// 1D squared distance transform
void edt1d(vector<double>& f,
           vector<double>& d,
           vector<int16_t>& v,
           vector<double>& z,
           uint32_t n) {
    v[0] = 0;
    z[0] = -INF;
    z[1] = +INF;

    for (uint32_t q = 1, k = 0; q < n; q++) {
        double s = ((f[q] + q * q) - (f[v[k]] + v[k] * v[k])) / (2 * q - 2 * v[k]);
        while (s <= z[k]) {
            k--;
            s = ((f[q] + q * q) - (f[v[k]] + v[k] * v[k])) / (2 * q - 2 * v[k]);
        }
        k++;
        v[k] = q;
        z[k] = s;
        z[k + 1] = +INF;
    }

    for (uint32_t q = 0, k = 0; q < n; q++) {
        while (z[k + 1] < q) k++;
        d[q] = (static_cast<double>(q) - v[k]) * (static_cast<double>(q) - v[k]) + f[v[k]];
    }
}


// 2D Euclidean distance transform by Felzenszwalb & Huttenlocher https://cs.brown.edu/~pff/dt/
void edt(vector<double>& data,
         uint32_t width,
         uint32_t height,
         vector<double>& f,
         vector<double>& d,
         vector<int16_t>& v,
         vector<double>& z) {
    for (uint32_t x = 0; x < width; x++) {
        for (uint32_t y = 0; y < height; y++) {
            f[y] = data[y * width + x];
        }
        edt1d(f, d, v, z, height);
        for (uint32_t y = 0; y < height; y++) {
            data[y * width + x] = d[y];
        }
    }
    for (uint32_t y = 0; y < height; y++) {
        for (uint32_t x = 0; x < width; x++) {
            f[x] = data[y * width + x];
        }
        edt1d(f, d, v, z, width);
        for (uint32_t x = 0; x < width; x++) {
            data[y * width + x] = sqrt(d[x]);
        }
    }
}


shared_ptr<im_buff> transformRasterToSDF(shared_ptr<im_buff> inp, double radius, double cutoff) {
    u32 w = inp->w, h = inp->h;
    uint32_t size = w * h;
    uint32_t maxDimension = max(w, h);
    
    auto rslt = make_shared<im_buff>(w, h, inp->fmt);
    
    // temporary arrays for the distance transform
    vector<double> gridOuter(size);
    vector<double> gridInner(size);
    vector<double> f(maxDimension);
    vector<double> d(maxDimension);
    vector<double> z(maxDimension + 1);
    vector<int16_t> v(maxDimension);
    
    for (uint32_t i = 0; i < size; i++) {
        double a = double(inp->buff[i]) / 255; // alpha value
        gridOuter[i] = a == 1.0 ? 0.0 : a == 0.0 ? INF : pow(max(0.0, 0.5 - a), 2.0);
        gridInner[i] = a == 1.0 ? INF : a == 0.0 ? 0.0 : pow(max(0.0, a - 0.5), 2.0);
    }

    edt(gridOuter, w, h, f, d, v, z);
    edt(gridInner, w, h, f, d, v, z);

    for (uint32_t i = 0; i < size; i++) {
        double distance = gridOuter[i] - gridInner[i];
        rslt->buff[i] = max(0l, min(255l, ::lround(255.0 - 255.0 * (distance / radius + cutoff))));
    }

    return rslt;
}

int main(int argc, char** argv)
{
    if(argc < 2) {
        printf("tiny_sdf inp [rad] [cutoff]");
        return 1;
    }
    double rad = 8., cutoff = .25;
    if(argc > 2) rad = atof(argv[2]);
    if(argc > 3) cutoff = atof(argv[3]);

    const char* file = argv[1];
	int wid, hgt;
	sscanf(file, "%dx%d", &wid, &hgt);

	long sz = wid * hgt;
	auto inp = make_shared<im_buff>(wid, hgt, IMP_FMT_GREY);

	FILE* fp = fopen(file, "r");
	if(fread(inp->buff, 1, sz, fp) < sz)
		printf("read err\n");
	fclose(fp);

	char out[256];
	sprintf(out, "%s.sdf.png", file);
	image_io::write(image_transform(transformRasterToSDF(inp, rad, cutoff)), out, IMAGE_FMT_PNG);
    return 0;
}