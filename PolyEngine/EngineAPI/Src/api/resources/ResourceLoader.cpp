#include <api/resources/ResourceLoader.hpp>

using namespace poly::api::resources;

ResourceLoader::ResourceLoader(std::unique_ptr<IResourceSource>&& source)
{
	// @todo implement
}

std::future<std::shared_ptr<IResource>> ResourceLoader::Load(std::unique_ptr<const IResourceLoadArgs>&& args)
{
	// @todo implement
	std::promise<std::shared_ptr<IResource>> promise;
	std::future<std::shared_ptr<IResource>> future = promise.get_future();
	promise.set_value(nullptr);
    return future;
}