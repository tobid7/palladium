#include <pd/smart_ctor.hpp>
#include <pd/NVec.hpp>

#include <vector>
#include <string>

#include <citro3d.h>

namespace Palladium {
    class Texture {
        public:
        Texture() = default;
        ~Texture() = default;
        PD_SMART_CTOR(Texture)

        void Delete();

        void LoadFile(const std::string& path);
        void LoadFromMemory(const std::vector<unsigned char>& data);
        void LoadPixels(const std::vector<unsigned char>& data, int w, int h);

        C3D_Tex* Get() { return this->tex; }
        NVec2 GetTexSize() { return tex_size; }
        NVec2 GetSize() { return img_size; }
        // As the texture is a pow of 2 we need a uv 
        NVec4 GetUV() { return uvs; }

        private:
        void MakeTex(std::vector<unsigned char> &buf, int w, int h);
        C3D_Tex* tex;
        NVec2 img_size;
        NVec2 tex_size;
        NVec4 uvs;
    };
}