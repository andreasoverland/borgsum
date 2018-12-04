#include <stdio.h>

/*__declspec(naked)*/ void
doStuff(unsigned long int val, unsigned long int flags, unsigned char *result)
{
  __asm
    {
        push rax
        push rbx
        push rcx

        mov eax, dword ptr[ebp + 8]  //val
        mov ebx, dword ptr[ebp + 12] //flags
        mov ecx, dword ptr[ebp + 16] //result

        and eax, ebx
        mov [ecx], eax

        pop rcx
        pop rbx
        pop rax
      }
}

int main(int argc, char *argv[])
{
  unsigned long val =   0xAA00A1F2;
  unsigned long flags = 0x00100002;
  unsigned char result = 0x0;

  doStuff(val, flags, &result);
  printf("Result is: %2Xh\n", result);

  return 0;
}
