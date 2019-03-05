#pragma once

#include <Defines.hpp>

namespace poly {
namespace api {
namespace resources {

    class IResourceLoadArgs : public Poly::BaseObject<>
    {
        virtual ~IResourceLoadArgs() override {}

        virtual bool IsPersistent() const noexcept = 0;

        virtual bool Compare(const IResourceLoadArgs& args) const noexcept = 0;
        virtual std::size_t GetHash() const noexcept = 0;
    };

}
}
}