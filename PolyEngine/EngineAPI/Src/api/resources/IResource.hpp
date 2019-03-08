#pragma once

#include <Defines.hpp>

namespace poly {
namespace api {
namespace resources {

    class IResource : public Poly::BaseObject<>
    {
    public:
        virtual ~IResource() override {}
    };  

}
}
}