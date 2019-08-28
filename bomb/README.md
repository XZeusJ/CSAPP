# Bomb Lab - Assembly, Stack Frame, GDB
## Machine-Level Representation of Programs

### preview
先看看符号表```objump -t bomb | less```  
再less下输入```/bomb```并回车, 检索到一系列文本关键字  
接着disassamble文件bomb ```objdump -d bomb > bomb.txt```  
bomb.txt里就有得来的汇编代码了  
另外我们可以使用gdb来查看汇编指令， ```gdb bomb -tui```加上```layout asm```，可以在调试的时候查看对应的汇编代码了
### main function
对主函数中的关键部分进行解析
```
  400e32:   e8 67 06 00 00          callq  40149e <read_line>
  400e37:   48 89 c7                mov    %rax,%rdi    // Set x = result
  400e3a:   e8 a1 00 00 00          callq  400ee0 <phase_1> // Call phase_1(x)
  400e3f:   e8 80 07 00 00          callq  4015c4 <phase_defused>
```
代码首先调用了readline函数，从输入打参数中读取一行字符串。由约定可推知字符串首地址应该放在rax中。  
接着代码把rax中读入数据的地址复制到了rdi中，根据约定用rdi来作为函数phase_1的输入参数。  
接下来调用函数phase_1和phase_defused，后面的每一关模式大致如此。  

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
解析：  
首先rsp为当前函数帧栈分配8bytes，这是因为会使用到地址运算法&，所以caller必须分配一个栈帧  
将地址0x4027f0存储在esi中，由x86特性，此时寄存器高32位地址被全部置为0,因此该指令效果等同于```movq $0x4027f0,%rsi```，同时注意到此时rdi里已经存了我们输入的字符串参数首地址  
调用函数strings_not_equal，根据rdi和rsi里的地址，遍历两组字符串，如果完全相等，则函数返回0，否则返回1  
判断eax里的返回值是否为0,如果为0,则跳转至400fc7，释放栈帧，并返回函数;否则实验失败。  
  
解答：  
根据```x/s $esi```检查esi存储的字符串表示，得到"Border relations with Canada have never been better."即为第一题应该输入的参数答案。

### phase_2
```
0000000000400efc <phase_2>:
  400efc:   55                      push   %rbp
  400efd:   53                      push   %rbx
  400efe:   48 83 ec 28             sub    $0x28,%rsp
  400f02:   48 89 e6                mov    %rsp,%rsi
  400f05:   e8 52 05 00 00          callq  40145c <read_six_numbers>
  400f0a:   83 3c 24 01             cmpl   $0x1,(%rsp)
  400f0e:   74 20                   je     400f30 <phase_2+0x34>
  400f10:   e8 25 05 00 00          callq  40143a <explode_bomb>
  400f15:   eb 19                   jmp    400f30 <phase_2+0x34>
  400f17:   8b 43 fc                mov    -0x4(%rbx),%eax
  400f1a:   01 c0                   add    %eax,%eax
  400f1c:   39 03                   cmp    %eax,(%rbx)
  400f1e:   74 05                   je     400f25 <phase_2+0x29>
  400f20:   e8 15 05 00 00          callq  40143a <explode_bomb>
  400f25:   48 83 c3 04             add    $0x4,%rbx
  400f29:   48 39 eb                cmp    %rbp,%rbx
  400f2c:   75 e9                   jne    400f17 <phase_2+0x1b>
  400f2e:   eb 0c                   jmp    400f3c <phase_2+0x40>
  400f30:   48 8d 5c 24 04          lea    0x4(%rsp),%rbx
  400f35:   48 8d 6c 24 18          lea    0x18(%rsp),%rbp
  400f3a:   eb db                   jmp    400f17 <phase_2+0x1b>
  400f3c:   48 83 c4 28             add    $0x28,%rsp
  400f40:   5b                      pop    %rbx
  400f41:   5d                      pop    %rbp
  400f42:   c3                      retq   
```
解析：  
先把rbp，rbx寄存器保存在栈帧中，并分配一个28bytes的栈帧  
将栈指针rsp作为第二个参数，调用read_six_numbers函数，我们猜测该函数读取六个数字并且保留在phase_2函数的栈帧中。为了证实猜测，我们输入参数"1 2 4 8 16 32"，并且```break *400f0a```， 利用```x/32xb $rsp```查看rsp开始的32字节内容。

```
(gdb) x/32xb $rsp
0x7fffffffdc20: 0x01    0x00    0x00    0x00    0x02    0x00    0x00    0x00
0x7fffffffdc28: 0x04    0x00    0x00    0x00    0x08    0x00    0x00    0x00
0x7fffffffdc30: 0x10    0x00    0x00    0x00    0x20    0x00    0x00    0x00
0x7fffffffdc38: 0x31    0x14    0x40    0x00    0x00    0x00    0x00    0x00
```
可以看到rsp开始的24个字节存储了输入的6个数字  
接着如果rsp（&x1）中存储的是数字1, 就跳转到0x400f30继续执行，否则就爆炸。  
0x400f30的指令告诉我们查看&x2保存到rbx里， 把&x7（结束标识）放在rbp里  
接着，将x1复制到eax里，乘上2,将结果与x2进行比较，如果不等，爆炸;否则查看下一个地址&x3,将其与结束标识&x7进行比较，如果相等，则释放栈帧，返回函数，否则重复执行，至到6个数字全部遍历完成。  
答案：
根据上述规则，第一个数字应该为1,且6个数字组成的数列为等比数列，比例为2，"1 2 4 8 16 32"

### phase_3
