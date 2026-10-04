#pragma once
#include <RE/B/BSInputEventQueue.h>
#include <span>
namespace TextBridge {
inline unsigned AvailableCharacters(const RE::BSInputEventQueue& queue) {
    return queue.charEventCount < queue.MAX_CHAR_EVENTS ? queue.MAX_CHAR_EVENTS - queue.charEventCount : 0;
}
// Temporarily isolate our appended chain without disturbing the engine's list.
inline RE::InputEvent* AppendGameCharacters(RE::BSInputEventQueue& queue, std::span<const std::uint32_t> characters) {
    auto& head = queue.GetQueueHead();
    auto& tail = queue.GetQueueTail();
    const auto savedHead = head, savedTail = tail;
    head = nullptr; tail = nullptr;
    for (auto character : characters) queue.AddCharEvent(character);
    const auto appended = head;
    head = savedHead; tail = savedTail;
    return appended;
}
}
