// Well yes, i finally try it

#include <pd/lithium/formatters.hpp>
#include <pd_system/gfx_directx9.hpp>

// Sicher ist sicher
#if defined(PD_ENABLE_DIRECTX9) && defined(_WIN32)
#include <d3d9.h>
#include <d3dcompiler.h>

#include <pd/drivers/drivers.hpp>

namespace PD {
static const char* g_vsCode = R"(
float4x4 projection;

struct VS_IN {
    float2 pos : POSITION0;
    float2 uv  : TEXCOORD0;
    float4 col : COLOR0;
};

struct VS_OUT {
    float4 pos : POSITION0;
    float2 uv  : TEXCOORD0;
    float4 col : COLOR0;
};

VS_OUT main(VS_IN input) {
    VS_OUT o;
    o.pos = mul(projection, float4(input.pos, 0.0, 1.0)); 
    o.uv = input.uv;
    o.col = input.col;
    return o;
}
)";

static const char* g_psCode = R"(
sampler2D tex : register(s0);
float alfa;

struct PS_IN {
    float2 uv  : TEXCOORD0;
    float4 col : COLOR0;
};

float4 main(PS_IN input) : COLOR0 {
    float4 tc = tex2D(tex, input.uv);
    if (alfa > 0.5)
        return float4(input.col.rgb, tc.a * input.col.a);
    else
        return tc * input.col;
}
)";

struct GfxDirectX9::Impl {
  IDirect3DDevice9* Device = nullptr;
  IDirect3DVertexBuffer9* VBO = nullptr;
  IDirect3DIndexBuffer9* IBO = nullptr;
  IDirect3DVertexDeclaration9* Decl = nullptr;
  IDirect3DVertexShader9* VS = nullptr;
  IDirect3DPixelShader9* FS = nullptr;
  IDirect3DTexture9* CurrentTex = nullptr;
  size_t VertexBufferSize = 0;
  size_t IndexBufferSize = 0;
};

void GfxDirectX9::SysInit() {
  if (impl) return;
  PDLOG("GfxDirectX9::SysInit();");
  impl = new Impl();
  impl->Device = (IDirect3DDevice9*)pDevice;
  if (impl->Device) {
    D3DVERTEXELEMENT9 elements[]{
        {0, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION,
         0},
        {0, 8, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,
         0},
        {0, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,
         0},
        D3DDECL_END()};
    impl->Device->CreateVertexDeclaration(elements, &impl->Decl);

    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* errBlob = nullptr;
    HRESULT hr = D3DCompile(g_vsCode, strlen(g_vsCode), nullptr, nullptr,
                            nullptr, "main", "vs_2_0", 0, 0, &vsBlob, &errBlob);
    if (FAILED(hr)) {
      PDLOG("Vertex Shader compile error: {}",
            errBlob ? (char*)errBlob->GetBufferPointer() : "");
    } else {
      impl->Device->CreateVertexShader((DWORD*)vsBlob->GetBufferPointer(),
                                       &impl->VS);
    }
    if (vsBlob) vsBlob->Release();
    if (errBlob) errBlob->Release();

    ID3DBlob* psBlob = nullptr;
    errBlob = nullptr;
    hr = D3DCompile(g_psCode, strlen(g_psCode), nullptr, nullptr, nullptr,
                    "main", "ps_2_0", 0, 0, &psBlob, &errBlob);
    if (FAILED(hr)) {
      PDLOG("Pixel Shader compile error: {}",
            errBlob ? (char*)errBlob->GetBufferPointer() : "");
    } else {
      impl->Device->CreatePixelShader((DWORD*)psBlob->GetBufferPointer(),
                                      &impl->FS);
    }
    if (psBlob) psBlob->Release();
    if (errBlob) errBlob->Release();
  } else {
    PDLOG(
        "GfxDirectX9::SysInit Error: pDevice is not set!\nYOu need to include "
        "your D3D9 Device as "
        "folowing:\nPD::Gfx::UseDriver<PD::GfxDirectX9>(D3D9Device);");
  }
}

void GfxDirectX9::SysDeinit() {
  if (!impl || !impl->Device) return;
  if (impl->VBO) impl->VBO->Release();
  if (impl->IBO) impl->IBO->Release();
  if (impl->Decl) impl->Decl->Release();
  if (impl->VS) impl->VS->Release();
  if (impl->FS) impl->FS->Release();
  delete impl;
  impl = nullptr;
  PDLOG("GfxDirectX9::SysDeinit()");
}

void GfxDirectX9::Submit(size_t count, size_t start) {
  if (!impl || !impl->Device || !impl->VBO || !impl->IBO) return;

  BindTexture(CurrentTex);
  impl->Device->SetVertexShaderConstantF(
      0, reinterpret_cast<const float*>(&Projection), 4);

  if (!impl->VBO || impl->VertexBufferSize != GetVertexPoolSize()) {
    if (impl->VBO) {
      impl->VBO->Release();
      impl->VBO = nullptr;
      impl->VertexBufferSize = 0;
    }
    if (impl->Device->CreateVertexBuffer(
            GetVertexPoolSize() * sizeof(Li::Vertex),
            D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT,
            &impl->VBO, nullptr) < 0)
      return;
    impl->VertexBufferSize = GetVertexPoolSize();
  }
  if (!impl->IBO || impl->IndexBufferSize != GetIndexPoolSize()) {
    if (impl->IBO) {
      impl->IBO->Release();
      impl->IBO = nullptr;
      impl->IndexBufferSize = 0;
    }
    if (impl->Device->CreateIndexBuffer(GetVertexPoolSize() * sizeof(u16),
                                        D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
                                        D3DFMT_INDEX16, D3DPOOL_DEFAULT,
                                        &impl->IBO, nullptr) < 0)
      return;
    impl->IndexBufferSize = GetIndexPoolSize();
  }
  void* vptr;
  impl->VBO->Lock(0, 0, &vptr, D3DLOCK_DISCARD);
  memcpy(vptr, GetVertexBufPtr(0), CurrentVertex * sizeof(PD::Li::Vertex));
  impl->VBO->Unlock();

  void* iptr;
  impl->IBO->Lock(0, 0, &iptr, D3DLOCK_DISCARD);
  memcpy(iptr, GetIndexBufPtr(0), CurrentIndex * sizeof(u16));
  impl->IBO->Unlock();

  impl->Device->SetStreamSource(0, impl->VBO, 0, sizeof(PD::Li::Vertex));
  impl->Device->SetIndices(impl->IBO);
  impl->Device->SetVertexDeclaration(impl->Decl);
  impl->Device->SetVertexShader(impl->VS);
  impl->Device->SetPixelShader(impl->FS);

  impl->Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, CurrentVertex,
                                     start, count / 3);
}

void GfxDirectX9::BindTexture(TextureID id) {
  if (!impl || !impl->Device) return;
  impl->CurrentTex = (IDirect3DTexture9*)id;
  impl->Device->SetTexture(0, impl->CurrentTex);

  bool a8 = false;
  if (impl->CurrentTex) {
    D3DSURFACE_DESC desc;
    impl->CurrentTex->GetLevelDesc(0, &desc);
    a8 = (desc.Format == D3DFMT_A8);
  }

  float v = a8 ? 1.0f : 0.0f;
  impl->Device->SetPixelShaderConstantF(0, &v, 1);
}

void GfxDirectX9::SysReset() {
  if (!impl || !impl->Device) return;

  impl->Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
  impl->Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
  impl->Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

Li::Texture GfxDirectX9::LoadTexture(const std::vector<PD::u8>& pixels, int w,
                                     int h, TextureFormat type,
                                     TextureFilter filter) {
  if (!impl || !impl->Device) return Li::Texture();
  IDirect3DTexture9* tex = nullptr;
  D3DFORMAT fmt = D3DFMT_A8R8G8B8;
  if (type == TextureFormat::RGB24)
    fmt = D3DFMT_X8R8G8B8;
  else if (type == TextureFormat::A8)
    fmt = D3DFMT_A8;

  HRESULT hr = impl->Device->CreateTexture(w, h, 1, 0, fmt, D3DPOOL_MANAGED,
                                           &tex, nullptr);
  if (FAILED(hr) || !tex) return Li::Texture();

  D3DLOCKED_RECT rect;
  tex->LockRect(0, &rect, nullptr, 0);

  if (type == TextureFormat::RGB24) {
    u8* dstRow = reinterpret_cast<u8*>(rect.pBits);
    for (int y = 0; y < h; ++y) {
      u32* dst = reinterpret_cast<u32*>(dstRow);
      for (int x = 0; x < w; ++x) {
        u8 r = pixels[(y * w + x) * 3 + 0];
        u8 g = pixels[(y * w + x) * 3 + 1];
        u8 b = pixels[(y * w + x) * 3 + 2];
        dst[x] = (0xFF << 24) | (r << 16) | (g << 8) | b;
      }
      dstRow += rect.Pitch;
    }
  } else if (type == TextureFormat::RGBA32) {
    u8* dstRow = reinterpret_cast<u8*>(rect.pBits);
    for (int y = 0; y < h; ++y) {
      u32* dst = reinterpret_cast<u32*>(dstRow);
      for (int x = 0; x < w; ++x) {
        u8 r = pixels[(y * w + x) * 4 + 0];
        u8 g = pixels[(y * w + x) * 4 + 1];
        u8 b = pixels[(y * w + x) * 4 + 2];
        u8 a = pixels[(y * w + x) * 4 + 3];
        dst[x] = (a << 24) | (r << 16) | (g << 8) | b;
      }
      dstRow += rect.Pitch;
    }
  } else if (type == TextureFormat::A8) {
    u8* dstRow = reinterpret_cast<u8*>(rect.pBits);
    for (int y = 0; y < h; ++y) {
      memcpy(dstRow, &pixels[y * w], w);
      dstRow += rect.Pitch;
    }
  }

  tex->UnlockRect(0);
  Li::Texture res;
  res.SetID((TextureID)tex);
  res.SetSize(w, h);
  res.SetUV(0.f, 0.f, 1.f, 1.f);
  RegisterTexture(res);
  PDLOG("GfxDirectX9::LoadTexture -> {{ {} }}, [{}, {}]", res, type, filter);
  return res;
}

void GfxDirectX9::DeleteTexture(const Li::Texture& tex) {
  if (!tex.GetID()) return;
  UnregisterTexture(tex);
  IDirect3DTexture9* t = (IDirect3DTexture9*)tex.GetID();
  t->Release();
}
}  // namespace PD
#else
namespace PD {
void GfxDirectX9::SysInit() {
  PDLOG(
      "GfxDirectX9::SysInit: DirectX9 Driver is not included in "
      "palladium-system");
}
void GfxDirectX9::SysDeinit() {}
void GfxDirectX9::Submit(size_t count, size_t start) {}
void GfxDirectX9::BindTexture(TextureID id) {}
void GfxDirectX9::SysReset() {}
Li::Texture GfxDirectX9::LoadTexture(const std::vector<PD::u8>& pixels, int w,
                                     int h, TextureFormat type,
                                     TextureFilter filter) {
  return Li::Texture();
}
void GfxDirectX9::DeleteTexture(const Li::Texture& tex) {}
}  // namespace PD
#endif