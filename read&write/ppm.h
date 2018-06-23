//================================//
//=== Portable Pixel Map (PPM) ===//
//================================//

#include <string>
#include <fstream>
#include <vector>

namespace ppm
{
    void hsv_to_rgb(const float h, const float s, const float v,
                    float &r, float &g, float &b)
    {
        int i = int(h * 6.0);
        float f = h * 6.0 - i;
        float p = v * (1.0 - s);
        float q = v * (1.0 - f * s);
        float t = v * (1.0 - (1.0 - f) * s);
        
        switch(i % 6)
        {
            case 0: r = v, g = t, b = p; break;
            case 1: r = q, g = v, b = p; break;
            case 2: r = p, g = v, b = t; break;
            case 3: r = p, g = q, b = v; break;
            case 4: r = t, g = p, b = v; break;
            default: r = v, g = p, b = q;
        }
    }
    
    bool write(const std::string filename,
               uint8_t const * const image,
               const int width, const int height,
               const float scale=1.0, const float offset=0.0)
    {
        std::ofstream img_stream(filename);
        img_stream << "P3\n"; // RGB
        img_stream << width << " " << height << "\n";
        img_stream << "255\n"; //Num-values per colour.
        
        for (int y=0; y<height; ++y)
        {
            for (int x=0; x<width; ++x)
            {
                float c = (image[x + y*width]*scale + offset + 0.5)/256.0;
                float r,g,b;
                
                //r = g = b = c;
                hsv_to_rgb(c, 1.0, 1.0, r, g, b);
                
                int ir = r * 256;
                if (ir > 255) ir=255;
                int ig = g * 256;
                if (ig > 255) ig=255;
                int ib = b * 256;
                if (ib > 255) ib=255;
                
                img_stream << ir << " " << ig << " " << ib << "  ";
            }
            img_stream << "\n";
        }
        
        img_stream.flush();
        img_stream.close();
        
        return true;
    }
    
    bool write(const std::string filename,
               const int H,
               const std::vector<int> image_vec,
               const float scale=1.0, const float offset=0.0)
    {
        const int W = image_vec.size()/H;
        
        uint8_t * const image_buf = new uint8_t[W * H];
        
        for (int y=0; y<H; ++y)
        {
            for (int x=0; x<W; ++x)
            {
                image_buf[y*W + x] = image_vec[y*W + x];
            }
        }
        
        const bool status = write(filename, image_buf, W, H, scale, offset);
        
        delete [] image_buf;
        
        return status;
    }
}
