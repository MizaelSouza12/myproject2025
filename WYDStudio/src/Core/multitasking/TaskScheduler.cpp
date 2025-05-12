// TaskScheduler Component Implementation
#include "TaskScheduler.h"
#include <memory>
#include <vector>

namespace WYDStudio {
namespace Core {

class TaskScheduler {
public:
    TaskScheduler();
    ~TaskScheduler();

    bool Initialize();
    void Update();
    void Shutdown();

private:
    struct Implementation;
    std::unique_ptr<Implementation> impl_;
};

} // namespace Core
} // namespace WYDStudio
