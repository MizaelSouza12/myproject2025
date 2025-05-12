// ResourceManager Component Implementation
#include "ResourceManager.h"
#include <memory>
#include <vector>

namespace WYDStudio {
namespace Core {

class ResourceManager {
public:
    ResourceManager();
    ~ResourceManager();

    bool Initialize();
    void Update();
    void Shutdown();

private:
    struct Implementation;
    std::unique_ptr<Implementation> impl_;
};

} // namespace Core
} // namespace WYDStudio
