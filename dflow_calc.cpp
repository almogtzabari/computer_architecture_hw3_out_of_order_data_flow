/* 046267 Computer Architecture - Winter 2019/20 - HW #3 */
/* Implementation (skeleton)  for the dataflow statistics calculator */

#include <vector>
#include <assert.h>
#include "dflow_calc.h"

class Instruction{
private:
    unsigned int id;
    unsigned int opcode;
    std::vector<int> dependencies;
    unsigned int depth;

public:
    Instruction(): id(0), opcode(0), dependencies(2, -1), depth(0){};
    ~Instruction(){};
    void addDependency(unsigned int instructionID, unsigned int dep_num){
        dependencies[dep_num - 1] = instructionID;
    }
    void setID(unsigned int id){
        this->id = id;
    }

    void setOpcode(unsigned int opcode){
        this->opcode = opcode;
    }

    unsigned int getOpcode(){
        return this->opcode;
    }

    void setDepth(unsigned int depth){
        this->depth = depth;
    }

    unsigned int getDepth(){
        return this->depth;
    }

    std::vector<int> getDependencies(){
        return this->dependencies;
    }

};

class Analayzer{
private:
    std::vector<int> lastInstructionModified; // Last instruction to modify a register
    std::vector<Instruction> instructions;
    unsigned int max_depth;

public:
    Analayzer(const unsigned int opsLatency[], const InstInfo progTrace[], unsigned int numOfInsts):
    lastInstructionModified(32, -1), instructions(), max_depth(0){
        InstInfo instInfo;
        for (unsigned int i = 0; i < numOfInsts; i++){
            unsigned int depth1 = 0, depth2 = 0, depth = 0;
            Instruction inst = Instruction();
            inst.setID(i);
            instInfo = progTrace[i];
            inst.setOpcode(instInfo.opcode);

            int dep1 = lastInstructionModified[instInfo.src1Idx];
            if(dep1 != -1){
                inst.addDependency(dep1, 1);
                depth1 = this->instructions[dep1].getDepth() + opsLatency[this->instructions[dep1].getOpcode()];
            }
            int dep2 = lastInstructionModified[instInfo.src2Idx];
            if(dep2 != -1){
                inst.addDependency(dep2, 2);
                depth2 = this->instructions[dep2].getDepth() + opsLatency[this->instructions[dep2].getOpcode()];
            }
            depth1 > depth2 ? depth = depth1 : depth = depth2;
            inst.setDepth(depth);

            lastInstructionModified[instInfo.dstIdx] = i;

            if(max_depth < depth + opsLatency[inst.getOpcode()]){
                max_depth = depth + opsLatency[inst.getOpcode()];
            }
            this->instructions.push_back(inst);
        }
    }
    ~Analayzer() = default;

    unsigned int getInstructionDepth(unsigned int instructionID){
        return instructions[instructionID].getDepth();
    }

    int updateDependencies(unsigned int theInst, int *src1DepInst, int *src2DepInst){
        if(theInst >= this->instructions.size()){
            return -1;
        }
        std::vector<int> deps = this->instructions[theInst].getDependencies();
        *src1DepInst = deps[0];
        *src2DepInst = deps[1];
        return 0;
    }

    int getMaxDepth(){
        return this->max_depth;
    }

};

ProgCtx analyzeProg(const unsigned int opsLatency[], const InstInfo progTrace[], unsigned int numOfInsts) {
    return new Analayzer(opsLatency, progTrace, numOfInsts);
}

void freeProgCtx(ProgCtx ctx) {
    delete (Analayzer*)ctx;
}

int getInstDepth(ProgCtx ctx, unsigned int theInst) {
    assert(theInst >= 0);
    return ((Analayzer*)ctx)->getInstructionDepth(theInst);
}

int getInstDeps(ProgCtx ctx, unsigned int theInst, int *src1DepInst, int *src2DepInst) {
    return ((Analayzer*)ctx)->updateDependencies(theInst, src1DepInst, src2DepInst);
}

int getProgDepth(ProgCtx ctx) {
    return ((Analayzer*)ctx)->getMaxDepth();
}


