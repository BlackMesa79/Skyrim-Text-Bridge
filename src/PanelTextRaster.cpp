#include "PanelTextRaster.h"
#include <Windows.h>
#include <dwrite.h>
#include <d2d1_1.h>
#include <d3d11.h>
#include <wincodec.h>
#include <wrl/client.h>
#include <cmath>
#include <algorithm>
#include <string>
namespace TextBridge::PanelTextRaster {
namespace {
using Microsoft::WRL::ComPtr;
// COM and graphics objects are confined to the HUD/render thread. Their
// destructor releases owned resources before balancing our apartment reference.
struct Renderer {
    HRESULT apartment = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    ComPtr<IDWriteFactory> write;
    ComPtr<ID2D1Factory1> factory;
    ComPtr<ID3D11Device> device;
    ComPtr<ID2D1Device> d2dDevice;
    ComPtr<ID2D1DeviceContext> context;
    Renderer() {
        if (FAILED(apartment) && apartment != RPC_E_CHANGED_MODE) return;
        if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown**>(write.GetAddressOf())))) return;
        D2D1_FACTORY_OPTIONS options{};
        if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory1),
            &options, reinterpret_cast<void**>(factory.GetAddressOf())))) return;
        if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT,
            nullptr, 0, D3D11_SDK_VERSION, &device, nullptr, nullptr))) return;
        ComPtr<IDXGIDevice> dxgi;
        if (FAILED(device.As(&dxgi)) || FAILED(factory->CreateDevice(dxgi.Get(), &d2dDevice))) return;
        d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &context);
    }
    ~Renderer() {
        context.Reset(); d2dDevice.Reset(); device.Reset(); factory.Reset(); write.Reset();
        if (SUCCEEDED(apartment)) CoUninitialize();
    }
};
thread_local Renderer renderer;
}
Bitmap Render(std::string_view text, float size, unsigned width, unsigned height, std::uint32_t color) {
    Bitmap result;
    if (text.empty() || text.size() > 16384 || !std::isfinite(size) || size < 1 || size > 128 ||
        !width || width > 2048 || !height || height > 256 || !renderer.context || !renderer.write) return result;
    const int length = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text.data(), static_cast<int>(text.size()), nullptr, 0);
    if (!length) return result;
    std::wstring wide(length, 0);
    if (!MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text.data(), static_cast<int>(text.size()), wide.data(), length)) return result;
    ComPtr<IDWriteTextFormat> format;
    // System fallback shapes supplementary emoji, variation selectors and ZWJ
    // sequences together. No bundled font files or 16-bit glyph substitution.
    if (FAILED(renderer.write->CreateTextFormat(L"Segoe UI", nullptr, DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, size, L"zh-CN", &format))) return result;
    format->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    DWRITE_TRIMMING trimming{DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0};
    ComPtr<IDWriteInlineObject> ellipsis;
    if (SUCCEEDED(renderer.write->CreateEllipsisTrimmingSign(format.Get(), &ellipsis))) format->SetTrimming(&trimming, ellipsis.Get());
    ComPtr<IDWriteTextLayout> layout;
    if (FAILED(renderer.write->CreateTextLayout(wide.data(), length, format.Get(), float(width), float(height), &layout))) return result;
    const auto targetProperties = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
    ComPtr<ID2D1Bitmap1> target;
    if (FAILED(renderer.context->CreateBitmap(D2D1::SizeU(width, height), nullptr, 0, targetProperties, &target))) return result;
    ComPtr<ID2D1SolidColorBrush> brush;
    if (FAILED(renderer.context->CreateSolidColorBrush(D2D1::ColorF(float(color & 255) / 255,
        float((color >> 8) & 255) / 255, float((color >> 16) & 255) / 255, float(color >> 24) / 255), &brush))) return result;
    renderer.context->SetTarget(target.Get());
    renderer.context->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
    renderer.context->BeginDraw();
    renderer.context->Clear(D2D1::ColorF(0, 0, 0, 0));
    renderer.context->DrawTextLayout(D2D1::Point2F(0, 0), layout.Get(), brush.Get(),
        D2D1_DRAW_TEXT_OPTIONS_CLIP | D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
    const auto drawn = renderer.context->EndDraw();
    renderer.context->SetTarget(nullptr);
    if (FAILED(drawn)) return result;
    const auto readProperties = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_CPU_READ | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
    ComPtr<ID2D1Bitmap1> readable;
    if (FAILED(renderer.context->CreateBitmap(D2D1::SizeU(width, height), nullptr, 0, readProperties, &readable)) ||
        FAILED(readable->CopyFromBitmap(nullptr, target.Get(), nullptr))) return result;
    D2D1_MAPPED_RECT mapped{};
    if (FAILED(readable->Map(D2D1_MAP_OPTIONS_READ, &mapped))) return result;
    result.width = width; result.height = height; result.rgba.resize(width * height * 4);
    for (unsigned y = 0; y < height; ++y) for (unsigned x = 0; x < width; ++x) {
        const auto source = mapped.bits + y * mapped.pitch + x * 4;
        auto dest = result.rgba.data() + (y * width + x) * 4;
        for (unsigned c = 0; c < 3; ++c) dest[c] = source[3] ? std::min(255u, unsigned(source[2 - c]) * 255u / source[3]) : 0;
        dest[3] = source[3];
    }
    readable->Unmap(); return result;
}
bool SavePng(const Bitmap& image, const wchar_t* path) {
    if (!image || !path) return false;
    ComPtr<IWICImagingFactory> factory;
    if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory)))) return false;
    ComPtr<IWICStream> stream;
    ComPtr<IWICBitmapEncoder> encoder;
    ComPtr<IWICBitmapFrameEncode> frame;
    if (FAILED(factory->CreateStream(&stream)) || FAILED(stream->InitializeFromFilename(path, GENERIC_WRITE)) ||
        FAILED(factory->CreateEncoder(GUID_ContainerFormatPng, nullptr, &encoder)) ||
        FAILED(encoder->Initialize(stream.Get(), WICBitmapEncoderNoCache)) || FAILED(encoder->CreateNewFrame(&frame, nullptr)) ||
        FAILED(frame->Initialize(nullptr)) || FAILED(frame->SetSize(image.width, image.height))) return false;
    // Let WIC convert from our straight RGBA buffer to the encoder's supported
    // format instead of assuming that its negotiated PNG layout is RGBA.
    ComPtr<IWICBitmap> bitmap;
    if (FAILED(factory->CreateBitmapFromMemory(image.width, image.height, GUID_WICPixelFormat32bppRGBA,
        image.width * 4, static_cast<UINT>(image.rgba.size()), const_cast<BYTE*>(image.rgba.data()), &bitmap))) return false;
    if (FAILED(frame->WriteSource(bitmap.Get(), nullptr))) return false;
    return SUCCEEDED(frame->Commit()) && SUCCEEDED(encoder->Commit());
}
}
