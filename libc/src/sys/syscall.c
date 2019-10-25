#include <sys/syscall.h>

int syscall(int s, uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5, uint32_t arg6, uint32_t arg7, uint32_t arg8, uint32_t arg9) {
  register long r0 asm ("r0") = s;
  register long r1 asm ("r1") = arg0;
  register long r2 asm ("r2") = arg1;
  register long r3 asm ("r3") = arg2;
  register long r4 asm ("r4") = arg3;
  register long r5 asm ("r5") = arg4;
  register long r6 asm ("r6") = arg5;
  register long r7 asm ("r7") = arg6;
  register long r8 asm ("r8") = arg7;
  register long r9 asm ("r9") = arg8;
  register long r10 asm ("r10") = arg9;
  asm volatile ("trapa #0" :: "r" (32));
  return r0;
}
