#include "ap.hpp"

std::shared_ptr<Instruction> createInstruction(const std::string& name) {
    if (name == "ADD") return std::make_shared<ADD>();
    if (name == "MUL") return std::make_shared<MUL>();
    return nullptr;
}

bool run(const std::vector<std::string>& program, MachineState& state) {
    std::vector<std::shared_ptr<Instruction>> pipeline;
    for (auto& instr_name : program) {
        auto instr = createInstruction(instr_name);
        if (!instr) {
            state.err_msg = "Unknown Instruction: " + instr_name;
            return false;
        }
        pipeline.push_back(instr);
    }

    int total_tick = pipeline.size() + 4;
    for (int tick = 0; tick < total_tick; tick++) {
        std::ostringstream oss;
        oss << "===== Start of tick "<< tick <<". =====\n";
        
        for (int i = 0; i < (int)pipeline.size(); ++i) {
            int stage = tick - i + 1;
            if (stage >= 1 && stage <= 5)
                pipeline[i]->execute(stage, oss);
        }
        
        oss << "===== End of tick " << tick << ". =====\n";
        state.log += oss.str();
    }
    
    return true;
}

void bgcolor(std::string s, int result) {
    if (result == FAIL)
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_RED);
    else if (result == SUCCESS)
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_GREEN);
    else
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_GREEN | BACKGROUND_RED);
    std::cout << s;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    std::cout << "\n";
}

int main() {
    MachineState state;
    bool result = false;

    std::cout << "Emulate Start:\n";
    result = run({"ADD", "MUL"}, state);
    
    if (result)
        bgcolor("Emulate success", result);
    else
        bgcolor("Emulate fail", result);

    std::cout << (result ? state.log : state.err_msg + state.log);
    
    std::cout << "Emulate End\n";
    return 0;
}

