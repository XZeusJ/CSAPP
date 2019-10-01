# Bomb Lab - Assembly, Stack Frame, GDB
## Machine-Level Representation of Programs

### x86-64位汇编-passing data函数传参小结  
1. 不超过6个的整型参数(包括指针)通过寄存器传递
2. 寄存器顺序, rdi rsi rdx rcx r8 r9
3. 超过部分通过调用栈传递(属于调用者的栈部分)
4. 简单的返回值通过rax传递

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
图示：  
'rdi rsi rdx rcx r8 r9'  
arg1(rdi) -> input  
arg2(rsi) -> %rsp/&x   

4 bits per cell  
[]  
[]  
[]  
[]  
[&arr[5]]  <- 0x14(%rsi) <- rax
[&arr[4]]  <- 0x10(%rsi) <- rax
[&arr[3]]  <- 0xc(%rsi) <- r9
[&arr[2]]  <- 0x8(%rsi) <- r8
[&arr[1]]  <- 0x4(%rsi) <- rcx
[&arr[0]]  <- (%rsp)  <- rsi <- rdx

8 bits per cell
[]  
[ &arr[5] ]  
[ &arr[4] ]  <- (%rsp) 
此时 rdx rcx r8 r9 (%rsp) 8(%rsp) 存放着数组里6个数字的地址，rdi是输入的字符串，rsi的值是0x4025c3,该地址的字符串是"%d %d %d %d %d %d"

0x18(%rsp) -> %rbp
0x14(%rsi) -> [] -> 0x8(%rsp)  
0x10(%rsi) -> [] -> (%rsp)  
0xc(%rsi)  -> [] -> %r9  
0x8(%rsi)  -> [&arr[2]] -> %r8  
0x4(%rsi)  -> [&arr[1]] -> %rcx  
%rsi       -> [&arr[0]] -> %rdx < %rsp    

rbx -> arr[1]  
eax -> arr[0] -> 2  
...  

解析：
先把rbp，rbx寄存器保存在栈帧中，并分配一个28bytes的栈帧  
调用read_six_numbers函数,我们猜测该函数读取六个数字并且保留在phase_2函数的栈帧中。为了证实猜测，我们输入参数"1 2 4 8 16 32"，并且```break *400f0a```， 利用```x/32xb $rsp```查看rsp开始的32字节内容。

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
```
0000000000400f43 <phase_3>:
  400f43:   48 83 ec 18             sub    $0x18,%rsp
  400f47:   48 8d 4c 24 0c          lea    0xc(%rsp),%rcx
  400f4c:   48 8d 54 24 08          lea    0x8(%rsp),%rdx
  400f51:   be cf 25 40 00          mov    $0x4025cf,%esi
  400f56:   b8 00 00 00 00          mov    $0x0,%eax
  400f5b:   e8 90 fc ff ff          callq  400bf0 <__isoc99_sscanf@plt>
  400f60:   83 f8 01                cmp    $0x1,%eax
  400f63:   7f 05                   jg     400f6a <phase_3+0x27>
  400f65:   e8 d0 04 00 00          callq  40143a <explode_bomb>
  400f6a:   83 7c 24 08 07          cmpl   $0x7,0x8(%rsp)
  400f6f:   77 3c                   ja     400fad <phase_3+0x6a>
  400f71:   8b 44 24 08             mov    0x8(%rsp),%eax
  400f75:   ff 24 c5 70 24 40 00    jmpq   *0x402470(,%rax,8)
  400f7c:   b8 cf 00 00 00          mov    $0xcf,%eax
  400f81:   eb 3b                   jmp    400fbe <phase_3+0x7b>
  400f83:   b8 c3 02 00 00          mov    $0x2c3,%eax
  400f88:   eb 34                   jmp    400fbe <phase_3+0x7b>
  400f8a:   b8 00 01 00 00          mov    $0x100,%eax
  400f8f:   eb 2d                   jmp    400fbe <phase_3+0x7b>
  400f91:   b8 85 01 00 00          mov    $0x185,%eax
  400f96:   eb 26                   jmp    400fbe <phase_3+0x7b>
  400f98:   b8 ce 00 00 00          mov    $0xce,%eax
  400f9d:   eb 1f                   jmp    400fbe <phase_3+0x7b>
  400f9f:   b8 aa 02 00 00          mov    $0x2aa,%eax
  400fa4:   eb 18                   jmp    400fbe <phase_3+0x7b>
  400fa6:   b8 47 01 00 00          mov    $0x147,%eax
  400fab:   eb 11                   jmp    400fbe <phase_3+0x7b>
  400fad:   e8 88 04 00 00          callq  40143a <explode_bomb>
  400fb2:   b8 00 00 00 00          mov    $0x0,%eax
  400fb7:   eb 05                   jmp    400fbe <phase_3+0x7b>
  400fb9:   b8 37 01 00 00          mov    $0x137,%eax
  400fbe:   3b 44 24 0c             cmp    0xc(%rsp),%eax
  400fc2:   74 05                   je     400fc9 <phase_3+0x86>
  400fc4:   e8 71 04 00 00          callq  40143a <explode_bomb>
  400fc9:   48 83 c4 18             add    $0x18,%rsp
  400fcd:   c3                      retq   
```

解析：  


```
'rdi rsi rdx rcx r8 r9'  

int sscanf(const char *restrict s, const char *restrict format, ...);

rdi -> phase_3#input -> s
rsi -> $0x4025cf -> "%d %d" -> format
rdx -> 8(%rsp) -> a
rcx -> c(%rsp) -> b

unsigned long a;
long b;
eax = sscanf(input, "%d %d", &a, &b);
if (eax > 1) {
    if (a > 7) {
        explode_bomb();
    } else {
        eax = a;
        dst_addr = *(0x402470 + 8*eax)
    }
} else {
        explode_bomb();
}

a = 0 -> 400f7c  
a = 1 -> 400fb9  
a = 2 -> 400f83  
a = 3 -> 400f8a  
a = 4 -> 400f91  
a = 5 -> 400f98  
a = 6 -> 400f9f  
a = 7 -> 400fa6  
```

答案：  
0 207  

### phase_4:  
```
000000000040100c <phase_4>:
  40100c:   48 83 ec 18             sub    $0x18,%rsp
  401010:   48 8d 4c 24 0c          lea    0xc(%rsp),%rcx
  401015:   48 8d 54 24 08          lea    0x8(%rsp),%rdx
  40101a:   be cf 25 40 00          mov    $0x4025cf,%esi
  40101f:   b8 00 00 00 00          mov    $0x0,%eax
  401024:   e8 c7 fb ff ff          callq  400bf0 <__isoc99_sscanf@plt>
  401029:   83 f8 02                cmp    $0x2,%eax
  40102c:   75 07                   jne    401035 <phase_4+0x29>
  40102e:   83 7c 24 08 0e          cmpl   $0xe,0x8(%rsp)
  401033:   76 05                   jbe    40103a <phase_4+0x2e>
  401035:   e8 00 04 00 00          callq  40143a <explode_bomb>
  40103a:   ba 0e 00 00 00          mov    $0xe,%edx
  40103f:   be 00 00 00 00          mov    $0x0,%esi
  401044:   8b 7c 24 08             mov    0x8(%rsp),%edi
  401048:   e8 81 ff ff ff          callq  400fce <func4>
  40104d:   85 c0                   test   %eax,%eax
  40104f:   75 07                   jne    401058 <phase_4+0x4c>
  401051:   83 7c 24 0c 00          cmpl   $0x0,0xc(%rsp)
  401056:   74 05                   je     40105d <phase_4+0x51>
  401058:   e8 dd 03 00 00          callq  40143a <explode_bomb>
  40105d:   48 83 c4 18             add    $0x18,%rsp
  401061:   c3                      retq   
```
解析:  
```
'rdi rsi rdx rcx r8 r9'  

int sscanf(const char *restrict s, const char *restrict format, ...);

*step 1*
rdi -> phase_3#input -> s
rsi -> $0x4025cf -> "%d %d" -> format
rdx -> 8(%rsp) -> a
rcx -> c(%rsp) -> b

*step 2*
rcx -> &b
rdx -> 15
rsi -> 0
rdi -> &a

unsigned long a;
long b;
eax = sscanf(input, "%d %d", &a, &b); 
if (eax != 2) { // *case 1*
    explode_bomb;
}
if (a <= 15) {
    eax = func4(a, 0, 15, b);  // *case 2*
    if (eax != 0) {
      explode_bomb;
    } else {
      if (b != 0) {
        explode_bomb;
      }
    }
} else {
    explode_bomb;
}

b = 0, a = ?

void func4(int x, int si, int dx) {
  // x, si=0, dx = 0
  int eax = ((dx-si)+(dx-si)>>31) >> 1;
  int cx = (((dx-si)+(dx-si)>>31) >> 1) + si;

  if (cx <= x) {
    eax = 0;
    if (cx >= x) {
      return;
    } else {
      eax = func4(x, cx+1, dx);
      return eax*2 + 1;
    }
  } else {
    dx = cx - 1
    eax = func4(x, si, cx - 1) ;
    return eax*2;
  }
}

```
由phase_4里的代码可知b=0, a=[0,15]且func4(a, 0, 15) == 0  
所以我们写一个测试程序来看看区间[0,15]具体哪些整数符合上述条件  
```
#include <stdio.h>

int func4(int x, int si, int dx) {
  // x, si=0, dx = 0
  int eax = ((dx-si)+((dx-si)>>31)) >> 1;
  int cx = (((dx-si)+((dx-si)>>31)) >> 1) + si;

  if (cx <= x) {
    eax = 0;
    if (cx >= x) {
      return eax;
    } else {
      eax = func4(x, cx+1, dx);
      return eax*2 + 1;
    }
  } else {
    eax = func4(x, si, cx - 1) ;
    return eax*2;
  }
}


int main(void)
{
  for (int i = 0; i<= 0xe; i++) {
    if (func4(i, 0, 0xe) == 0) {
      printf("%d\n",i);
    }
  }
  return 0;
}
```
答案：
根据测试结果，a={0,1，3,7}， b=0
