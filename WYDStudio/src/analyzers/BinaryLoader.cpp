// BinaryLoader Implementation
#include "BinaryLoader.h"
#include <vector>
#include <string>

namespace WYDStudio {
namespace Analyzers {

class BinaryLoader {
public:
    BinaryLoader();
    ~BinaryLoader();

    bool Analyze(const std::vector<uint8_t>& data);
    void GenerateReport();

private:
    struct Implementation;
    std::unique_ptr<Implementation> impl_;
};

} // namespace Analyzers
} // namespace WYDStudio
