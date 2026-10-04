#include <SKSE/SKSE.h>
#include <RE/C/ControlMap.h>
#include "GameCharacterQueue.h"
#include <iostream>
#include <stdexcept>
#include <array>
#include <cstring>
namespace {
unsigned checks{};
void Check(bool condition, const char* label) {
    if (!condition) throw std::runtime_error(label);
    ++checks;
}
}
int wmain(int argc, wchar_t** argv) {
    if (argc != 2) return 1;
    const std::filesystem::path addressRoot(argv[1]);
    struct Fixture { REL::Version version; REL::Module::Runtime runtime; const wchar_t* file; std::size_t head, characters, control; std::uint64_t poll, queue, ui, controlID; std::size_t pollOffset; };
    const std::array fixtures{
        Fixture{{1,5,97,0}, REL::Module::Runtime::SE, L"version-1-5-97-0.bin", 0x380,0x200,0xE8,67315,520856,514178,514705,0xC150B0},
        Fixture{{1,6,1170,0}, REL::Module::Runtime::AE, L"versionlib-1-6-1170-0.bin", 0x380,0x200,0xF0,68617,407374,400327,400863,0xCD8F40},
        Fixture{{1,7,104,0}, REL::Module::Runtime::AE, L"versionlib-1-7-104-0.bin", 0x558,0x208,0xF0,68617,407374,400327,400863,0xCF95F0}
    };
    for (const auto& f : fixtures) {
        Check(REL::Module::mock(f.version,f.runtime,L"TextBridgeQueueTest.exe",0), "mock runtime without launching Skyrim");
        // Auto-detect exactly the same format-1/2/5 loader used in production.
        Check(REL::IDDB::inject((addressRoot/f.file).wstring(), f.version), "load actual runtime address library");
        Check(REL::ID(f.poll).offset()==f.pollOffset, "resolve dispatch relocation");
        Check(REL::ID(f.queue).offset() && REL::ID(f.ui).offset() && REL::ID(f.controlID).offset(), "resolve queue and focus singletons");
        alignas(16) std::array<std::byte,0x600> bytes{};
        auto& queue = *reinterpret_cast<RE::BSInputEventQueue*>(bytes.data());
        Check(reinterpret_cast<std::byte*>(&queue.GetQueueHead())==bytes.data()+f.head &&
              reinterpret_cast<std::byte*>(&queue.GetQueueTail())==bytes.data()+f.head+8, "queue links use runtime layout");
        auto oldHead=reinterpret_cast<RE::InputEvent*>(bytes.data()+0x580);
        auto oldTail=reinterpret_cast<RE::InputEvent*>(bytes.data()+0x5A0);
        queue.GetQueueHead()=oldHead; queue.GetQueueTail()=oldTail;
        if (f.version[1]==7) std::fill(bytes.begin()+0x388,bytes.begin()+0x558,std::byte{0xA5});
        const std::array<std::uint32_t,3> text{0x4E2D,0x1F600,'W'};
        queue.charEventCount=2;
        Check(TextBridge::AvailableCharacters(queue)==3, "existing character pool limits capacity");
        const auto first=TextBridge::AppendGameCharacters(queue,text);
        Check(queue.GetQueueHead()==oldHead && queue.GetQueueTail()==oldTail, "append restores preexisting engine chain");
        Check(queue.charEventCount==5 && TextBridge::AvailableCharacters(queue)==0, "commit advances bounded character count");
        auto event=first;
        for (unsigned i=0;i<text.size();++i) {
            Check(reinterpret_cast<std::byte*>(event)==bytes.data()+f.characters+(i+2)*0x20, "characters occupy correct runtime pool slots");
            Check(static_cast<RE::CharEvent*>(event)->keyCode==text[i], "Unicode and English preserved in append order");
            event=event->next;
        }
        Check(!event, "appended chain terminates");
        Check(!TextBridge::AppendGameCharacters(queue,{}) && queue.GetQueueHead()==oldHead && queue.GetQueueTail()==oldTail, "full pool does not disturb existing chain");
        if (f.version[1]==7) Check(std::all_of(bytes.begin()+0x388,bytes.begin()+0x558,[](auto b){return b==std::byte{0xA5};}), "new 1.7 event pools remain intact");
        alignas(16) std::array<std::byte,0x150> controls{};
        auto& controlMap=*reinterpret_cast<RE::ControlMap*>(controls.data());
        Check(reinterpret_cast<std::byte*>(&controlMap.GetRuntimeData())==controls.data()+f.control, "SkyUI focus state uses correct runtime layout");
        std::cout << "Queue and address loader verified for " << f.version.string() << '\n';
        REL::IDDB::reset(); REL::Module::reset();
    }
    std::cout << checks << " runtime queue/address checks passed (no game entry points executed)\n";
}
