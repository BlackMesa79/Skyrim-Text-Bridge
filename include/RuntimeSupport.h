#pragma once
namespace TextBridge {
constexpr bool SupportedRuntime(unsigned major,unsigned minor,unsigned patch,unsigned build) {
    // Experimental branch: admit only explicitly targeted 1.7 runtimes.
    return major==1 && build==0 && ((minor==5 && patch==97) || (minor==6 && patch==1170) || (minor==7 && (patch==99 || patch==104)));
}
}
