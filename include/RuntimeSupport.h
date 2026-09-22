#pragma once
namespace TextBridge {
constexpr bool SupportedRuntime(unsigned major,unsigned minor,unsigned patch,unsigned build) {
    return major==1 && build==0 && ((minor==5 && patch==97) || (minor==6 && patch==1170));
}
}
