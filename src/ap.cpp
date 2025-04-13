#include "ap.hpp"

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
        
        std::unordered_map<int, std::string> reg_in_use;
        for (int i = 0; i < (int)pipeline.size(); ++i) {
            std::string instr = pipeline[i]->name();
            int stage = tick - i;
            if (stage < 0 || stage > 4)
                continue;
            
            auto required = pipeline[i]->getRegRequired(stage);
            for (auto& r : required) {
                if (reg_in_use.find(r) != reg_in_use.end()) {
                    state.err_msg = "Resource conflict at tick" + std::to_string(tick) + 
                    ": instruction " + instr + " at stage E" + std::to_string(stage + 1) +
                    " try to allocate reg R" + std::to_string(r + 1) + " which is already in use by " + reg_in_use[r] + "\n";
                    return false;
                }
                reg_in_use[r] = instr;
                /* Reg in use */
                //oss << "R" + std::to_string(r + 1) + " ";
            }
            pipeline[i]->execute(stage, oss, state);
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
    std::vector<std::vector<std::string>> testcases {{"ADD", "MUL"},
                                                     {"MUL", "ADD"}};

    std::cout << "Emulate Start:\n";

    for (auto& tc : testcases) {
        result = run(tc, state);
    
        if (result)
            bgcolor("Emulate success", result);
        else
            bgcolor("Emulate fail", result);
        
        std::cout << (result ? state.log : state.err_msg + state.log) << std::endl;
    }
    
    std::cout << "Emulate End\n";
    return 0;
}

