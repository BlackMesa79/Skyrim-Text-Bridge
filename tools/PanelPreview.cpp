// Optional development preview. Supply the same ImGui 1.90.8 sources as the
// framework; not part of the plugin or distributable mod.
#define TEXTBRIDGE_PANEL_TEST
#include "../src/ImePanel.cpp"
#include "../src/Settings.cpp"
#undef IM_COL32
#undef IM_COL32_WHITE
#undef IM_COL32_BLACK
#undef IM_COL32_BLACK_TRANS
#include <imgui.h>
#include <fstream>
namespace TextBridge::ImePanel {
namespace {
std::uint64_t TestNow() { return 100; }
bool TestForeground(HWND) { return true; }
void* TestResolve(const char*) { return nullptr; }
ImGuiMCP::ImGuiContext* PreviewContext() { return reinterpret_cast<ImGuiMCP::ImGuiContext*>(ImGui::GetCurrentContext()); }
ImGuiMCP::ImGuiViewport* PreviewViewport() { return reinterpret_cast<ImGuiMCP::ImGuiViewport*>(ImGui::GetMainViewport()); }
ImGuiMCP::ImDrawList* PreviewDrawList() { return reinterpret_cast<ImGuiMCP::ImDrawList*>(ImGui::GetForegroundDrawList()); }
ImGuiMCP::ImFont* PreviewFont() { return reinterpret_cast<ImGuiMCP::ImFont*>(ImGui::GetFont()); }
const ImGuiMCP::ImFontGlyph* PreviewGlyph(ImGuiMCP::ImFont* font, ImGuiMCP::ImWchar c) { return reinterpret_cast<const ImGuiMCP::ImFontGlyph*>(reinterpret_cast<::ImFont*>(font)->FindGlyphNoFallback(c)); }
void PreviewFill(ImGuiMCP::ImDrawList* draw, ImGuiMCP::ImVec2 a, ImGuiMCP::ImVec2 b, ImGuiMCP::ImU32 c, float radius, ImGuiMCP::ImDrawFlags flags) { reinterpret_cast<::ImDrawList*>(draw)->AddRectFilled({a.x,a.y},{b.x,b.y},c,radius,flags); }
void PreviewRect(ImGuiMCP::ImDrawList* draw, ImGuiMCP::ImVec2 a, ImGuiMCP::ImVec2 b, ImGuiMCP::ImU32 c, float radius, ImGuiMCP::ImDrawFlags flags, float thickness) { reinterpret_cast<::ImDrawList*>(draw)->AddRect({a.x,a.y},{b.x,b.y},c,radius,flags,thickness); }
void PreviewText(ImGuiMCP::ImDrawList* draw, const ImGuiMCP::ImFont* font, float size, ImGuiMCP::ImVec2 pos, ImGuiMCP::ImU32 c, const char* start, const char* end, float wrap, const ImGuiMCP::ImVec4* clip) { reinterpret_cast<::ImDrawList*>(draw)->AddText(reinterpret_cast<const ::ImFont*>(font),size,{pos.x,pos.y},c,start,end,wrap,reinterpret_cast<const ::ImVec4*>(clip)); }
void PreviewMeasure(ImGuiMCP::ImVec2* out, ImGuiMCP::ImFont* font, float size, float max, float wrap, const char* start, const char* end, const char** remaining) { const auto result=reinterpret_cast<::ImFont*>(font)->CalcTextSizeA(size,max,wrap,start,end,remaining); *out={result.x,result.y}; }

}
}
int main(int argc,char** argv) {
    if(argc!=3)return 1;
    ImGui::CreateContext(); auto& io=ImGui::GetIO(); io.IniFilename=nullptr; io.DisplaySize={1920,1080}; io.DeltaTime=1.f/60;
    ImFontConfig config; config.OversampleH=1; config.OversampleV=1;
    static const ::ImWchar ranges[]={0x20,0xFF,0x2000,0x206F,0x3000,0x30FF,0x4E00,0x9FFF,0};
    if(!io.Fonts->AddFontFromFileTTF(argv[1],24,&config,ranges))return 2;
    unsigned char* pixels{}; int width{},height{}; io.Fonts->GetTexDataAsRGBA32(&pixels,&width,&height); io.Fonts->SetTexID(reinterpret_cast<::ImTextureID>(1));
    using namespace TextBridge::ImePanel;
    backend.context=PreviewContext; backend.viewport=PreviewViewport; backend.drawList=PreviewDrawList; backend.font=PreviewFont; backend.glyph=PreviewGlyph; backend.rect=PreviewRect; backend.fill=PreviewFill; backend.text=PreviewText; backend.measure=PreviewMeasure;
    TextBridge::Settings::Data value; value.opacity=.55f; value.x=.025f; value.y=.065f; TextBridge::Settings::Set(value);
    Show(reinterpret_cast<HWND>(1),L"中文输入 · F8 关闭\nhaha\n当前输入法：微软拼音\n> 1. 哈哈\n  2. 哈哈大笑\n  3. 蛤蛤\n  4. O(∩_∩)O哈哈~\n  5. 好啊好啊\n  6. Hello");
    ImGui::NewFrame(); TextBridge::ImePanel::Render(); ImGui::Render(); const auto data=ImGui::GetDrawData();
    std::ofstream file(argv[2],std::ios::binary); auto write=[&](const auto& v){file.write(reinterpret_cast<const char*>(&v),sizeof(v));};
    write(std::uint32_t(1));write(std::uint64_t(1));write(width);write(height);file.write(reinterpret_cast<const char*>(pixels),width*height*4);
    write(data->CmdListsCount);
    for(int i=0;i<data->CmdListsCount;++i){const auto list=data->CmdLists[i];write(list->VtxBuffer.Size);write(list->IdxBuffer.Size);write(list->CmdBuffer.Size);file.write(reinterpret_cast<const char*>(list->VtxBuffer.Data),list->VtxBuffer.Size*sizeof(::ImDrawVert));for(auto index:list->IdxBuffer)write(static_cast<std::uint32_t>(index));for(const auto& cmd:list->CmdBuffer){write(cmd.ClipRect);write(cmd.ElemCount);write(cmd.IdxOffset);write(cmd.VtxOffset);write(std::uint64_t(reinterpret_cast<std::uintptr_t>(cmd.TextureId)));}}
    ImGui::DestroyContext();
}
