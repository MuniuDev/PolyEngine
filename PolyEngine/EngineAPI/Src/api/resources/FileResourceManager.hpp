#pragma once

#include <Defines.hpp>

#include <Resources/IResourceManager.hpp>

namespace poly {
namespace api {
namespace resources {

    template <typename Resource>
    class FileResourceManager : public IResourceManager
    {
    private:
        struct ResourcePtr
        {
            std::weak_ptr<IResource> WeakPtr;
            std::shared_ptr<IResource> SharedPtr;

            std::shared_ptr<IResource> GetLocked()
            {
                if(SharedPtr)
                    return SharedPtr;
                return WeakPtr.lock();
            }
        };

        class ArgsWrapper
        {
        public:
            ArgsWrapper(std::unique_ptr<IResourceLoadArgs>&& args)
             : Args(std::move(args))
            {
            }

            const IResourceLoadArgs& GetArgs() { return *(Args.get()); }
        private:
            std::unique_ptr<const IResourceLoadArgs> Args;
        };

        struct ArgsWrapperHash
        {
            std::size_t operator()(const ArgsWrapper& args) const noexcept { return args.Args.GetHash(); }
        };
    public:
        ~FileResourceManager() override {}

        std::shared_ptr<IResource> Load(std::unique_ptr<IResourceLoadArgs>&& args) override
        {
            ArgsWrapper argsWrap(std::move(args));
            std::shared_ptr<IResource> res;
            auto it = ResourceMap.find(argsWrap);
            
            if (it != ResourceMap.end())
            {

            }

            if (it == ResourceMap.end())
            {
                std::shared_ptr<IResource> res = it->second.GetLocked();
                if(!res)
                {
                    res = CreateResource(argsWrap.GetArgs());
                }
            }
            else
            {
                res = ;
                if(!res)
                {

                }
            }
            
        }
        
        size_t GetLoadedCount() const override
        {
            return ResourceMap.size();
        }
    protected:
        virtual std::shared_ptr<IResource> CreateResource(IResourceLoadArgs& args) = 0;
    private:
        std::unordered_map<ResourceLoadArgsWrapper, ResourcePtr> ResourceMap;
    };

}
}
}