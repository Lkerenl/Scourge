from pwn import *

filename = "text_tester"

code = "\xEB\x01\x53\x63\x6f\x75\x72\x67\x65\x20\x42\x65\x67\x69\x6e\x00VY560601A0WXSZ\xEB\x0F\x53\x63\x6f\x75\x72\x67\x65\x20\x45\x6e\x64\x00VY560601A0WXSZ"

# hexdump(code)
print(code)
with open(filename, "wb") as f:
    f.write(code * 2)
print disasm(code)
