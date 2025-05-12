// ProcessManager Component Implementation
#include "ProcessManager.h"
#include <memory>
#include <vector>

namespace WYDStudio {
namespace Core {

class ProcessManager {
public:
    ProcessManager();
    ~ProcessManager();

    bool Initialize();
    void Update();
    void Shutdown();

private:
    struct Implementation;
    std::unique_ptr<Implementation> impl_;
};

} // namespace Core
} // namespace WYDStudio
