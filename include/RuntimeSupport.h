#pragma once
namespace TextBridge {
constexpr bool SupportedRuntime(unsigned major,unsigned minor,unsigned patch,unsigned build) {
    // Experimental branch: admit only the explicitly targeted 1.7.104 runtime.
    return major==1 && build==0 && ((minor==5 && patch==97) || (minor==6 && patch==1170) || (minor==7 && patch==104));
}
}
