nasm -f win64 asmconvert.asm
gcc -c cconvert.c -o cconvert.obj -m64
gcc cconvert.obj asmconvert.obj -o cconvert.exe -m64
cconvert.exe
pause