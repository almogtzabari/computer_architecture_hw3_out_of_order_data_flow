/* 046267 Computer Architecture - Winter 2019/20 - HW #3 */
/* Implementation (skeleton)  for the dataflow statistics calculator */

#include <vector>
#include <assert.h>
#include "dflow_calc.h"

/**
 * Instruction - data structure for each instruction
 */
class Instruction{
private:
    unsigned int id;
    unsigned int opcode;
    std::vector<int> dependencies;
    unsigned int depth;

public:
    /**
     * Constructor
     * Each dependency is initialized to -1, meaning no dependencies.
     */
    Instruction(): id(0), opcode(0), dependencies(2, -1), depth(0){};

    /**
     * Destructor
     */
    ~Instruction(){};

    /**
     * Add a dependency to instruction
     * @param instructionID - the dependency
     * @param dep_num - dependency num (either 1 or 2 - according to operand)
     */
    void addDependency(unsigned int instructionID, unsigned int dep_num){
        dependencies[dep_num - 1] = instructionID;
    }

    /**
     * Sets instruction's ID
     * @param id - id to set
     */
    void setID(unsigned int id){
        this->id = id;
    }

    /**
     * Set instruction's opcode
     * @param opcode - opcode to set
     */
    void setOpcode(unsigned int opcode){
        this->opcode = opcode;
    }

    /**
     * @return instruction's opcode
     */
    unsigned int getOpcode(){
        return this->opcode;
    }

    /**
     * Sets instruction's depth
     * @param depth - depth to set
     */
    void setDepth(unsigned int depth){
        this->depth = depth;
    }

    /**
     * @return instruction's depth
     */
    unsigned int getDepth(){
        return this->depth;
    }

    /**
     * @return a vector of instruction's dependencies
     */
    std::vector<int> getDependencies(){
        return this->dependencies;
    }

};

/**
 * Analayzer - data structure to analyze the data
 */
class Analayzer{
private:
    std::vector<int> lastInstructionModified; // Last instruction to modify a register
    std::vector<Instruction> instructions;
    unsigned int max_depth;

public:
    /**
     * Constructor - Create an instance of the Analayzer which analyze the data flow graph.
     * @param opsLatency - array of opcode's latencies
     * @param progTrace - array of instruction (trace of a program)
     * @param numOfInsts - number of instructions in the progTrace
     */
    Analayzer(const unsigned int opsLatency[], const InstInfo progTrace[], unsigned int numOfInsts):
    lastInstructionModified(32, -1), instructions(), max_depth(0){
        InstInfo instInfo;
        for (unsigned int i = 0; i < numOfInsts; i++){
            unsigned int depth1 = 0, depth2 = 0, depth = 0;
            // Read first instruction
            instInfo = progTrace[i];

            // Create our instruction data structure
            Instruction inst = Instruction();
            inst.setID(i);
            inst.setOpcode(instInfo.opcode);

            // First operand dependency
            int dep1 = lastInstructionModified[instInfo.src1Idx]; // Check for the last instruction that updated src1
            if(dep1 != -1){
                inst.addDependency(dep1, 1);
                depth1 = this->instructions[dep1].getDepth() + opsLatency[this->instructions[dep1].getOpcode()];
            }

            // Second operand dependency
            int dep2 = lastInstructionModified[instInfo.src2Idx]; // Check for the last instruction that updated src2
            if(dep2 != -1){
                inst.addDependency(dep2, 2);
                depth2 = this->instructions[dep2].getDepth() + opsLatency[this->instructions[dep2].getOpcode()];
            }

            // Check max depth and set our instruction's depth
            depth1 > depth2 ? depth = depth1 : depth = depth2;
            inst.setDepth(depth);

            // This instruction (with id=i) is the last instruction to update the destination register
            lastInstructionModified[instInfo.dstIdx] = i;

            // Update program's depth if needed.
            if(max_depth < depth + opsLatency[inst.getOpcode()]){
                max_depth = depth + opsLatency[inst.getOpcode()];
            }

            // Add analyzed instruction to data structure
            this->instructions.push_back(inst);
        }
    }

    /**
     * Destructor
     */
    ~Analayzer() = default;

    /**
     * @param instructionID - id of the instruction
     * @return instruction's depth
     */
    unsigned int getInstructionDepth(unsigned int instructionID){
        return instructions[instructionID].getDepth();
    }

    /**
     * Returns the instructions IDs that the given instruction is depending on
     * @param theInst - instructions ID
     * @param src1DepInst - address to write the first dependency
     * @param src2DepInst - address to write the second dependency
     * @return 0 if succeeded, -1 otherwise
     */
    int updateDependencies(unsigned int theInst, int *src1DepInst, int *src2DepInst){
        if(theInst >= this->instructions.size()){
            return -1;
        }
        std::vector<int> deps = this->instructions[theInst].getDependencies();
        *src1DepInst = deps[0];
        *src2DepInst = deps[1];
        return 0;
    }

    /**
     * @return program's depth.
     */
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
    return ((Analayzer*)ctx)->getInstructionDepth(theInst);
}

int getInstDeps(ProgCtx ctx, unsigned int theInst, int *src1DepInst, int *src2DepInst) {
    return ((Analayzer*)ctx)->updateDependencies(theInst, src1DepInst, src2DepInst);
}

int getProgDepth(ProgCtx ctx) {
    return ((Analayzer*)ctx)->getMaxDepth();
}


