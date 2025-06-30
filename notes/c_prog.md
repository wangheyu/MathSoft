# C 语言的结构

我们知道一个 C 语言的源代码总是由头文件加上各种函数构成。其中在一个可执行项目中，必须要有一个主函数 `main()`。

比如我们写一个最基本的 hello.c 程序：
```
// 引入标准输入输出库
#include <stdio.h>  

// 主函数，程序从这里开始执行
int main() {  
    // 输出 "Hello, World!" 并换行
    printf("Hello, World!\n");  
    // 返回 0，表示程序正常结束
    return 0;  
}
```
在这里主函数调用了 `printf()` 函数，而 `printf()` 函数在 `stdio.h` 中被声明，这个文件我们可以在系统目录 `/usr/include` 下找到，我们可以观察到里面确实定义了 `printf`。
```
grep printf /usr/include/stdio.h 
```
我们会注意到
```
extern int printf (const char *__restrict __format, ...); 
```
这就是 `printf` 函数的声明位置。我们可以看到一些语法细节，比如 `extern` 表示这个函数的定义会在其他文件中实现；`printf` 函数是有返回值的，返回值是一个整数，实际上代表了成功打印的字符数。
```
printf("Hello, World!\n");
```
的返回值是 14。它的第一个参数是一个字符串，就是我们熟悉的带格式的字符串。最后的 `...` 代表可变参数，我们并不知道在一次实际使用的时候，用户会采用的参数的个数。

在 debian / ubuntu 系统下，默认情况下我们看不到 `printf` 的源码，因为它已经被预编译成二进制文件，直接供系统内的 C 语言调用使用。具体的文件是 `/lib/x86_64-linux-gnu/libc.so.6`，这是个二进制文件，你无法直接查看。

将 C 语言从源代码变到可执行文件的过程，分为编译和链接两步：
```
gcc -c hello.c
```
这步完成编译（Compilation），它会检查预编译命令是否正确（含 # 的命令，包括头文件是否存在）；函数声明是否正确且一致；语法是否一致；然后对代码进行必要的优化；最后将代码转换成机器能够理解的机器码，作为二进制的.o文件输出。在这一步，由于 `stdio.h` 文件在系统的默认头文件目录 `/usr/include` 中，所以不需要我们做任何事情，编译器 `gcc` 就会找到。
```
gcc -o hello hello.c
```
这一步是链接，既然实现 hello 的功能需要使用 printf，也就是需要找到 `/lib/x86_64-linux-gnu/libc.so.6` 这个文件，并使用它来实现 `printf` 的功能。这个过程中，我们实际上由两种选择，一种就是像上面那样的默认方式。编译器只是在可执行文件 `hello` 中打一个 `/lib/x86_64-linux-gnu/libc.so.6` 的标记，当用户真正去执行 `hello` 这个文件的时候，通过操作系统实时和`/lib/x86_64-linux-gnu/libc.so.6`对接来完成完整的功能。这么做的好处是我们在整个系统中只需要保存一份真正的`printf`的二进制功能代码，同时保证目标可执行文件 `hello` 尺寸更小。但是这么做要求我们正确地设置了动态库文件 `/lib/x86_64-linux-gnu/libc.so.6` 的位置，使得需要用到它的程序总是能正确找到。我们可以用下面的命令观察到一个可执行文件中标记了哪些动态链接库：
```
ldd hello
```
我们看到结果：
```
linux-vdso.so.1 (0x00007ffc68d40000)
libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f649612e000)
/lib64/ld-linux-x86-64.so.2 (0x00007f6496322000)
```
这里真正调用的动态链接库是第二行，为了实现 `printf`，第一行是一个接见调用，即 `printf` 实现时调用的其他函数；而第三行，其实就是为了实现动态链接库本身功能而提供的动态链接库。

除了动态链接，我们也可以使用静态链接：
```
mv hello hello_dy
gcc -o hello hello.o -static
```
我们注意到静态版本的 `hello` 比动态版本的 `hello` 要大的多，因为它完整包含了 `libc.a` 中和 `printf` 相关的部分，这个文件是 `libc.so.6` 的静态版本，相当于一个完整的 C 语言内置程序库，不但包含 `printf`，也包含 `scanf` 等等各种内置函数。相当于为了一顿饺子，把整个超市，或者至少是超市的食品部门打包带走了。所以最终生成的可执行文件会这么大。它的好处是运行时不再依赖其他文件，产生的可执行文件可以独立运行。
```
ls /usr/lib/x86_64-linux-gnu/libc.a
```

所以除了一些奇怪的场合，我们一般都会使用动态链接库。

有了以上这些知识，我们来学习如何组织我们自己的头文件和库文件。首先我们将 `hello.c` 改一下：
```
#include <stdio.h>  

int hello(char *name) {
    return printf("Hello, %s!\n", name);
}

```
然后增加一个声明 `hello()` 函数的头文件`hello.h`：
```
int hello(char *name);
```
最后再增加一个包含主函数的文件 `main.c`：
```
#include <stdio.h>
#include "hello.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("usage: %s <name>\n", argv[0]);
    } else {
        hello(argv[1]);
    }
    return 0;
}
```
编译的过程是：
```
gcc -c *.c
gcc -o hello *.o
```
这里我们顺便介绍一点语法点，主函数的参数 `argc` 表示命令行参数的个数，包含命令本身；`argv` 是一个字符串数组，依次存放了命令行参数。例如最终可执行文件是 `hello` 的话，那么在终端运行：
```
./hello Crazyfish
```
时，`argc == 2`，`argv[0] == "./hello"`，`argv[1] == Crazyfish`。当然这里我们主要目的就是调用一下我们的自定义函数 `hello()`。

第二个语法点是我们在两个地方 `#include <stdio.h>`，但是编译时不会报错重复定义。那是因为用预处理做了保护。我们改进一下我们的头文件 `hello.h`：
```
#ifndef HELLO_CRAZYFISH
#define HELLO_CRAZYFISH
int hello(char *name);
#else
//Do nothing
#endif
```
这样我们的头文件也不怕重复引用了。

现在我们将 `main.c` 复制到一个独立的目录下，那么它还能继续调用 `hello()` 么？
```
mkdir ../c_demos
mv main.c ../c_demos
```
我们尝试编译 `main.c`：
```
gcc -c main.c
```
由于 `hello.h` 不在当前目录和系统目录，果然找不到。我们手工给出路径：
```
gcc -c main.c -I ../hello
```
编译没有问题，产生了二进制文件 `main.o`。进一步链接：
```
gcc -o hello main.o
```
肯定会有问题，找不到 `hello()` 的可使用二进制代码。现在我们尝试将 `hello.o` 编译成静态和动态库供全系统，或者至少是当前用户使用。为此我们规划一下我们当前用户的头文件和库文件管理。比如我们在用户目录下建两个目录，分别叫 `include` 和 `lib`
```
mkdir ~/include ~/lib
```
我们没有必要真的把 `hello.h` 复制到 `~/include` 下，而是做个硬链接更好：
```
ln -s ~/Projects/MathSoft/src/hello/hello.h ~/include
```
效果和复制过去一毛一样，但其实只有一份文件，另一个只是一个标记，但可以通过它直接访问文件。我们今后可以将所有当前用户设置的头文件都移动或链接到这个目录，这样我们总是可以在编译的使用用`-I /home/$USER/include` 来找到我们的头文件：
```
gcc -c main.c -I /home/$USER/include
```
现在回到 `hello` 目录下，编译我们的库文件。首先是静态库：
```
ar rcs libhello.a hello.o
```
然后是动态库：
```
gcc -shared -o libhello.so hello.o
```
这里我们注意到，库命名是有规律的，静态库总是叫 `lib<库名>.a` 而动态库总是叫 `lib<库名>.so`。这个是约定的规定，并没有强制，但强烈建议遵守。
现在我们继续将这两个库链接到我们规划的 `lib` 目录下：
```
cd ~/lib
ln -s ~/Projects/MathSoft/src/hello/libhello.a .
ln -s ~/Projects/MathSoft/src/hello/libhello.so .
```
现在回到 `c_demos` 下，尝试调用这些库来完成 `hello`，首先是静态：
```
gcc main.o -L ~/lib -lhello -o hello_static -static
```
然后是动态，默认就是动态的：
```
gcc main.o -L ~/lib -lhello -o hello_dynamic
```
这里 `-L` 参数指定库的位置，`-l`参数指定具体是哪个库，不管动态还是静态，都可以省略前面的 `lib` 和后缀。`-static` 指定静态，不指定就优先动态。

现在静态库编译的版本直接就能用。但动态库就：
```
./hello_dynamic: error while loading shared libraries: libhello.so: cannot open shared object file: No such file or directory
```
因为动态库需要在执行的时候找到对应库文件的位置，这里没有找到 `libhello.so`，尽管在编译的时候告诉过它，但编译和执行是两个系统的，互相没通气。

解决的办法有很多，一种是通过环境变量：
```
export LD_LIBRARY_PATH=~/lib:$LD_LIBRARY_PATH
./hello_dynamic
```
还有一种是编译的时候让可执行文件记住去哪里找：
```
gcc main.o -L ~/lib -lhello -o hello_dynamic -Wl,-rpath=~/lib
```
这样也可以。

现在总结一下，如果我们有一个 C 的库应用，我们要使用的话，只需要清楚两件事，第一，头文件在哪里？第二，库文件在哪里。

让我们来尝试一下：libgsl

