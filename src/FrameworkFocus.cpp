#include "FrameworkFocus.h"
#include "PrismaBridge.h"
#include "MeridianBridge.h"
#include "ScaleformFocus.h"

namespace TextBridge::FrameworkFocus {
void Poll(bool enabled) { Meridian::PollAutoFocus(enabled && !Prisma::OwnsInput() && !ScaleformFocus::ConsoleOpen()); }
Target Current() {
    // Ownership precedes field detection. A focused framework without a
    // supported editable field must not fall through to the UI underneath.
    if (Prisma::OwnsInput()) return {Backend::Native, {}};
    if (ScaleformFocus::ConsoleOpen()) return {};
    if (Meridian::HasFocus()) return {Backend::Meridian, Meridian::AutoTarget()};
    auto field = ScaleformFocus::Target();
    return field.empty() ? Target{} : Target{Backend::Scaleform, std::move(field)};
}
bool Begin(const Target& target) {
    if (target.identity.empty() || Prisma::OwnsInput() || ScaleformFocus::ConsoleOpen()) return false;
    if (target.backend == Backend::Meridian) return Meridian::Begin(target.identity);
    // Re-check focus before activation. The identity contains the actual movie
    // instance, so an old field cannot be reused after the menu is recreated.
    return target.backend == Backend::Scaleform && !Meridian::HasFocus() &&
        ScaleformFocus::Target() == target.identity;
}
}
