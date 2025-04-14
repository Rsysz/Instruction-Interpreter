#include "ap.hpp"

bool printInstructionOnTick(std::vector<std::shared_ptr<Instruction>>& pipeline, int tick, std::ostringstream& oss, MachineState& state) {
    bool done = true;
    for (int i = 0; i < (int)pipeline.size(); ++i) {
        auto instr = pipeline[i]->name();
        auto& issued = pipeline[i]->issued_tick;
        if (issued == -1)
            break;
        
        int stage = tick - issued;
        if (stage < 0 || stage > 4)
            continue;

        done = false;
        pipeline[i]->execute(stage, oss, state);
    }
    return done;
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

    int tick = 0, curr = 0;
    bool done = false;

    while (!done) {
        std::ostringstream oss;
        oss << "===== Start of tick "<< tick <<". =====\n";
        
        if (curr < (int)pipeline.size()) {
            auto instr = pipeline[curr]->name();
            auto& issued = pipeline[curr]->issued_tick;
            /*
            tick
            0   1   2   3   4   5
            ADD ADD ADD ADD ADD ADD
            */
            bool canIssued = true;
            for (int stage = 0; stage < STAGE; stage++) {

                for (int prev = std::max(curr - STAGE + 1, 0); prev < curr; prev++) {
                    int prev_stage = tick - pipeline[prev]->issued_tick;
                    if (prev_stage < 0 || prev_stage > 4)
                        continue;

                    auto& used = pipeline[prev]->getRegRequired(prev_stage);
                    auto& require = pipeline[curr]->getRegRequired(stage);

                    //std::cout << pipeline[curr]->name() << " " << stage_tick << " " << (int)require.size() << std::endl;
                    for (auto& r : require) {
                        if (std::find(used.begin(), used.end(), r) != used.end()) {
                            state.warn_msg = "Reschedule happen at tick" + std::to_string(tick) + 
                            ": instruction " + instr + " at stage E" + std::to_string(stage + 1) +
                            " try to allocate reg R" + std::to_string(r + 1) + " which is already in use\n\n";
                            canIssued = false;
                        }
                    }
                }
            }
            
            if (canIssued) {
                issued = tick;
                curr++;
            }
        }
        
        done = printInstructionOnTick(pipeline, tick, oss, state);
        oss << "===== End of tick " << tick << ". =====\n";
        state.log += oss.str();
        tick++;
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
    std::vector<std::vector<std::string>> testcases {{"ADD", "MUL"},
                                                     {"MUL", "ADD"},
                                                     {"ADD", "ADD", "ADD"}};

    std::cout << "Emulate Start:\n";

    for (auto& tc : testcases) {
        MachineState state;
        bool result = false;
        result = run(tc, state);
    
        if (result)
            bgcolor("Emulate success", result);
        else
            bgcolor("Emulate fail", result);
        
        std::cout << (result ? state.warn_msg + state.log : state.warn_msg + state.log) << std::endl;
    }
    
    std::cout << "Emulate End\n";
    return 0;
}

