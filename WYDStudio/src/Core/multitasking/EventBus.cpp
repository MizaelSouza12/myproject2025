// EventBus Component Implementation
#include "EventBus.h"
#include <memory>
#include <vector>

namespace WYDStudio {
namespace Core {

class EventBus {
public:
    EventBus();
    ~EventBus();

    bool Initialize();
    void Update();
    void Shutdown();

private:
    struct Implementation;
    std::unique_ptr<Implementation> impl_;
};

} // namespace Core
} // namespace WYDStudio
