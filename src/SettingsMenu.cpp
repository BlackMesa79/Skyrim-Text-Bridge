#include <SKSE/SKSE.h>
#include <RE/I/InputEvent.h>
#include "Settings.h"
#include "DirectIme.h"
#include "../extern/SKSEMenuFramework/SKSEMenuFramework.h"
#include <atomic>
namespace TextBridge::SettingsMenu {
namespace {
std::atomic<bool> capturing{false}, menuOpen{false}; bool installed=false; std::string feedback;
std::string Utf8(const std::wstring& text) {
    auto size=WideCharToMultiByte(CP_UTF8,0,text.data(),(int)text.size(),nullptr,0,nullptr,nullptr);
    std::string result(size,0); WideCharToMultiByte(CP_UTF8,0,text.data(),(int)text.size(),result.data(),size,nullptr,nullptr); return result;
}
void __stdcall Render() {
    auto value=Settings::Get(); bool changed=false;
    ImGuiMCP::TextUnformatted("Skyrim Text Bridge");
    ImGuiMCP::TextUnformatted("候选面板现使用游戏内 ImGui HUD 绘制；框架须配置中文字体。");
    changed |= ImGuiMCP::Checkbox("启用模组",&value.enabled);
    changed |= ImGuiMCP::Checkbox("文本框聚焦时自动开启",&value.autoEnable);
    ImGuiMCP::TextUnformatted("可选：按 UI 框架识别 Scaleform 与 Meridian 的有效文本框，无菜单名称白名单。");
    ImGuiMCP::TextUnformatted("Scaleform 包含 SkyUI、RaceMenu 等；仅打开菜单或捏脸界面不会自动开启。");
    ImGuiMCP::TextUnformatted("Prisma 保持原生输入；Modex 等独立 ImGui 界面仍用 F8 手动开启。");
    ImGuiMCP::TextUnformatted("无法可靠识别的界面保留手动开启；新界面的文字送达仍需游戏测试。");
    ImGuiMCP::TextUnformatted("按输入法快捷键手动关闭后，本输入框暂不自动开启；离开后重新聚焦即可恢复。");
    ImGuiMCP::TextUnformatted("开启后跟随当前输入法中英文状态，英文可直接输入；Ctrl + Space 仍可切换。");
    ImGuiMCP::TextUnformatted("也可用输入法自身的中英文切换键；再次按输入法快捷键退出。");
    ImGuiMCP::TextUnformatted("Esc：有拼音时先取消，输入完成后退出；退出时关闭游戏输入法，恢复正常操作。");
    ImGuiMCP::TextUnformatted("输入期间拦截普通键热键，保留鼠标及退格、方向、删除等编辑键。");
    ImGuiMCP::TextUnformatted(("输入法快捷键：" + Utf8(Settings::KeyName(value.hotkey))).c_str());
    if (ImGuiMCP::Button(capturing ? "等待按键（Esc 取消）" : "修改快捷键")) capturing = !capturing.load();
    ImGuiMCP::Separator();
    ImGuiMCP::TextUnformatted("面板位置与大小");
    changed |= ImGuiMCP::SliderFloat("水平位置",&value.x,0,1,"%.2f");
    changed |= ImGuiMCP::SliderFloat("垂直位置",&value.y,0,1,"%.2f");
    changed |= ImGuiMCP::SliderFloat("面板缩放",&value.size,.6f,1.8f,"%.2fx");
    ImGuiMCP::TextUnformatted("位置 0 为左/上，1 为右/下。调整后在输入框开启面板查看。");
    ImGuiMCP::Separator();
    changed |= ImGuiMCP::ColorEdit3("背景颜色",value.background.data());
    changed |= ImGuiMCP::ColorEdit3("强调颜色",value.accent.data());
    changed |= ImGuiMCP::ColorEdit3("文字颜色",value.foreground.data());
    changed |= ImGuiMCP::SliderFloat("背景不透明度",&value.opacity,0,1,"%.2f");
    ImGuiMCP::TextUnformatted("0 完全透明，1 完全不透明；文字保持清晰。");
    if (changed) { Settings::Set(value); feedback="设置已应用，点击保存可在下次启动时保留。"; }
    if (ImGuiMCP::Button("保存设置")) feedback=Settings::Save()?"设置已保存。":"保存失败，请检查配置文件写入权限。";
    ImGuiMCP::SameLine();
    if (ImGuiMCP::Button("恢复默认")) { Settings::Set({}); feedback="已恢复默认，点击保存可保留。"; }
    ImGuiMCP::TextUnformatted(feedback.c_str());
}
void __stdcall Event(SKSEMenuFramework::Model::EventType type) {
    if (type==SKSEMenuFramework::Model::kOpenMenu) { menuOpen=true; DirectIme::Reset(); }
    if (type==SKSEMenuFramework::Model::kCloseMenu) { menuOpen=false; capturing=false; }
}
}
void Install() {
    const auto module=GetModuleHandleW(L"SKSEMenuFramework.dll");
    if (!module) {
        SKSE::log::info("Menu Framework unavailable; settings remain accessible via user INI"); return;
    }
    for (auto name : {"AddSectionItem","IsAnyBlockingWindowOpened","RegisterEvent","igTextUnformatted","igCheckbox","igButton","igSeparator","igSliderFloat","igColorEdit3","igSameLine"})
        if (!GetProcAddress(module,name)) { SKSE::log::warn("Menu Framework settings disabled: missing {}",name); return; }
    SKSEMenuFramework::SetSection("Skyrim Text Bridge");
    SKSEMenuFramework::AddSectionItem("输入法与面板设置",Render);
    using RegisterEvent=std::int64_t (*)(SKSEMenuFramework::Model::EventCallback);
    static auto listener = reinterpret_cast<RegisterEvent>(GetProcAddress(module,"RegisterEvent"))(Event);
    installed=true; SKSE::log::info("Menu Framework settings registered");
}
bool Blocking() { return installed && (menuOpen || SKSEMenuFramework::IsAnyBlockingWindowOpened()); }
bool CaptureKey(unsigned scan) {
    if (!capturing || scan==0 || scan>255) return false;
    capturing=false;
    if (scan!=1) { auto value=Settings::Get(); value.hotkey=scan; Settings::Set(value); }
    return true;
}
}
