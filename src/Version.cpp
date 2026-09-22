#include <SKSE/SKSE.h>
#ifdef TEXTBRIDGE_MANUAL_BUILD
namespace {
// This CommonLib fork defaults VersionNumber to 1.0.0. Explicit zero padding
// is required for the SKSE compatible-version list's zero terminators.
constexpr auto compatibility = []<std::size_t... I>(std::index_sequence<I...>) {
    using Number = SKSE::PluginDeclaration::VersionNumber;
    return SKSE::PluginDeclaration::RuntimeCompatibility{
        Number{1,5,97,0}, Number{1,6,1170,0}, (static_cast<void>(I), Number{0})...};
}(std::make_index_sequence<14>{});
}
SKSEPluginInfo(
    .Version = {0, 3, 5, 0},
    .Name = "SkyrimTextBridge",
    .Author = "BlackMesa79",
    .RuntimeCompatibility = compatibility
);
#endif
