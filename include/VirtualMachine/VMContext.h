#ifndef __VM_CONTEXT_H
#define __VM_CONTEXT_H 1

#include <inttypes.h>

struct _VMInstruction;
struct _VMContext;
struct _SaveRegs;

typedef void ( __stdcall *re_entry_no_state) (void);
typedef void ( __stdcall *re_entry_has_state)(uint32_t,struct _SaveRegs);
typedef void ( __stdcall *re_entry_to_instruction)(uint32_t,struct _SaveRegs, struct _VMInstruction *);
typedef struct _VMInstruction * ( __stdcall *lookup_instruction)(struct _VMContext *, uint32_t);


struct _SaveRegs
{
  uint32_t Regs[8];
};


struct _VMInstruction
{
  uint32_t key;
  uint8_t opcode;
  uint8_t datalen;
  uint8_t RVAFixup1;
  uint8_t RVAFixup2;
  uint8_t data[16];
};


struct _VMContext
{
  struct _VMInstruction * curr_insn; //ip Current instruction pointer
  uint32_t * x86stack; //esp saved vm stack data
  uint32_t * extra; //An extra register available to the VM instructions

  // This VM entrypoint takes no stack or register arguments
  re_entry_no_state fast_re_entry;

  // This VM entrypoint assumes that:
  // * The saved flags and registers are at the bottom of the stack
  // * EAX has a function pointer
  // It restores the flags/registers and jumps to the function pointer
  re_entry_has_state enter_function_pointer;

  // This VM entrypoint assumes that:
  // * The saved flags and registers are at the bottom of the stack
  // * Above that on the stack is a VMInstruction * (next insn to execute)
  // * EBX contains a pointer to the VMContext *
  // It executes by:
  // * Copying the VMInstruction * into VMContext->pCurrInsn
  // * Jumping to fpEnterFunctionPointer
  re_entry_to_instruction execute_VMInstruction;
  // This VM entrypoint assumes that:
  // * The saved flags and registers are at the bottom of the stack
  // * EBX contains a pointer to the VMContext *
  // * ECX points to the data area of a VMInstruction *
  // It executes by:
  // * If DWORD insnData[1] is non-zero, add it to VMContext->pCurrInsn
  // * Else, treat insnData[5] as an RVA and jump there
  re_entry_has_state relative_jmp;
  // This VM entrypoint assumes that:
// * The saved flags and registers are at the bottom of the stack
// * EBX contains a pointer to the VMContext *
// It executes by:
// * Adding sizeof(VMInstruction) to VMContext->pCurrInsn
// * Else, treat insnData[5] as an RVA and jump there
  re_entry_has_state next;


// VM State


// Pointer to decompressed instruction array.
  struct _VMInstruction * insn;

  void * PIC_base;
  uint32_t base_address;
  uint32_t saved_esp;
  uint32_t saved_esp2;

//Points to FindInstructionByKey function
  lookup_instruction lookup_insn;

// Copy of current instruction (not a pointer)
  struct _VMInstruction instruction;

// Pointer to beginning of data area, used as dynamically-generated code stack
  uint32_t dynmiac_code_stack;

// An array of data, used as both an X86 and a VM stack
  uint32_t data[0x3FEB];

};

#endif
