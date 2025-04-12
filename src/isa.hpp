#include <string>
#include <unordered_map>

#include "windows.h"

enum RESULT {
    FAIL = 0,
    SUCCESS,
    WARN,
};

struct MachineState {
    std::string log;
    std::string err_msg;
};

const std::unordered_map<std::string, int> instruction_table {
    {"ADD", 5},
    {"MUL", 5},
};

class Instruction {
    public:
        virtual ~Instruction() = default;
        virtual std::string name() const = 0;
    
        virtual void execute(int stage, std::ostringstream& oss) const {
            // TODO: consider stage invalid
            oss << name() + "_E" + std::to_string(stage) + ";\n";
        }
};
    
class ADD : public Instruction {
    public:
        std::string name() const override { return "ADD"; }
};

class MUL : public Instruction {
    public:
        std::string name() const override { return "MUL"; }
};
