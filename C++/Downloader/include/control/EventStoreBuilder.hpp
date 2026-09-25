#pragma once

#include "control/EventStore.hpp"
#include "model/DatalakeLayout.hpp"

#include <filesystem>
#include <memory>

namespace downloader::control {

class EventStoreBuilder {
public:
    static std::unique_ptr<EventStore> build(
        downloader::model::DatalakeLayout layout,
        const std::filesystem::path& baseDirectory = "."
    );
};

}
