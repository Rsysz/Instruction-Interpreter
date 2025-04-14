#include <string>
#include <unordered_map>

#include "windows.h"

#define UNUSED(x) (void) (x)

enum RESULT {
    FAIL = 0,
    SUCCESS,
    WARN,
};

enum REG {
    R1 = 0,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7,
    R9,
    R10,
};

enum STAGE {
    E1 = 0,
    E2,
    E3,
    E4,
    E5,
    STAGE,
};

std::unordered_map<std::string, std::unordered_map<int, std::vector<int>>> resource_table {
    {"ADD", {
        {E1, {R1}},
        {E2, {R2}},
        {E4, {R5}},
        {E5, {R5}},
    }},
    {"MUL", {
        {E1, {R1, R2}},
        {E3, {R3}},
        {E4, {R3}},
    }},
    {"LD", {
        {E1, {R1}},
        {E2, {R3, R5}},
        {E4, {R3}},
        {E5, {R2}},
    }},
    {"ST", {
        {E1, {R1, R4}},
        {E2, {R4}},
        {E3, {R5}},
        {E5, {R3}},
    }},
    {"INVALID", {
        {STAGE, {}},
    }},
};

struct MachineState {
    std::string log;
    std::string err_msg;
    std::string warn_msg;
};

class Instruction {
    public:
        virtual ~Instruction() = default;
        virtual std::string name() const = 0;
        virtual std::vector<int>& getRegRequired(int stage) const {
            std::string instr = name();
            if (resource_table.find(instr) == resource_table.end())
                return resource_table["INVALID"][STAGE]; // not found
            
            if (resource_table[instr].find(stage) == resource_table[instr].end())
                return resource_table["INVALID"][STAGE]; // no resource required
            
            return resource_table[instr][stage];
        }
    
        virtual void execute(int stage, std::ostringstream& oss, MachineState& state) const {
            // TODO: consider stage invalid
            UNUSED(state);
            oss << name() + "_E" + std::to_string(stage + 1) + ";\n";
        }

        int issued_tick = -1;
};
    
class ADD : public Instruction {
    public:
        std::string name() const override { return "ADD"; }
};

class MUL : public Instruction {
    public:
        std::string name() const override { return "MUL"; }
};

class LD : public Instruction {
    public:
        std::string name() const override { return "LD"; }
};

class ST : public Instruction {
    public:
        std::string name() const override { return "ST"; }
};

class LDI : public Instruction {
    public:
        std::string name() const override { return "LDI"; }
};

class OR : public Instruction {
    public:
        std::string name() const override { return "OR"; }
};

class AND : public Instruction {
    public:
        std::string name() const override { return "AND"; }
};

class LT : public Instruction {
    public:
        std::string name() const override { return "LT"; }
};

class EQ : public Instruction {
    public:
        std::string name() const override { return "EQ"; }
};

class NOP : public Instruction {
    public:
        std::string name() const override { return "NOP"; }
};

std::shared_ptr<Instruction> createInstruction(const std::string& name) {
    if (name == "ADD") return std::make_shared<ADD>();
    if (name == "MUL") return std::make_shared<MUL>();
    if (name == "LD") return std::make_shared<LD>();
    if (name == "ST") return std::make_shared<ST>();
    return nullptr;
}
