//===-- RISCVInstrInfo.h - RISCV instruction information ----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the RISCV implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TARGET_RISCVINSTRINFO_H
#define LLVM_TARGET_RISCVINSTRINFO_H

#include "RISCV.h"
#include "RISCVRegisterInfo.h"
#include "llvm/Target/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "RISCVGenInstrInfo.inc"

namespace llvm {

class RISCVTargetMachine;

namespace RISCVII {
  enum {
    // See comments in RISCVInstrFormats.td.
    SimpleLoad  = (1 << 0),
    SimpleStore = (1 << 1)
  };
  // RISCV MachineOperand target flags.
  enum {
    // Masks out the bits for the access model.
    MO_SYMBOL_MODIFIER = (1 << 0),

    // @GOT (aka @GOTENT)
    MO_GOT = (1 << 0)
  };
}

class RISCVInstrInfo : public RISCVGenInstrInfo {
  const RISCVRegisterInfo RI;

  void splitMove(MachineBasicBlock::iterator MI, unsigned NewOpcode) const;
  void splitAdjDynAlloc(MachineBasicBlock::iterator MI) const;

public:
  explicit RISCVInstrInfo(RISCVTargetMachine &TM);

  // Override TargetInstrInfo.
  virtual unsigned isLoadFromStackSlot(const MachineInstr *MI,
                                       int &FrameIndex) const LLVM_OVERRIDE;
  virtual unsigned isStoreToStackSlot(const MachineInstr *MI,
                                      int &FrameIndex) const LLVM_OVERRIDE;
  virtual bool AnalyzeBranch(MachineBasicBlock &MBB,
                             MachineBasicBlock *&TBB,
                             MachineBasicBlock *&FBB,
                             SmallVectorImpl<MachineOperand> &Cond,
                             bool AllowModify) const LLVM_OVERRIDE;
  virtual unsigned RemoveBranch(MachineBasicBlock &MBB) const LLVM_OVERRIDE;
  virtual unsigned InsertBranch(MachineBasicBlock &MBB, MachineBasicBlock *TBB,
                                MachineBasicBlock *FBB,
                                const SmallVectorImpl<MachineOperand> &Cond,
                                DebugLoc DL) const LLVM_OVERRIDE;
  virtual void copyPhysReg(MachineBasicBlock &MBB,
                           MachineBasicBlock::iterator MBBI, DebugLoc DL,
                           unsigned DestReg, unsigned SrcReg,
                           bool KillSrc) const LLVM_OVERRIDE;
  virtual void
    storeRegToStackSlot(MachineBasicBlock &MBB,
                        MachineBasicBlock::iterator MBBI,
                        unsigned SrcReg, bool isKill, int FrameIndex,
                        const TargetRegisterClass *RC,
                        const TargetRegisterInfo *TRI) const LLVM_OVERRIDE;
  virtual void
    loadRegFromStackSlot(MachineBasicBlock &MBB,
                         MachineBasicBlock::iterator MBBI,
                         unsigned DestReg, int FrameIdx,
                         const TargetRegisterClass *RC,
                         const TargetRegisterInfo *TRI) const LLVM_OVERRIDE;
  virtual bool
    expandPostRAPseudo(MachineBasicBlock::iterator MBBI) const LLVM_OVERRIDE;
  virtual bool
    ReverseBranchCondition(SmallVectorImpl<MachineOperand> &Cond) const
    LLVM_OVERRIDE;

  // Return the RISCVRegisterInfo, which this class owns.
  const RISCVRegisterInfo &getRegisterInfo() const { return RI; }

  // Return true if MI is a conditional or unconditional branch.
  // When returning true, set Cond to the mask of condition-code
  // values on which the instruction will branch, and set Target
  // to the operand that contains the branch target.  This target
  // can be a register or a basic block.
  bool isBranch(const MachineInstr *MI, unsigned &Cond,
                const MachineOperand *&Target) const;

  // Get the load and store opcodes for a given register class.
  void getLoadStoreOpcodes(const TargetRegisterClass *RC,
                           unsigned &LoadOpcode, unsigned &StoreOpcode) const;

  // Opcode is the opcode of an instruction that has an address operand,
  // and the caller wants to perform that instruction's operation on an
  // address that has displacement Offset.  Return the opcode of a suitable
  // instruction (which might be Opcode itself) or 0 if no such instruction
  // exists.
  unsigned getOpcodeForOffset(unsigned Opcode, int64_t Offset) const;

  // Emit code before MBBI in MI to move immediate value Value into
  // physical register Reg.
  void loadImmediate(MachineBasicBlock &MBB,
                     MachineBasicBlock::iterator MBBI,
                     unsigned Reg, uint64_t Value) const;
};
} // end namespace llvm

#endif
