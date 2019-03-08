#pragma once

#include <Defines.hpp>

#include <api/resources/IResource.hpp>
#include <api/resources/IResourceSource.hpp>
#include <api/resources/IResourceLoadArgs.hpp>

namespace poly {
namespace api {
namespace resources {

    class ResourceLoader : public Poly::BaseObject<>
    {
    public:
        ResourceLoader(std::unique_ptr<IResourceSource>&& source);
        std::future<std::shared_ptr<IResource>> Load(std::unique_ptr<const IResourceLoadArgs>&& args);

    private:
        class ArgsWrapper
        {
        public:
            ArgsWrapper(std::unique_ptr<IResourceLoadArgs>&& args)
             : Args(std::move(args)) {}
            
            inline const IResourceLoadArgs& GetArgs() const { return *(Args.get()); } 
            inline bool operator=(const ArgsWrapper& rhs) const { return Args->IsSameResource(rhs.GetArgs()); }

            struct Hasher
            {
                inline std::size_t operator()(const ArgsWrapper& args) const noexcept { return args.GetArgs().GetHash(); }
            };
        private:
            std::unique_ptr<const IResourceLoadArgs> Args;
        };

        // Not copyable, not movable
        ResourceLoader(const ResourceLoader&) = delete;
        ResourceLoader(ResourceLoader&&) = delete;
        ResourceLoader& operator=(const ResourceLoader&) = delete;
        ResourceLoader& operator=(ResourceLoader&&) = delete;

        std::unique_ptr<IResourceSource> Source;
        std::unordered_map<ArgsWrapper, std::weak_ptr<IResource>, ArgsWrapper::Hasher> ResourceMap;
    };

}
}
}