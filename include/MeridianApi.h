#pragma once
#include "../extern/MeridianUI/ViewAPI.h"
namespace TextBridge {
inline ::Meridian::UI::View::IViewAPI* RequestMeridianView(::Meridian::UI::View::QueryMeridianExtensionFn query) {
    using namespace ::Meridian::UI::View;
    void* value{};
    return query && query(EXTENSION_NAME, INTERFACE_VERSION, &value, nullptr, "SkyrimTextBridge") && value
        ? static_cast<IViewAPI*>(value) : nullptr;
}
}
