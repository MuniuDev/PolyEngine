#pragma once

#include <Defines.hpp>
#include <UniqueID.hpp>

#include <api/resources/IResource.hpp>
#include <api/resources/IResourceLoadArgs.hpp>

namespace poly {
namespace api {
namespace resources {

    class IResourceSource : public Poly::BaseObject<>
    {
    public:
        virtual ~IResourceSource() override {}

        virtual std::shared_ptr<IResource> Create(const IResourceLoadArgs&) = 0;
        virtual bool IsResourceAvailable(const Poly::UniqueID& id) = 0;
    };  

}
}
}