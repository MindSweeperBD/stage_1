#pragma once

#include "model/BookEvent.hpp"

namespace downloader::control {

class EventStore {
public:
    virtual ~EventStore() = default;

    virtual void store(
        const downloader::model::BookEvent& event
    ) = 0;
};

}
