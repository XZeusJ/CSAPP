# Bomb Lab - Assembly, Stack Frame, GDB
## Machine-Level Representation of Programs

### preview
先看看符号表```objump -t bomb | less```  
再less下输入```/bomb```并回车, 检索到一系列文本关键字  
接着disassamble文件bomb ```objdump -d bomb > bomb.txt```  
bomb.txt里就有得来的汇编代码了  
### phase_1
检索bomb.txt, 可观察到第一阶段的代码  
```x86asm
0000000000400fb0 <phase_1>:
  400fb0:	48 83 ec 08          sub    $0x8,%rsp
  400fb4:	be f0 27 40 00       mov    $0x4027f0,%esi
  400fb9:	e8 72 04 00 00       callq  401430 <strings_not_equal>
  400fbe:	85 c0                test   %eax,%eax
  400fc0:	74 05                je     400fc7 <phase_1+0x17>
  400fc2:	e8 3d 07 00 00       callq  401704 <explode_bomb>
  400fc7:	48 83 c4 08          add    $0x8,%rsp
  400fcb:	c3                   retq
```

