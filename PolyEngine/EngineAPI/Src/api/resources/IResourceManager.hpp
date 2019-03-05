#pragma once

#include <Defines.hpp>

#include <Resources/IResource.hpp>
#include <Resources/IResourceLoadArgs.hpp>

namespace poly {
namespace api {
namespace resources {

    class IResourceManager : public Poly::BaseObject<>
    {
    public:
        virtual ~IResourceManager() override {}

        virtual std::shared_ptr<IResource> Load(std::unique_ptr<IResourceLoadArgs>&& args) = 0;
        
        virtual size_t GetLoadedCount() const noexcept = 0;

    private:
        // Not copyable, not movable
        IResourceManager(const IResourceManager&) = delete;
        IResourceManager(IResourceManager&&) = delete;
        IResourceManager& operator=(const IResourceManager&) = delete;
        IResourceManager& operator=(IResourceManager&&) = delete;
    };

}
}
}