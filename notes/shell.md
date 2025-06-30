
## Shell 编程

在本书中我们已经开始使用 C 语言编写 Linux 程序，现在我们将稍作转向，进入 Shell 程序编写的领域。为什么呢？因为 Linux 并不像那些将命令行界面作为图形界面附属功能的系统。UNIX 是 Linux 的灵感来源，它最初根本没有图形界面——一切操作都通过命令行完成。因此，UNIX 的命令行系统经历了大量发展，成为了一个非常强大的功能。这一功能被引入到 Linux 中，而一些最强大的操作也最容易通过 Shell 来完成。由于 Shell 对 Linux 来说至关重要，并且在自动化简单任务方面非常有用，因此我们在本书前期就介绍 Shell 编程。

在本章中，我们将向你展示 Shell 的语法、结构以及可用的命令，通常通过交互式（基于屏幕）的示例来进行讲解。这些示例可以作为 Shell 大多数功能及其效果的有用摘要。我们还会简单介绍几个在 Shell 中经常调用的特别有用的命令行工具：`grep` 和 `find`。在讲解 `grep` 的过程中，我们还会介绍正则表达式的基础知识，正则表达式广泛应用于 Linux 工具以及 Perl、Ruby 和 PHP 等编程语言中。在本章末尾，你将学习如何编写一个实际的脚本，该脚本将在本书中用 C 语言重新编写和扩展。

本章将涵盖以下内容：

- ✅ 什么是 Shell
- ✅ 基本注意事项
- ✅ 语法细节：变量、条件和程序控制
- ✅ 列表
- ✅ 函数
- ✅ 命令及其执行
- ✅ Here 文档
- ✅ 调试
- ✅ `grep` 和正则表达式
- ✅ `find`

无论你是在系统管理中面对一个复杂的 Shell 脚本，还是想为你的一个重大（但设计优雅）想法做原型，或只是想加快某些重复性任务的速度，本章都适合你。


## 为什么要用 Shell 编程？

使用 Shell 进行编程的一个原因是你可以快速而简单地编写程序。此外，即使是在最基本的 Linux 安装中，Shell 也是始终可用的，因此在你进行简单原型开发时，它可以帮助你快速验证想法是否可行。Shell 也非常适合用于执行一些效率要求不高的小型实用工具，这些工具更注重易于配置、维护和可移植性。你可以使用 Shell 来组织进程控制，让命令按预定顺序依次执行，取决于每个阶段是否成功完成。

虽然 Shell 在表面上与 Windows 的命令提示符相似，但它功能更强大，能够独立运行相当复杂的程序。你不仅可以执行命令和调用 Linux 实用工具，还可以编写这些命令。Shell 执行的是 Shell 脚本，这些脚本在运行时解释执行。这通常使调试变得更容易，因为你可以轻松地执行单行命令，并且不需要重新编译。然而，这也使得 Shell 不适用于对时间或处理器资源要求较高的任务。

---

## 一点哲学思考

我们来谈谈 UNIX —— 当然也包括 Linux —— 的哲学思想。UNIX 建立在高度代码重用的理念之上。你可以构建一个小而简单的工具，然后用户将它作为一个链条中的一环，与其他命令串联使用。Linux 的乐趣之一就在于它拥有种类繁多、优秀的工具。下面是一个简单的示例命令：

```bash
$ ls -al | more
```

这个命令使用了 `ls` 和 `more` 两个工具，并通过管道将文件列表的输出按页显示。每个工具只是另一个构建模块。你可以将许多小脚本组合在一起来创建大型复杂的程序集。

例如，如果你想打印一份 `bash` 手册页的参考副本，可以使用以下命令：

```bash
$ man bash | col -b | lpr
```

此外，由于 Linux 拥有自动的文件类型处理功能，用户通常不需要知道这些工具是用什么语言编写的。如果某个工具需要更高的运行速度，通常可以先在 Shell 中原型设计，然后再用 C 或 C++、Perl、Python 等更高效的语言重新实现。一旦某个想法被验证是有价值的，用更快的语言重写也值得。而如果 Shell 中的实现已足够好，那就不必再动了。

是否需要重新实现脚本，取决于它是否需要优化、是否需要具备可移植性、是否需要易于修改，以及它是否（像通常那样）已经超出了最初的用途。

> *你的 Linux 系统中已经预装了许多 Shell 脚本示例，如果你感兴趣，可以查看其中的一些，比如包安装器、`.xinitrc` 和 `startx`，以及 `/etc/rc.d` 中用于配置系统启动的脚本。*

---


## 什么是 Shell？

在深入了解如何使用 Shell 编程之前，让我们先回顾一下 Shell 的功能以及 Linux 中可用的不同 Shell。**Shell** 是一种程序，它在你与 Linux 系统之间充当接口，使你能够输入命令让操作系统执行。从这个角度看，它类似于 Windows 的命令提示符，但正如前面所提到的，Linux 的 Shell 更加强大。例如，输入和输出可以通过 `<` 和 `>` 来重定向，运行中的程序之间可以通过 `|` 管道传递数据，从子进程获取输出则可使用 `$(...)`。在 Linux 中，安装多个 Shell 并允许不同用户选择自己喜欢的 Shell 是完全可行的。图 2-1 展示了 Shell（图中以 bash 和 csh 为例）及其他程序是如何围绕 Linux 内核而存在的。

---

- 内核（Kernel）位于中心
- 周围有 csh、bash、其他程序（Other programs）和 X 窗口系统（The X Window System）

---

由于 Linux 的模块化特性，你可以选择使用多种 Shell 中的任何一种，尽管大多数 Shell 都是从原始 Bourne Shell 演化而来的。在 Linux 中，标准的 Shell 通常安装为 `/bin/sh`，它被称为 **bash**（GNU Bourne-Again SHell，GNU 工具集的一部分）。由于 bash 是一个极好的 Shell，始终预装在 Linux 系统中，是开源的，并且几乎可以在所有 UNIX 的变体中使用，因此我们将在本章中使用 bash。本章使用的是 bash 第 3 版，并主要使用了所有符合 POSIX 标准的 Shell 所共有的功能。我们假设系统中已将 bash 安装为 `/bin/sh`，并且它是你登录时的默认 Shell。在大多数 Linux 发行版中，程序 `/bin/sh` 实际上是一个指向 `/bin/bash` 的链接。

你可以使用以下命令检查你当前的 bash 版本：

```bash
$ /bin/bash --version
GNU bash, version 3.2.9(1)-release (i686-pc-linux-gnu)
Copyright (C) 2005 Free Software Foundation, Inc.
```

---

📦 **提示：**

**如何切换到其他 Shell ——**  
如果 bash 不是你系统中的默认 Shell，例如 ——  
只需执行所需 Shell 的程序（例如 `/bin/bash`）即可运行新 Shell 并更改命令提示符。如果你正在使用 UNIX，而系统中尚未安装 bash，你可以从 GNU 官方网站 [www.gnu.org](http://www.gnu.org) 免费下载。**这些源码高度可移植，几乎可以在任何 UNIX 系统上编译并直接运行。**

---

当你创建 Linux 用户时，可以设置他们将要使用的 Shell，无论是在创建用户时设置，还是之后通过修改用户信息来设置。图 2-2 展示了在 Fedora 系统中为用户选择 Shell 的界面。

（图 2-2 显示了 Fedora 中“用户管理器”的界面，选中用户 `rick` 并设置其登录 Shell 为 `/bin/bash`，其他可选项包括 `/bin/csh`, `/bin/sh`, `/bin/tcsh`, `/sbin/nologin` 等）

---

许多其他的 Shell 也可以使用，有些是免费的，有些是商业软件。下表简要总结了几种较为常见的 Shell 及其背景：

| **Shell 名称** | **简要历史** |
|----------------|----------------|
| **sh (Bourne)** | 最早版本 UNIX 中的原始 Shell。 |
| **csh, tcsh, zsh** | C Shell 及其衍生版本，最初由伯克利 UNIX 的 Bill Joy 创建。C Shell 是继 bash 和 Korn Shell 之后第三大流行的 Shell 类型。 |
| **ksh, pdksh** | Korn Shell 及其公共领域版本，由 David Korn 编写，是许多商业 UNIX 版本的默认 Shell。 |
| **bash** | GNU 项目的 Linux 主流 Shell。bash（Bourne Again SHell）具有源码可自由获取的优势，即使你的 UNIX 系统当前未运行 bash，它很可能也已被移植过去。bash 在很多方面类似于 Korn Shell。 |

---

除了 C Shell 及其少量衍生版本外，其他所有 Shell 都非常相似，并且与 X/Open 4.2 和 POSIX 1003.2 规范中的 Shell 要求高度一致。POSIX 1003.2 提供了 Shell 的最小规范，而 X/Open 中的扩展规范则提供了更友好、更强大的 Shell。X/Open 通常要求更严格，但也带来了更友好的系统体验。

---

## 管道与重定向

在深入讲解 Shell 程序细节之前，我们需要简单了解一下 Linux 程序（不仅仅是 Shell 程序）的输入输出是如何进行重定向的。

---

### 重定向输出（Redirecting Output）

你可能已经熟悉一些重定向的用法，例如：

```bash
$ ls -l > lsoutput.txt
```

这条命令会将 `ls` 命令的输出保存到一个名为 `lsoutput.txt` 的文件中。

然而，重定向的内容远不止这个简单示例所展示的。你将在第 3 章中进一步了解标准文件描述符，但现在你只需知道：  
- 文件描述符 0 表示标准输入（stdin）  
- 文件描述符 1 表示标准输出（stdout）  
- 文件描述符 2 表示标准错误输出（stderr）

你可以分别对它们进行重定向。实际上，你甚至可以重定向除 0、1、2 之外的其他文件描述符，不过这种情况比较少见。

上面的例子就是通过 `>` 操作符将标准输出重定向到文件中。默认情况下，如果文件已存在，它将被覆盖。如果你不想让它被覆盖，可以使用命令：

```bash
set -o noclobber
```

或

```bash
set -C
```

这将启用 `noclobber` 选项以防止文件被重定向覆盖。你也可以使用如下命令取消该选项：

```bash
set +o noclobber
```

在本章后面你会看到更多关于 `set` 命令的选项。

---

要将输出追加到文件中，可以使用 `>>` 操作符。例如：

```bash
$ ps >> lsoutput.txt
```

这将把 `ps` 命令的输出追加到指定文件的末尾。

---

要重定向标准错误输出，需要在 `>` 操作符前加上要重定向的文件描述符编号。因为标准错误是文件描述符 2，所以使用 `2>` 操作符。这通常用于丢弃错误信息或防止错误信息出现在屏幕上。

---

假设你想从脚本中使用 `kill` 命令终止某个进程。总有一点风险是该进程会在 `kill` 命令执行之前已经终止。这种情况下，`kill` 会将错误信息写到标准错误输出（默认会显示在屏幕上）。你可以通过同时重定向标准输出和标准错误来避免 `kill` 命令在屏幕上输出任何内容。

如下命令：

```bash
$ kill -HUP 1234 >killout.txt 2>killerr.txt
```

将会把输出和错误信息分别重定向到不同的文件中。

---

如果你希望将标准输出和标准错误输出都写入同一个文件，你可以使用 `>&` 操作符将两者合并。因此：

```bash
$ kill -1 1234 >killouterr.txt 2>&1
```

会将标准输出和错误输出都写入同一个文件 `killouterr.txt`。请注意操作符的顺序：这条命令的含义是“将标准输出重定向到 `killouterr.txt`，然后将标准错误也重定向到标准输出的位置”。如果顺序错误，重定向可能不会按你期望的方式工作。

由于你可以通过返回码来判断 `kill` 命令的执行结果（本章后面会详细讨论），你通常并不需要保存标准输出或标准错误。你可以使用 Linux 通用的“黑洞”设备 `/dev/null` 来高效地丢弃所有输出，例如：

```bash
$ kill -1 1234 >/dev/null 2>&1
```

---

## 重定向输入（Redirecting Input）

就像可以重定向输出一样，你也可以重定向输入。例如：

```bash
$ more < killout.txt
```

显然，在 Linux 中这是个比较形式化的例子；因为 Linux 的 `more` 命令完全可以直接接受文件名作为参数，不像 Windows 的命令行工具那样受限。

---

## 管道（Pipes）

你可以使用管道操作符（`|`）将进程连接起来。在 Linux 中，不同于 MS-DOS，通过管道连接的进程可以同时运行，并且会根据数据流自动重新调度。  
比如你想用 `sort` 命令对 `ps` 命令的输出进行排序，这时可以使用如下简单示例：

如果你不使用管道，你必须使用多步操作，如：

```bash
$ ps > psout.txt
$ sort psout.txt > pssort.out
```

一个更优雅的做法是使用管道将进程连接在一起：

```bash
$ ps | sort > pssort.out
```

如果你想将结果在屏幕上分页显示，可以再连接一个第三个进程（例如 `more`），在同一命令行中完成：

```bash
$ ps | sort | more
```

实际上，连接的进程数量几乎没有限制。假设你想查看当前正在运行的所有进程名称，但排除 Shell 进程，可以使用：

```bash
$ ps -xo comm | sort | uniq | grep -v sh | more
```

这条命令会将 `ps` 的输出按字母顺序排序，使用 `uniq` 去除重复项，然后用 `grep -v sh` 过滤掉名称中包含 `sh` 的 Shell 进程，最后使用 `more` 分页显示结果。


正如你所见，相比一串分开的命令每次都生成临时文件的方式，使用管道是一种更优雅的解决方案。但这里有一点需要注意：如果你使用的是一串命令，那么输出文件会在命令开始执行时立即被创建或写入，因此**绝不要在同一串命令中两次使用相同的文件名**。比如，如果你尝试这样做：

```bash
cat mydata.txt | sort | uniq > mydata.txt
```

你将得到一个空文件，因为在读取 `mydata.txt` 之前它就已经被覆盖了。

---

## 将 Shell 作为编程语言使用

既然你已经了解了一些基本的 Shell 操作，现在是时候进入 Shell 脚本编程的实际内容了。有两种编写 Shell 程序的方式：

- 你可以输入一系列命令，并让 Shell 交互式地执行它们；
- 或者你可以将这些命令存储在一个文件中，之后像程序一样调用它。

---

### 交互式程序（Interactive Programs）

直接在命令行中输入 Shell 脚本是一种快速简便的方式，适合尝试一些小片段的代码，对于学习或测试非常有用。

假设你有许多 C 文件，想要检查哪些文件中包含字符串 `POSIX`。与其使用 `grep` 命令搜索并手动列出文件，你可以编写一个交互式脚本像下面这样操作：

```bash
$ for file in *
> do
> if grep -l POSIX $file
> then
> more $file
> fi
> done
posix
This is a file with POSIX in it — treat it well
$
```

请注意，当 Shell 期待后续输入时，提示符从 `$` 变成了 `>`。你可以继续输入，Shell 会在你完成时自动执行整个脚本。

在这个例子中，`grep` 命令会找到包含 `POSIX` 的文件，`more` 命令则显示文件内容。最后 Shell 提示符会返回。你还调用了 Shell 变量 `file` 来标识每个文件，从而让脚本更具可读性。你也可以使用其他变量名，比如 `i`，但 `file` 更容易理解。

---

Shell 还会执行通配符扩展（通常称为 **globbing**）。你可能已经知道 `*` 可作为通配符匹配任意字符字符串。你或许不知道的是：

- `?` 可用于匹配任意单个字符；
- `[set]` 匹配 `set` 中的任意一个字符；
- `[^set]` 表示匹配不在 `set` 中的字符（取反）；
- 使用 `{}`（在一些 Shell 中，如 `bash`）可以进行**花括号扩展**，将一组任意字符串组合在一起，Shell 会自动扩展。

例如：

```bash
$ ls my_{finger,toe}s
```

将会扩展为：

```bash
ls my_fingers my_toes
```

---

这条命令会列出当前目录中符合匹配规则的文件，例如 `my_fingers` 和 `my_toes`。Shell 会检查当前目录下的所有文件。我们将在本章后面更详细地讲解匹配模式规则，特别是在介绍 `grep` 和正则表达式的强大功能时。

有经验的 Linux 用户可能会用更高效的方式来执行这个操作，比如使用以下命令：

```bash
$ more `grep -l POSIX *`
```

或者它的等价形式：

```bash
$ more $(grep -l POSIX *)
```

此外，你还可以这样写：

```bash
$ grep -l POSIX * | more
```

这条命令会输出包含字符串 **POSIX** 的所有文件名，并通过 `more` 分页显示其内容。在这个脚本中，你可以看到 Shell 是如何通过 `grep` 和 `more` 等其他命令来完成主要工作的。Shell 的作用就是让你可以以新的、有力的方式将多个已有命令组合在一起。

你将在后面的脚本中多次看到通配符扩展的使用，我们还会在介绍 `grep` 命令时详细讲解正则表达式。

---

每次都要手动输入一串命令非常麻烦。你可以将命令保存到一个文件中，这样的文件通常被称为 **Shell 脚本（shell script）**，你可以随时执行它们。

---

## 创建脚本（Creating a Script）

你可以使用任何文本编辑器来创建一个包含命令的文件。比如，创建一个名为 `first` 的文件，内容如下：

```sh
#!/bin/sh

# first
# 此脚本会遍历当前目录下的所有文件，
# 查找包含字符串 POSIX 的文件，并将这些
# 文件的文件名输出到标准输出。

for file in *
do
  if grep -q POSIX $file
  then
    echo $file
  fi
done

exit 0
```

注释以 `#` 开头，并持续到行尾。按照惯例，`#` 应放在行首。同时需要注意，第一行 `#!/bin/sh` 是一种特殊的声明形式。

注释中的 `#!` 字符告诉系统，这行之后的参数是用来执行该文件的程序。在这个例子中，`/bin/sh` 是默认的 Shell 程序。

> **注意注释中指定的绝对路径。** 约定俗成地，建议该路径不要超过 32 个字符，以兼容某些较老的 UNIX 系统（它们在使用 `#!` 时只支持有限长度）。不过，Linux 通常不受此限制。

---

由于脚本本质上被当作标准输入传递给 Shell，因此它可以包含任何 PATH 环境变量中引用的 Linux 命令。

`exit` 命令用于确保脚本返回一个合理的退出码（稍后在本章中会详细介绍）。在交互执行程序时，退出码很少被检查，但如果你希望从另一个脚本中调用此脚本并检查它是否成功执行，那么返回一个合适的退出码就非常重要。  
即使你不打算让脚本被其他脚本调用，也应该养成良好习惯，使用合理的退出码。相信你的脚本可能将来会被复用！

在 Shell 编程中，**退出码为 0 表示成功**。当前脚本中没有错误检测，因此它总是返回成功。我们稍后会在讲解 `exit` 命令时解释为何要返回 0。

---

注意，这个脚本没有使用任何文件扩展名。在 Linux（以及 UNIX）中，文件扩展名并不用于判断文件类型。你可以使用 `.sh` 扩展名，也可以不用，Shell 并不在意。  
大多数预装脚本也没有文件扩展名。判断某个文件是否为脚本的最佳方式是使用 `file` 命令，例如：

```bash
file first
file /bin/bash
```

你可以根据自己的工作环境或喜好采用不同的命名约定。

---

## 让脚本可执行（Making a Script Executable）

当你创建好脚本文件后，有两种方式可以运行它。**更简单的方式**是通过 Shell 调用脚本文件名作为参数：

```bash
$ /bin/sh first
```

这当然可以运行，但如果你可以**直接键入脚本名来执行**，就像使用其他 Linux 命令一样，那就更好了。为此，你需要通过 `chmod` 命令更改文件权限，使其对所有用户可执行：

```bash
$ chmod +x first
```

> 🛠 **当然，`chmod` 的用法远不止于此。**  
> 使用 `man chmod` 可以了解更多关于八进制参数和其他选项的信息。

然后你就可以使用以下命令来执行脚本了：

```bash
$ first
```

你可能会遇到“找不到命令”的错误。这几乎可以肯定是因为 Shell 的环境变量 `PATH` 没有设置为查找当前目录的命令。要解决这个问题，你可以在命令行中输入：

```bash
PATH=.:$PATH
```

或者编辑你的 `.bash_profile` 文件，在文件末尾添加这条命令，然后注销并重新登录。

另一个替代方案是，在包含脚本的目录中使用如下方式运行脚本：

```bash
./first
```

这将告诉 Shell 使用脚本的完整相对路径来执行它。

---

使用 `./` 前缀还有另一个优点：它可以确保你不会意外执行系统中与脚本同名的其他命令。

> ⚠️ **不要以这种方式更改 `PATH` 变量用于超级用户（root）。**  
> 这是一个安全漏洞，因为系统管理员以 root 身份登录时可能会被诱导执行伪装成标准命令的恶意脚本。  
> 一位作者曾经为了向系统管理员证明这个问题的严重性，做过一次这样的实验。当然，这只是为了安全教育！  
> 虽然对普通账户来说这只是一个小风险，但如果你特别在意安全性，请养成良好习惯，**在本地目录中的命令前加 `./` 前缀**。

---

一旦你确认脚本可以正常运行，你可以将其移动到比当前目录更合适的位置。

- 如果脚本只是你个人使用，可以在你的主目录中建立一个 `bin` 目录，并将其添加到你的 `PATH`。
- 如果希望其他人也能执行该脚本，可以将其放入 `/usr/local/bin/` 或其他系统目录中。

如果你没有 root 权限，可以请求系统管理员帮你复制脚本文件。当然，你可能需要先说服他们这个脚本确实有用。

为了防止其他用户（可能是无意间）修改脚本，你应该移除脚本的写权限。以下是为脚本设置所有权和权限的命令序列：

```bash
# cp first /usr/local/bin
# chown root /usr/local/bin/first
# chgrp root /usr/local/bin/first
# chmod 755 /usr/local/bin/first
```

请注意，相较于改变权限标志的某一部分，这里使用的是 `chmod` 的绝对形式，因为你清楚知道你需要哪些权限。

如果你愿意，也可以使用更长但更清晰的 `chmod` 命令形式：

```bash
# chmod u=rwx,go=rx /usr/local/bin/first
```

想了解更多细节，请查看 `chmod` 的手册页。

---

> 💡 **在 Linux 中，如果你对某个目录具有写权限，即可删除该目录中的文件。**  
> 为了安全起见，请确保只有超级用户对那些包含重要文件的目录拥有写权限。  
> 这非常合理，因为目录其实只是一个保存文件名的文件，而对目录具有写权限就意味着可以添加或删除文件名。


## Shell 语法（Shell Syntax）

现在你已经见过一个简单的 shell 程序示例，是时候更深入地了解 shell 的编程能力了。Shell 是一种非常容易学习的编程语言，部分原因是它可以轻松地交互式测试小的程序片段，然后再将它们组合成更大的脚本。你可以使用 `bash` shell 编写相当大且结构化的程序。

接下来的几个小节将涵盖以下内容：

- ✅ 变量：字符串、数字、环境变量和参数  
- ✅ 条件语句：Shell 中的布尔值  
- ✅ 程序控制结构：`if`, `elif`, `for`, `while`, `until`, `case`  
- ✅ 列表  
- ✅ 函数  
- ✅ Shell 内置命令  
- ✅ 获取命令的输出结果  
- ✅ Here 文档（Here documents）

---

## 变量（Variables）

在 Shell 中你通常不需要在使用前声明变量。你只需直接使用它们（比如直接赋一个初始值）。默认情况下，所有变量都被当作字符串处理和存储，即使它们被赋予的是数字值。

Shell 及一些工具会在需要时将数值字符串转换为真正的数值以便进行运算。Linux 是区分大小写的系统，因此变量 `foo` 与 `Foo` 是不同的，同时它们也都不同于 `FOO`。

在 Shell 中，要访问变量的值，需要在变量名前加上 `$`。每次提取变量内容时，都必须加上 `$`。赋值时，只写变量名（Shell 会在必要时动态创建该变量）。检查变量值的一种简单方法是通过 `echo` 命令输出它，加上 `$` 前缀即可。

在命令行中，你可以看到如下变量 `salutation` 的赋值与输出示例：

```bash
$ salutation=Hello
$ echo $salutation
Hello

$ salutation="Yes Dear"
$ echo $salutation
Yes Dear

$ salutation=7+5
$ echo $salutation
7+5
```

> 💡 **注意：** 如果字符串中包含空格，必须使用引号括起来。此外，**等号两边不能有空格。**


你可以使用 `read` 命令将用户输入赋值给变量。这个命令接受一个参数，即变量名，然后等待用户输入文本。当用户按下回车键时，`read` 命令完成读取。

从终端读取变量时，通常不需要使用引号。例如：

```bash
$ read salutation
Wie geht's?
$ echo $salutation
Wie geht's?
```

---

## 引号（Quoting）

在继续之前，你需要了解 Shell 的一个重要特性：**引号的使用**。

在脚本中，参数通常由空白字符（如空格、Tab 或换行符）分隔。如果你希望一个参数中包含一个或多个空白字符，就必须对它加引号。

变量（例如 `$foo`）在引号中的行为取决于你使用的是哪种引号：

- 如果你使用**双引号**（`"`），变量会在执行时被替换为它的值；
- 如果你使用**单引号**（`'`），则不会发生变量替换；
- 如果你希望输出 `$` 本身的字面含义，可以在前面加反斜杠 `\` 进行转义。

通常，字符串使用双引号括起来，这样可以防止变量因空白字符而被分割，同时允许 `$` 变量展开。

---

## 🧪 试试看：变量实验（Try It Out — Playing with Variables）

下面这个示例展示了引号对变量输出结果的影响：

```sh
#!/bin/sh

myvar="Hi there"

echo $myvar
echo "$myvar"
echo '$myvar'
echo \$myvar

echo Enter some text
read myvar

echo '$myvar' now equals $myvar
exit 0
```

### 执行效果如下：

```bash
$ ./variable
Hi there
Hi there
$myvar
$myvar
Enter some text
```

---

这个例子清晰地展示了变量在不加引号、双引号、单引号和加反斜杠时的不同表现。


### **它是如何工作的（How It Works）**

变量 `myvar` 被创建并赋值为字符串 `Hi there`。使用 `echo` 命令显示变量的内容，展示了在变量名前加上 `$` 符号可以展开变量的值。你可以看到：

- 使用**双引号**不会影响变量的替换；
- 使用**单引号**或**反斜杠**则不会进行变量替换。

你还使用了 `read` 命令从用户处获取字符串输入。

---

## 环境变量（Environment Variables）

当 Shell 脚本启动时，一些变量会从环境中被初始化。这些变量通常使用全大写字母，以便与脚本中用户自定义的变量（通常使用小写）区分开来。  
这些环境变量的值取决于你的个人配置，很多都可以在手册页中找到。以下是最常用的一些变量：

| 环境变量（Environment Variable） | 描述（Description） |
|-----------------------------|-------------------------------|
| `$HOME` | 当前用户的主目录 |
| `$PATH` | 用冒号分隔的目录列表，用于查找命令 |
| `$PS1` | 主命令提示符，通常是 `$`，但在 bash 中你可以使用更复杂的值。例如 `[\u@\h \w]\$` 是一个常见默认值，显示用户名、主机名、当前目录，并提供 `$` 提示符 |
| `$PS2` | 二级提示符，在要求用户输入额外内容时使用，通常是 `>` |
| `$IFS` | 输入字段分隔符。用于分隔输入中的单词，通常包括空格、Tab 和换行符 |
| `$0` | 当前执行的 Shell 脚本名称 |
| `$#` | 传递给脚本的参数数量 |
| `$$` | 当前 Shell 脚本的进程 ID，常用于生成唯一的临时文件名，例如 `/tmp/tmpfile_$$` |

---

> 💡 **提示：**  
> 如果你想查看程序在不同环境下的运行效果，可以使用 `env <命令>` 来运行，或查阅 `env` 命令的手册。  
> 在本章后面你还将学习如何使用 `export` 命令在子 Shell 中设置环境变量。


## 参数变量（Parameter Variables）

如果你的脚本在执行时传入了参数，那么会自动创建一些额外的变量。如果没有传入参数，环境变量 `$#` 仍然存在，但其值为 `0`。

参数变量如下表所示：

| 参数变量（Parameter Variable） | 描述（Description） |
|-------------------------------|---------------------|
| `$1`, `$2`, … | 传递给脚本的参数 |
| `$*` | 所有参数组成的一个单一字符串，参数之间由环境变量 `IFS` 的首个字符分隔。如果你修改了 `IFS`，则 `$*` 分隔参数的方式也会随之改变 |
| `$@` | `$@` 是 `$*` 的一个细微变体：它不使用 `IFS` 环境变量，因此即使 `IFS` 是空的，参数也不会被合并成一个整体，而是保留为独立项 |

---

你可以通过以下例子轻松看出 `$@` 和 `$*` 的区别：

```sh
$ IFS=''
$ set foo bar bam
$ echo "$@"
foo bar bam
$ echo "$*"
foobarbam
$ unset IFS
$ echo "$*"
foo bar bam
```

如你所见，在双引号中，`$@` 会将位置参数作为独立字段展开，无论 `IFS` 的值为何。  
通常来说，如果你想访问参数，使用 `$@` 更为稳妥。

除了使用 `echo` 打印变量内容外，你也可以使用 `read` 命令读取变量。

---

## 🧪试试看：操作参数和环境变量（Try It Out — Manipulating Parameter and Environment Variables）

下面这个脚本展示了一些简单的变量操作方法。输入并保存该脚本为 `try_var` 后，别忘了通过以下命令赋予它可执行权限：

```bash
chmod +x try_var
```

脚本内容如下：

```sh
#!/bin/sh

salutation="Hello"
echo $salutation

echo "The program $0 is now running"
echo "The second parameter was $2"
echo "The first parameter was $1"
echo "The parameter list was $*"
echo "The user's home directory is $HOME"

echo "Please enter a new greeting"
read salutation

echo $salutation
echo "The script is now complete"
exit 0
```

如果你运行这个脚本，例如输入：

```bash
$ ./try_var foo bar baz
```

你会得到如下输出：

```
Hello
The program ./try_var is now running
The second parameter was bar
The first parameter was foo
The parameter list was foo bar baz
The user's home directory is /home/rick
Please enter a new greeting
Sire
Sire
The script is now complete
```

---

## 它是如何工作的（How It Works）

这个脚本创建了变量 `salutation`，显示其值，然后展示了如何使用各种参数变量和环境变量。变量 `$HOME` 已经存在，并且具有合适的值。

我们将在本章稍后更详细地讲解参数替换（parameter substitution）。

---

## 条件语句（Conditions）

所有编程语言的基础功能之一就是测试条件，并根据结果执行不同的操作。在深入具体控制结构之前，先来看看在 shell 脚本中可以使用的条件测试方法。

Shell 脚本可以测试任何命令的退出码，无论是系统命令还是你自己编写的脚本。这就是为什么在你编写的每个脚本最后都应该加上 `exit` 命令并设置一个返回值的原因。

---

## `test` 或 `[` 命令（The `test` or `[` Command）

在实际应用中，大多数脚本广泛使用 `[` 或 `test` 命令来进行布尔检查（也就是条件判断）。在某些系统中，`[` 和 `test` 命令是等价的，唯一的区别是使用 `[` 命令时需要在末尾加上 `]`，这样写法更易读。

虽然 `[ ... ]` 的语法看起来有点奇怪，但在 shell 中它让命令看起来更简洁、清晰，也更像其他编程语言中的语法结构。


> 这些命令在一些旧版 UNIX shell 中会调用外部程序，但在较新的系统中通常是内建的。我们稍后在讲解命令时会再回到这个话题。
>
> 由于 `test` 命令在 Shell 脚本之外使用得不多，很多从未编写过 Shell 脚本的 Linux 用户都会尝试写一些简单脚本并调用 `test`。如果某个脚本无法运行，很可能是因为外部的 `test` 程序与 Shell 的 `test` 命令发生了冲突。
>
> 要检查系统中是否存在某个名称的外部命令，可以尝试输入 `which test` 来查看使用的是哪个 `test` 命令。你也可以使用 `./test` 来确保执行的是当前目录下的脚本。
>
> 如果不确定，建议养成一个好习惯：**执行脚本时在脚本名前加上 `./`**。

---

我们将用最简单的条件之一来介绍 `test` 命令：**检查文件是否存在**。

命令格式为：

```bash
test -f <filename>
```

因此你可以在脚本中这样写：

```bash
if test -f fred.c
then
  ...
fi
```

你也可以这样写：

```bash
if [ -f fred.c ]
then
  ...
fi
```

`test` 命令的**退出码**（也就是条件是否成立）决定了后续条件代码是否会执行。

---

> ⚠️ **注意：你必须在 `[` 和条件表达式之间加空格。**
>
> 你可以这样记住：`[` 实际上就等同于 `test` 命令，因此就像 `test` 后面需要空格一样，`[` 后也必须加空格。
>
> **如果你希望将 `then` 写在 `if` 同一行上，必须用分号 `;` 将 `test` 和 `then` 分隔开：**
>
> ```bash
> if [ -f fred.c ]; then
>   ...
> fi
> ```

---

`test` 命令支持的条件类型大致分为三类：

1. **字符串比较（string comparison）**  
2. **算术比较（arithmetic comparison）**  
3. **文件条件（file conditionals）**

下表将介绍这些条件类型（见下一页或下一节）。


## 字符串比较（String Comparison）

| 表达式 | 结果 |
|--------|------|
| `string1 = string2` | 如果两个字符串相等，结果为真 |
| `string1 != string2` | 如果两个字符串不相等，结果为真 |
| `-n string` | 如果字符串非空，结果为真 |
| `-z string` | 如果字符串为空（即空字符串），结果为真 |

---

## 算术比较（Arithmetic Comparison）

| 表达式 | 结果 |
|--------|------|
| `expression1 -eq expression2` | 如果两个表达式相等，结果为真 |
| `expression1 -ne expression2` | 如果两个表达式不相等，结果为真 |
| `expression1 -gt expression2` | 如果 expression1 大于 expression2，结果为真 |
| `expression1 -ge expression2` | 如果 expression1 大于或等于 expression2，结果为真 |
| `expression1 -lt expression2` | 如果 expression1 小于 expression2，结果为真 |
| `expression1 -le expression2` | 如果 expression1 小于或等于 expression2，结果为真 |
| `! expression` | 如果表达式为假，结果为真（逻辑非）|

---

## 文件条件判断（File Conditional）

| 表达式 | 结果 |
|--------|------|
| `-d file` | 如果文件是目录，结果为真 |
| `-e file` | 如果文件存在，结果为真。**注意：`-e` 在历史上不够通用，所以通常使用 `-f` 替代。** |
| `-f file` | 如果文件是普通文件，结果为真 |
| `-g file` | 如果文件设置了 set-group-id 位，结果为真 |
| `-r file` | 如果文件可读，结果为真 |
| `-s file` | 如果文件大小不为零，结果为真 |
| `-u file` | 如果文件设置了 set-user-id 位，结果为真 |
| `-w file` | 如果文件可写，结果为真 |
| `-x file` | 如果文件可执行，结果为真 |

--- 

这些条件可以与 `test` 命令或 `[ ... ]` 语法一起使用，在 Shell 脚本中用于实现逻辑判断。


> **你可能会好奇什么是 `set-group-id` 和 `set-user-id`（也称为 `set-gid` 和 `set-uid`）位。**
>
> - `set-uid` 位让程序**以文件拥有者的权限**运行，而不是以运行者的权限。
> - `set-gid` 位让程序**以文件所属用户组的权限**运行。
>
> 这两个位可以通过 `chmod` 命令设置，使用选项 `s` 和 `g`。
>
> ⚠️注意：`set-gid` 和 `set-uid` 标志**对包含 Shell 脚本的文件无效，仅对可执行的二进制文件有效**。

---

虽然我们稍微提前介绍了这些内容，但下面这个例子展示了如何检测文件 `/bin/bash` 的状态，让你看看这些条件在实际中是如何使用的：

```sh
#!/bin/sh

if [ -f /bin/bash ]
then
  echo "file /bin/bash exists"
fi

if [ -d /bin/bash ]
then
  echo "/bin/bash is a directory"
else
  echo "/bin/bash is NOT a directory"
fi
```

---

要使测试结果为真，所有**文件条件测试**都要求文件本身必须存在。

上面的列表只是 `test` 命令中**最常用的一些选项**。想查看完整选项，请参考 `test` 命令的手册页。如果你使用的是 bash，它内建了 `test` 命令，可以输入：

```bash
help test
```

来获取更多详细信息。本章稍后我们还会继续使用这些选项。

---

现在你已经了解了“条件”，我们可以开始学习使用这些条件的**控制结构**了。

---

## 控制结构（Control Structures）

Shell 提供了一套控制结构，这些结构与其他编程语言中的非常相似。

> 💡 **说明：在接下来的部分中，"语句" 指的是在满足某个条件时执行的一系列命令。**

---

### `if` 语句

`if` 语句非常简单：它会测试一条命令的执行结果，然后根据结果有条件地执行一组语句：

```sh
if condition
then
  statements
else
  statements
fi
```

---

`if` 的一个常见用途是：**提出一个问题并根据答案做出决定**：

```sh
#!/bin/sh

echo "Is it morning? Please answer yes or no"
read timeofday

if [ $timeofday = "yes" ]; then
  echo "Good morning"
else
  echo "Good afternoon"
fi

exit 0
```

这个脚本的运行结果如下：

```
Is it morning? Please answer yes or no
yes
Good morning
```

---

这个脚本使用 `[` 命令来检查变量 `timeofday` 的内容。判断结果由 `if` 命令来评估，从而执行不同的代码。

> 💡 **注意：你可以使用额外的空格对 `if` 块中的语句进行缩进。这只是为了让代码对人类更易读，Shell 实际上会忽略这些多余的空格。**

---

### `elif`

不幸的是，上面这个简单脚本存在几个问题：

- 它只接受 `yes`，并将**除 `yes` 之外的任何输入都当作 `no`**。
  
你可以使用 `elif`（**else if** 的缩写）结构来避免这个问题。

`elif` 允许你在 `else` 部分执行之前，检查第二个条件。

---

## 🧪试试看：使用 `elif` 进行检查（Try It Out — Doing Checks with an `elif`）

你可以修改上面的脚本，使其在用户输入的不是 `yes` 或 `no` 时给出错误提示。

方法是将 `else` 替换为 `elif`，并添加另一个条件判断：

```sh
#!/bin/sh

echo "Is it morning? Please answer yes or no"
read timeofday

if [ $timeofday = "yes" ]
then
  echo "Good morning"
elif [ $timeofday = "no" ]
then
  echo "Good afternoon"
else
  echo "Sorry, $timeofday not recognized. Enter yes or no"
  exit 1
fi

exit 0
```


## 它是如何工作的（How It Works）

这个脚本与前面的版本非常相似，但现在使用了 `elif` 命令：**当第一个 `if` 条件不成立时，它会再次测试变量的值**。

如果两个条件都不满足，就会输出一条错误信息，并以退出码 `1` 结束脚本。调用程序可以通过这个退出码判断脚本是否执行成功。

---

## 变量的问题（A Problem with Variables）

上面的脚本修复了最明显的缺陷，但还有一个更微妙的问题。

**试试运行这个新脚本，但什么都别输入，直接按下回车（Enter）键**。你将看到如下错误信息：

```
[: =: unary operator expected
```

### 发生了什么？

问题出现在第一个 `if` 判断中。

当 `timeofday` 变量是空字符串时，`if` 语句实际变成了：

```sh
if [ = "yes" ]
```

这不是一个有效的条件表达式。

---

### ✅ 解决方法：

**在变量两边加上双引号，避免空值导致语法错误：**

```sh
if [ "$timeofday" = "yes" ]
```

如果变量为空，上面的判断就会变成：

```sh
if [ "" = "yes" ]
```

这是一个合法的条件表达式。

---

### 新脚本如下：

```sh
#!/bin/sh

echo "Is it morning? Please answer yes or no"
read timeofday

if [ "$timeofday" = "yes" ]
then
  echo "Good morning"
elif [ "$timeofday" = "no" ]; then
  echo "Good afternoon"
else
  echo "Sorry, $timeofday not recognized. Enter yes or no"
  exit 1
fi

exit 0
```

如果用户只是按下了 Enter 键而没有输入内容，这样处理是安全的。

---

> 💡 **如果你希望 `echo` 命令不要在末尾输出换行符，最通用的做法是使用 `printf` 命令**（稍后章节会讲到 `printf`），而不是使用 `echo`。
>
> 有些 Shell 提供 `echo -e` 来启用转义字符，但**并非所有系统都支持**。
>
> bash 支持 `echo -n` 来取消换行，所以如果你确定你的脚本只在 bash 中运行，可以使用这个方式。
>
> ```sh
> echo -n "Is it morning? Please answer yes or no: "
> ```
>
> 📌 **注意：在引号结尾前多加一个空格，以便用户输入时字符不会紧贴提示符，看起来更整洁。**

---

## `for` 循环

使用 `for` 构造可以遍历一组值，这些值可以是任意的字符串集合。它们可以直接在程序中列出，也可以通过 Shell 的文件名扩展生成。

### 语法如下：

```sh
for variable in values
do
  statements
done
```

---

### 💡试试看：使用固定字符串进行 `for` 循环

这些值通常是字符串，因此你可以写成如下形式：

```sh
#!/bin/sh

for foo in bar fud 43
do
  echo $foo
done
exit 0
```

---

### 这个脚本的输出结果是：

```
bar
fud
43
```


> **如果你将第一行从 `for foo in bar fud 43` 改为 `for foo in "bar fud 43"` 会发生什么？**  
> 记住，加上引号后，Shell 会将引号中的内容视为一个整体字符串。这是让变量中包含空格的一种方式。

---

## 它是如何工作的（How It Works）

这个例子创建了变量 `foo`，并在每次循环中赋予它不同的值。由于 Shell 默认将所有变量视为字符串，因此使用字符串 `43` 和 `fud` 是完全合法的。

---

## 🧪 试试看：使用通配符扩展的 for 循环

如前所述，`for` 循环常与 Shell 中的文件名扩展结合使用。这意味着你可以用通配符表示一组文件名，让 Shell 在运行时自动展开。

你已经在之前的例子中见过这一点（如 `first` 示例）。该脚本使用 `*` 扩展匹配当前目录中所有文件名，并将每个文件名赋值给变量 `$file` 来逐个处理。

让我们快速看一个新的例子：假设你想打印出当前目录下所有以字母 **f** 开头、以 **.sh** 结尾的脚本文件：

```sh
#!/bin/sh

for file in $(ls f*.sh); do
  lpr $file
done
exit 0
```

---

## 工作原理（How It Works）

这个例子使用了 `$(command)` 语法（后面章节会详细讲解）。基本上，`for` 循环的参数由 `$(ls f*.sh)` 的输出提供。

Shell 会将 `f*.sh` 展开为所有匹配该模式的文件名。

> 💡 **记住：Shell 脚本中的变量扩展在脚本执行时才会进行**。即使写在前面，只有在运行时才会报错（如之前讲到的空变量引号问题）。

---

## `while` 循环

由于 Shell 中的值默认都被视为字符串，`for` 循环适合遍历字符串集合。但如果你**事先不知道循环要执行多少次**，就不太适用了。

这时你应该使用 `while` 循环。其语法如下：

```sh
while condition
do
  statements
done
```

### 示例：一个简单的密码检查程序

```sh
#!/bin/sh

echo "Enter password"
read trythis

while [ "$trythis" != "secret" ]; do
  echo "Sorry, try again"
  read trythis
done

exit 0
```

输出示例：

```
Enter password
password
Sorry, try again
secret
$
```

这个方式不够安全，但很好地说明了 `while` 循环的用法。

`do` 与 `done` 之间的语句会持续执行，直到条件不再成立（即输入为 `secret`）。然后继续执行 `done` 之后的语句。

---

## `until` 循环

`until` 的语法如下：

```sh
until condition
do
  statements
done
```

这与 `while` 类似，但条件的逻辑相反：

- `while`：条件为真时继续循环  
- `until`：条件为**假**时继续循环

---

> 💡 **一般来说，如果循环至少应执行一次，使用 `while`；如果循环可能一次都不执行，使用 `until`。**

---

### 示例：使用 `until` 设置登录提醒

```sh
#!/bin/bash

until who | grep "$1" > /dev/null
do
  sleep 60
done

# 提示用户已登录
echo -e '\a'
echo "***** $1 has just logged in *****"

exit 0
```

如果用户已经登录，循环不会执行，`until` 在这种场景比 `while` 更自然。

---

## `case` 语句

`case` 结构比前面介绍的控制结构稍复杂。其语法如下：

```sh
case variable in
  pattern1) statements ;;
  pattern2) statements ;;
  ...
esac
```

这看起来稍显复杂，但 `case` 允许你用简洁的方式检查变量的值是否匹配某些模式，并根据匹配结果执行不同的代码。

比起多层 `if/elif/else` 判断，`case` 更适合处理用户输入。

> ⚠️ **注意每个模式后要用双分号 `;;` 结束**，以标记一个模式匹配的结束，另一个的开始。

---

> 💡 **如果你在 `case` 模式中使用了通配符（如 `*`），请小心！**  
> `case` 会**匹配第一个符合的模式**，即使后面还有更精确的匹配也不会执行。


## 🧪 试试看：案例一 - 用户输入（Case I: User Input）

你可以用 `case` 语句重写输入测试脚本，使其更具选择性，并对非预期输入更宽容：

```sh
#!/bin/sh

echo "Is it morning? Please answer yes or no"
read timeofday

case "$timeofday" in
  yes) echo "Good Morning" ;;
  no ) echo "Good Afternoon" ;;
  y  ) echo "Good Morning" ;;
  n  ) echo "Good Afternoon" ;;
  *  ) echo "Sorry, answer not recognized" ;;
esac

exit 0
```

---

## 它是如何工作的（How It Works）

当 `case` 语句执行时，会将变量 `timeofday` 的值与每个字符串进行比较。一旦匹配成功，就执行对应的命令。

你可以在匹配模式中使用 `*` 通配符来匹配所有输入，并作为默认分支使用。`case` 会从上往下依次匹配，不寻找最精确的匹配，而是**第一个匹配即生效**。

---

## 🧪 试试看：案例二 - 合并模式（Case II: Putting Patterns Together）

将多个模式合并在一起，可以使脚本更整洁：

```sh
#!/bin/sh

echo "Is it morning? Please answer yes or no"
read timeofday

case "$timeofday" in
  yes | y | Yes | YES ) echo "Good Morning" ;;
  n* | N* )             echo "Good Afternoon" ;;
  * )                  echo "Sorry, answer not recognized" ;;
esac

exit 0
```

---

## 它是如何工作的

这个脚本在每个 `case` 项中匹配多个字符串，使其更短更易读。  
例如，用户输入 `never` 会匹配 `n*`，从而错误地输出 “Good Afternoon”。

注意：`*` 通配符不能放在引号中使用。

---

## 🧪 试试看：案例三 - 执行多条语句（Case III: Executing Multiple Statements）

为了让脚本更可重用，可以在输入无效时更改退出码：

```sh
#!/bin/sh

echo "Is it morning? Please answer yes or no"
read timeofday

case "$timeofday" in
  yes | y | Yes | YES )
    echo "Good Morning"
    echo "Up bright and early this morning"
    ;;
  [nN]*)
    echo "Good Afternoon"
    ;;
  *)
    echo "Sorry, answer not recognized"
    echo "Please answer yes or no"
    exit 1
    ;;
esac

exit 0
```

---

## 它是如何工作的

这个版本展示了如何匹配不同模式，并为每个模式执行多条命令。注意：

- `case` 是**匹配第一个符合的项**，不是最佳匹配。
- 默认的 `*` 分支应放在最后。
- `;;` 是必须的，标记每个分支的结束。

> 💡 想让 `case` 更加强大，可以使用如下模式：
> 
> ```sh
> [yY] | [yY][eE][sS])
> ```

---

## 列表（Lists）

有时你想要**将多个条件或命令串联**执行，比如：

```sh
if [ -f this_file ]; then
  if [ -f that_file ]; then
    if [ -f the_other_file ]; then
      echo "All files present, and correct"
    fi
  fi
fi
```

或者想要多个条件中至少一个为真：

```sh
if [ -f this_file ]; then
  foo="True"
elif [ -f that_file ]; then
  foo="True"
elif [ -f the_other_file ]; then
  foo="True"
else
  foo="False"
fi

if [ "$foo" = "True" ]; then
  echo "One of the files exists"
fi
```

这些可以用 `if` 实现，但写法很繁琐。Shell 提供了**命令列表构造**：AND 列表 和 OR 列表。

---

## AND 列表（The AND List）

AND 列表允许你依次执行一系列命令，**前一个命令成功（返回 true）时才会继续下一个命令**。

语法如下：

```sh
statement1 && statement2 && statement3 && ...
```

- 每条语句从左到右执行。
- 如果某条语句返回 false，后续语句将不再执行。
- `&&` 检查前一个命令的返回状态。

AND 列表整体只有在所有命令都成功时才算成功，否则失败。

例如：

```sh
cd mydir && ls && echo "Listed files"
```

如果 `cd mydir` 失败，后面的命令将不会执行。

以下是图片中英文内容的中文翻译：

---

## 🧪 试试看：AND 列表（AND Lists）

下面这个脚本中，使用 `touch file_one` 创建一个文件（如果不存在也会被创建），并使用 `rm` 删除 `file_two`。然后，AND 列表会按顺序检查文件是否存在，并在其间输出一些文本：

```sh
#!/bin/sh

touch file_one
rm -f file_two

if [ -f file_one ] && echo "hello" && [ -f file_two ] && echo " there"
then
  echo "in if"
else
  echo "in else"
fi

exit 0
```

运行结果将是：

```
hello
in else
```

---

### 它是如何工作的（How It Works）

- `touch` 和 `rm` 确保当前目录的文件状态是已知的。
- `[ -f file_one ]` 成立，继续执行 `echo "hello"`。
- `echo` 总是返回 true。
- `[ -f file_two ]` 返回 false（因为文件被删除了）。
- 因此整个 `&&` 列表的结果是 false，进入 `else` 分支。

---

## OR 列表（The OR List）

OR 列表允许你依次执行多条命令，**直到有一条成功为止**。语法如下：

```sh
statement1 || statement2 || statement3 || ...
```

- 从左向右执行。
- 如果某条语句失败，继续执行下一条。
- 一旦有语句成功，后续就停止执行。

OR 列表与 AND 列表类似，但逻辑相反：**AND 需全部成功，OR 只需一条成功**。

---

## 🧪 试试看：OR 列表（OR Lists）

复制上一个例子，并修改关键部分如下：

```sh
#!/bin/sh

rm -f file_one

if [ -f file_one ] || echo "hello" || echo " there"
then
  echo "in if"
else
  echo "in else"
fi

exit 0
```

输出结果为：

```
hello
in if
```

---

### 它是如何工作的（How It Works）

- `[ -f file_one ]` 失败（文件不存在）
- `echo "hello"` 执行并返回 true
- 后续 `echo " there"` 不再执行
- `if` 成立，因为 `echo` 成功

---

这两种构造的执行结果，取决于**最后一个被执行的语句的结果**。

这被称为**短路求值（short circuit evaluation）**，常见于 C 语言中。

---

你也可以组合使用：

```sh
[ -f file_one ] && command for true || command for false
```

---

## 语句块（Statement Blocks）

如果你想在 `AND` 或 `OR` 列表中执行多条语句，可以用花括号 `{}` 包起来，例如：

```sh
get_confirm && {
  grep -v "$cdcatnum" $tracks_file > $temp_file
  cat $temp_file > $tracks_file
  echo
  add_record_tracks
}
```

---

## 函数（Functions）

Shell 中可以定义函数。如果你的脚本较大，使用函数有助于结构化你的代码。

> 💡 **提示：**  
> 与其编写多个小脚本，不如将逻辑组织成函数。调用外部脚本比调用函数慢，也更难传递结果。

---

### 定义函数语法如下：

```sh
function_name () {
  statements
}
```

---

## 🧪 试试看：一个简单函数（A Simple Function）

```sh
#!/bin/sh

foo() {
  echo "Function foo is executing"
}

echo "script starting"
foo
echo "script ended"

exit 0
```

运行结果：

```
script starting
Function foo is executing
script ended
```

## 🧠 它是如何工作的（How It Works）

这个脚本从顶部开始执行。因此，当它遇到 `foo()` 构造时，它知道这是在定义一个名为 `foo` 的函数，并将其存储起来。之后执行 `foo` 时，Shell 会跳转去执行这个函数，函数执行完毕后，脚本从调用 `foo` 的那一行继续执行下去。

在 Shell 中必须**先定义函数再调用**，类似 Pascal 语言中的函数调用方式。Shell 不支持前向声明（forward declarations），但这也没关系，因为脚本从上到下执行，所以只要在第一次调用函数前定义好即可。

调用函数时，脚本中的位置参数（如 `$*`, `$@`, `$1`, `$2` 等）会被函数参数所替换，函数结束后这些值会恢复。

> ⚠️ 有些旧版 Shell 不会恢复位置参数。为了可移植性，尽量不要依赖这种行为！

---

你可以使用 `return` 命令让函数返回数值。若想让函数返回字符串，通常将字符串保存在变量中，或者用 `echo` 打印再捕获：

```sh
foo () { echo JAY; }
result="$(foo)"
```

---

### 局部变量（local）

你可以使用 `local` 关键字在函数中声明局部变量，仅在函数内部可见。若局部变量与全局变量同名，它只会在函数内覆盖全局变量。

例如：

```sh
#!/bin/sh

sample_text="global variable"

foo() {
  local sample_text="local variable"
  echo "Function foo is executing"
  echo $sample_text
}

echo "script starting"
foo
echo "script ended"
echo $sample_text
exit 0
```

输出：

```
script starting
Function foo is executing
local variable
script ended
global variable
```

---

## 🧪 试试看：返回一个值（Returning a Value）

脚本 `my_name` 展示如何将参数传递给函数，并返回 true 或 false：

1. 定义函数：

```sh
#!/bin/sh

yes_or_no() {
  echo "Is your name $* ?"
  while true
  do
    echo -n "Enter yes or no: "
    read x
    case "$x" in
      y | yes ) return 0 ;;
      n | no  ) return 1 ;;
      *       ) echo "Answer yes or no" ;;
    esac
  done
}
```

2. 主程序部分：

```sh
echo "Original parameters are $*"

if yes_or_no "$1"
then
  echo "Hi $1, nice name"
else
  echo "Never mind"
fi
exit 0
```

运行示例：

```
$ ./my_name Rick Neil
Original parameters are Rick Neil
Is your name Rick ?
Enter yes or no: yes
Hi Rick, nice name
```

---

## 🧠 它是如何工作的

函数 `yes_or_no` 接收参数 `$1` 并打印提问信息。根据用户输入的 yes 或 no，返回 0（true）或 1（false），`if` 根据返回值执行不同分支。

---

## Commands（命令）

Shell 脚本中可以执行两种类型的命令：

- **外部命令（external commands）**：可在命令行直接运行
- **内建命令（built-in commands）**：Shell 内部实现，效率更高

大多数内建命令也有对应的外部版本，符合 POSIX 规范。

---

## break 命令

`break` 用于跳出当前 `for`、`while` 或 `until` 循环。默认跳出一层循环。

```sh
#!/bin/sh

rm -rf fred*
echo > fred1
echo > fred2
mkdir fred3
echo > fred4

for file in fred*
do
  if [ -d "$file" ]; then
    break
  fi
done

echo first directory starting fred was $file
rm -rf fred*
exit 0
```

---

## `:` 命令（冒号命令）

`:` 是空命令，什么也不做。相当于 `true`，但比 `true` 执行更快。

常用场景：

- 无限循环：`while :`
- 条件赋值：`: ${var:=value}`

> ⚠️ 有些旧脚本中使用 `:` 开头表示注释，但推荐使用 `#`，因为更高效。

示例：

```sh
#!/bin/sh

rm -f fred
if [ -f fred ]; then
  :
else
  echo file fred did not exist
fi

exit 0
```

---

## continue 命令

类似 C 语言中的 `continue`，跳过当前循环并进入下一次迭代：

```sh
#!/bin/sh

rm -rf fred*
echo > fred1
echo > fred2
mkdir fred3
echo > fred4

for file in fred*
do
  if [ -d "$file" ]; then
    echo "skipping directory $file"
    continue
  fi
  echo file is $file
done

rm -rf fred*
exit 0
```

也可以带数字参数跳出多层循环（不推荐）：

```sh
for x in 1 2 3
do
  echo before $x
  continue 1
  echo after $x
done
```

输出：

```
before 1
before 2
before 3
```

---

## `.` 命令（点命令）

点命令在**当前 Shell 中执行脚本**：

```sh
. ./shell_script
```

与普通方式（`./script`）不同，它不会创建新环境，因此可以修改当前 Shell 的变量。

这与 C/C++ 中的 `#include` 类似，但它不会复制内容，只是在当前环境中执行。

适用于：

- 设置环境变量
- 引入函数或变量定义
- 多项目共享配置文件


### 🧪 试试看：点号命令（The Dot Command）

以下示例在命令行中使用点号命令，但你也可以在脚本中使用它：

**1.** 假设你有两个文件，分别包含两个不同开发环境的环境设置。要为旧的 classic 命令设置环境，使用 classic_set，可以这样写：

```sh
#!/bin/sh

version=classic  
PATH=/usr/local/old_bin:/usr/bin:/bin:.  
PS1="classic> "
```

**2.** 对于新命令，使用 latest_set：

```sh
#!/bin/sh

version=latest  
PATH=/usr/local/new_bin:/usr/bin:/bin:.  
PS1=" latest version> "
```

你可以通过与点号命令结合使用这些脚本来设置环境，如下所示的会话：

```sh
$ . ./classic_set  
classic> echo $version  
classic  
classic> . ./latest_set  
latest version> echo $version  
latest  
latest version>
```

---

### 它是如何工作的（How It Works）

这些脚本是使用点号命令执行的，因此每个脚本都是在当前 shell 中执行的。这使得脚本能够更改当前 shell 中的环境设置，并且这些更改在脚本执行完成后仍然保留。

---

## echo

尽管 X/Open 强烈建议在现代 shell 中使用 printf 命令，我们仍然习惯使用 echo 命令输出字符串，并在末尾附加一个换行符。

一个常见问题是如何抑制换行符。不幸的是，不同版本的 UNIX 对此的实现各不相同。在 Linux 中的常用方法是使用：

```sh
echo -n "string to output"
```

但你常常会看到如下写法：

```sh
echo -e "string to output\c"
```

第二个选项 echo -e 启用对反斜杠转义字符的解释，例如 \c 用于抑制换行，\t 用于输出制表符，\n 用于输出回车换行。在旧版本 bash 中，这通常是默认启用的，但在较新版本中，常常默认不解释反斜杠转义字符。请参阅你所用发行版的手册了解详细行为。

> **如果你需要一种可移植的方法来去除末尾的换行符，可以使用外部命令 tr 来删除它，但它会稍慢一些。如果你需要在 UNIX 系统中实现可移植性，最好坚持使用 printf。如果你的脚本只需要在 Linux 和 bash 上运行，echo -n 是可以的，不过你可能需要以 `#!/bin/bash` 开头来确保启用 bash 风格的行为。**

---

## eval

eval 命令允许你对参数进行求值。它是 shell 的内建命令，通常不会作为独立命令存在。它可能最适合用一个简短的例子来演示（改编自 X/Open 规范本身）：

```sh
foo=10  
x=foo  
y='$'$x  
echo $y
```

这将输出：

```
$foo
```

然而：

```sh
foo=10  
x=foo  
eval y='$'$x  
echo $y
```

将输出：

```
10
```

因此，eval 有点像额外的 $：它为你提供一个变量的值的值。

eval 命令非常有用，它允许你生成代码并实时运行。它会使脚本调试更复杂，但也使你可以做一些通常非常困难甚至不可能做的事情。

---

## exec

exec 命令有两个不同的用途。它的典型用途是用另一个程序替换当前 shell。例如：

```sh
exec wall "Thanks for all the fish"
```

在脚本中，这将用 wall 命令替换当前 shell。exec 之后的脚本行将不会被处理，因为执行脚本的 shell 已经不存在了。

exec 的第二个用途是修改当前文件描述符：

```sh
exec 3< afile
```

这会打开文件描述符 3，从文件 afile 中读取。这种用途很少见。

---

## exit n

exit 命令使脚本以退出码 n 退出。如果你在交互式 shell 中使用它，它将把你登出。如果你让脚本在没有指定退出码的情况下退出，那么脚本中最后执行的命令的状态就会作为返回值。因此，最好始终提供一个退出码。

在 shell 脚本中，退出码 0 表示成功，1 到 125 的数字是脚本可用的错误码。其余值具有保留含义，如下表所示：

| Exit Code | Description                   |
|-----------|-------------------------------|
| 126       | The file was not executable   |
| 127       | A command was not found       |
| 128 及以上| A signal occurred             |

使用 0 表示成功对许多 C 或 C++ 程序员来说可能有些不习惯。脚本的主要优势在于它们允许你使用 125 个用户定义的错误码，而无需使用全局错误变量。

以下是一个返回成功的简单示例，如果当前目录中存在名为 .profile 的文件：

```sh
#!/bin/sh

if [ -f .profile ]; then  
  exit 0  
fi  

exit 1
```

如果你喜欢简洁的写法，或者至少不介意极简风格，你可以将该脚本重写为一行，使用前面介绍的 AND 和 OR 列表：

```sh
[ -f .profile ] && exit 0 || exit 1
```

---

## export

export 命令使作为其参数的变量在子 shell 中可用。默认情况下，在 shell 中创建的变量在从该 shell 启动的后续（子）shell 中是不可用的。export 命令根据其参数创建一个环境变量，该变量可以被其他脚本和程序访问，这些脚本和程序是从当前程序中调用的。更准确地说，被 export 的变量在 shell 中形成环境变量，并在从该 shell 派生的任何子进程中可见。

这可以通过两个脚本 export1 和 export2 的例子来最好地说明。

### 🧪 试试看：导出变量（Exporting Variables）

**1.** 首先，列出 `export2`：

```sh
#!/bin/sh

echo "${foo}"  
echo "${bar}"
```

**2.** 现在是 `export1`。在这个脚本的末尾调用 `export2`：

```sh
#!/bin/sh

foo="The first meta-syntactic variable"  
export bar="The second meta-syntactic variable"

export2
```

如果你运行它们，将会得到如下输出：

```sh
$ ./export1  
The second meta-syntactic variable  
$
```

---

### 它是如何工作的（How It Works）

`export2` 脚本只是简单地回显两个变量的值。`export1` 脚本设置了两个变量，但只将 `bar` 标记为已导出，因此当它随后调用 `export1` 时，`foo` 的值已丢失，但 `bar` 的值已被导出到第二个脚本。空白行的产生是因为 `$foo` 的值为空，而回显一个空变量会得到一个换行符。

一旦变量从某个 shell 中导出，它就会被导出到从该 shell 调用的任何脚本中，并且会一直这样传递下去。如果脚本 `export2` 调用了另一个脚本，它也将拥有可用的 `bar` 变量值。

> **命令 `set -a` 或 `set -allexport` 会将此后的所有变量都导出。**

---

## expr

`expr` 命令将其参数作为表达式进行求值。它最常见的用途是以下形式的简单算术：

```sh
x=`expr $x + 1`
```

你也可以使用 `$()` 语法而不是反引号来写它，如下所示：

```sh
x=$(expr $x + 1)
```

`expr` 命令功能强大，可以执行许多表达式求值。主要的表达式如下表所示：

| 表达式求值       | 描述                              |
|------------------|-----------------------------------|
| expr1 \| expr2   | 如果 expr1 非零，则为 expr1，否则为 expr2 |
| expr1 & expr2    | 如果任一表达式为零，则结果为零，否则为 expr1 |
| expr1 = expr2    | 相等                              |
| expr1 > expr2    | 大于                              |
| expr1 >= expr2   | 大于或等于                        |
| expr1 < expr2    | 小于                              |
| expr1 <= expr2   | 小于或等于                        |
| expr1 != expr2   | 不等                              |
| expr1 + expr2    | 加法                              |
| expr1 - expr2    | 减法                              |
| expr1 * expr2    | 乘法                              |
| expr1 / expr2    | 整数除法                          |
| expr1 % expr2    | 取模                              |

在较新的脚本中，`expr` 通常被更高效的 `$(...)` 语法所取代，本章稍后会介绍。

---

## printf

`printf` 命令只在较新的 shell 中可用。X/Open 建议它应被优先于 `echo` 用于生成格式化输出，尽管似乎很少有人遵循这一建议。

语法如下：

```sh
printf "格式字符串" 参数1 参数2 ...
```

格式字符串非常类似于 C 或 C++ 中使用的格式字符串，但有一些限制。主要是 shell 中所有算术运算都是整数运算，因此不支持浮点数。格式字符串由任意组合的文字字符、转义序列和转换说明符组成。格式字符串中的所有字符，除了 `%` 和 `\`，在输出中按原样显示。

支持以下转义序列：

| 转义序列 | 描述                              |
|----------|-----------------------------------|
| \"       | 双引号                            |
| \\       | 反斜杠                            |
| \a       | 响铃（发出提示音或哔声）          |
| \b       | 退格符                            |
| \c       | 抑制后续输出                      |
| \f       | 走纸换页                          |
| \n       | 换行符                            |
| \r       | 回车                              |
| \t       | 制表符                            |
| \v       | 垂直制表符                        |
| \ooo     | 八进制值为 ooo 的单个字符         |
| \xHH     | 十六进制值为 HH 的单个字符        |

转换说明符较复杂，这里我们只列出常用的用法。更多细节可查看 bash 在线手册或 `printf` 的第 1 节在线手册页（`man 1 printf`）。转换说明符由一个 `%` 字符和一个转换字符组成。主要的转换说明符如下表所示：

| 转换说明符 | 描述                  |
|------------|-----------------------|
| d          | 输出十进制数          |
| c          | 输出单个字符          |
| s          | 输出字符串            |
| %          | 输出 `%` 字符         |

格式字符串用于解释其后的参数并输出结果，如下例所示：

```sh
$ printf "%s\n" hello  
hello  
$ printf "%s %d\t%s" "Hi There" 15 people  
Hi There 15    people
```

请注意你必须使用 `" "` 来保护 `"Hi There"` 字符串，并将其作为一个参数。

---

## return

`return` 命令使函数返回，如我们在前面查看函数时所提到的。`return` 接受一个单个数值参数，该参数对调用该函数的脚本可用。如果未指定参数，则 `return` 默认为上一个命令的退出码。

---

## set

`set` 命令为 shell 设置参数变量。它在处理以空格分隔字段为输出的命令时非常有用。

假设你想在一个 shell 脚本中使用当前月份的名称。系统提供了 `date` 命令，其中包含月份作为字符串，但你需要将其与其他字段分离。你可以通过组合 `set` 命令和 `$(...)` 构造来执行 `date` 命令并返回结果（稍后会更详细地介绍）。`date` 命令输出的第二个参数是月份：

```sh
#!/bin/sh

echo the date is $(date)  
set $(date)  
echo The month is $2

exit 0
```

该程序将参数列表设置为 `date` 命令的输出，然后使用位置参数 `$2` 获取月份。

请注意我们使用 `date` 命令只是为了展示如何提取位置参数。由于 `date` 命令受语言区域影响，实际上你会使用 `date +%B` 来提取月份名称。`date` 命令有许多其他格式选项；请参阅手册页了解更多细节。

你还可以使用 `set` 命令通过传递参数来控制 shell 的执行方式。最常见的形式是 `set -x`，它使脚本显示其当前执行的命令。我们将在调试部分更深入地讨论 `set` 及其选项。

---

## shift

`shift` 命令将所有参数变量向下移动一个位置，使得 `$2` 变成 `$1`，`$3` 变成 `$2`，以此类推。`$1` 的之前值被丢弃，而 `$0` 保持不变。如果在调用 `shift` 时指定了数值参数，参数将移动相应数量的位置。其他变量如 `$*`、`$@` 和 `$#` 也会根据新的参数排列进行修改。

`shift` 通常用于扫描传递到脚本中的参数。如果你的脚本需要 10 个或更多参数，你将需要使用 `shift` 来访问第十个及以后的参数。

例如，你可以像下面这样扫描所有位置参数：

```sh
#!/bin/sh

while [ "$1" != "" ]; do  
  echo "$1"  
  shift  
done

exit 0
```

---

## trap

`trap` 命令用于指定在接收到信号时要采取的操作，你将在本书后面更详细地了解它。一个常见用法是在脚本被中断时清理脚本。历史上，shell 总是使用信号编号，但新脚本应该使用来自 `#include` 文件 `signal.h` 的名称，去掉 `SIG` 前缀。要查看信号编号和关联名称，你只需在命令行中键入：

```sh
trap -l
```

> **对于不熟悉信号的人来说，信号是异步发送到程序的事件。默认情况下，它们通常会导致程序终止。**

`trap` 命令的语法是传递要采取的操作，后跟要捕获的信号名称（或多个名称）：

```sh
trap command signal
```

请记住脚本通常是从上到下解释执行的，因此你必须在希望保护的脚本部分之前指定 `trap` 命令。

要将一个 trap 条件重置为默认，只需将命令指定为 `-`。要忽略一个信号，将命令设置为空字符串 `''`。不带参数的 `trap` 命令会打印当前的 trap 列表和动作。

下表列出了一些 X/Open 标准涵盖的重要信号（括号中为常规信号编号）。更多细节可在在线手册的第 7 节中找到（`man 7 signal`）：

| 信号     | 描述                                                        |
|----------|-------------------------------------------------------------|
| HUP (1)  | 挂起；通常在终端掉线或用户注销时发送                      |
| INT (2)  | 中断；通常通过按 Ctrl+C 发送                                |
| QUIT (3) | 退出；通常通过按 Ctrl+\ 发送                                |
| ABRT (6) | 异常中止；通常因严重执行错误而发送                          |
| ALRM (14)| 警报；通常用于处理超时                                      |
| TERM (15)| 终止；通常由系统在关闭时发送                                |


### 🧪 试试看：捕捉信号（Trapping Signals）

以下脚本演示了一些简单的信号处理：

```sh
#!/bin/sh

trap 'rm -f /tmp/my_tmp_file_$$' INT
echo creating file /tmp/my_tmp_file_$$
date > /tmp/my_tmp_file_$$

echo "press interrupt (CTRL-C) to interrupt ...."
while [ -f /tmp/my_tmp_file_$$ ]; do
  echo File exists
  sleep 1
done
echo The file no longer exists

trap INT
echo creating file /tmp/my_tmp_file_$$
date > /tmp/my_tmp_file_$$

echo "press interrupt (control-C) to interrupt ...."
while [ -f /tmp/my_tmp_file_$$ ]; do
  echo File exists
  sleep 1
done

echo we never get here
exit 0
```

如果你运行这个脚本，在每个循环中按住 Ctrl 然后按 C（或你的中断组合键是啥），你将得到如下输出：

```
creating file /tmp/my_tmp_file_141
press interrupt (CTRL-C) to interrupt ....
File exists
File exists
File exists
File exists
The file no longer exists
creating file /tmp/my_tmp_file_141
press interrupt (CTRL-C) to interrupt ....
File exists
File exists
File exists
File exists
```

---

### 它是如何工作的（How It Works）

这个脚本使用 trap 命令来安排当收到 INT（中断）信号时执行命令 `rm -f /tmp/my_tmp_file_$$`。脚本随后进入一个 while 循环，只要文件存在就持续运行。当用户按下 Ctrl+C，语句 `rm -f /tmp/my_tmp_file_$$` 被执行，然后 while 循环继续。由于文件现在已被删除，第一个 while 循环正常终止。

脚本随后再次使用 trap 命令，这次是指定在发生 INT 信号时不执行任何命令。它随后重新创建该文件并进入第二个 while 循环。这次当用户按下 Ctrl+C 时，没有语句被配置来执行，因此发生默认行为，即立即终止脚本。因为脚本立即终止，最终的 echo 和 exit 语句永远不会被执行。

---

## unset

unset 命令从环境中移除变量或函数。它无法对 shell 本身定义的只读变量这样做，例如 IFS。它不常被使用。

以下脚本会输出一次 Hello World 和一次换行：

```sh
#!/bin/sh

foo="Hello World"
echo $foo

unset foo
echo $foo
```

> **写 `foo=` 会有非常相似但不完全相同的效果。写 `foo=` 的作用是将 foo 设为 null，但 foo 仍然存在。使用 `unset foo` 的效果是将变量 foo 从环境中移除。**

---

## 两个更有用的命令和正则表达式（Two More Useful Commands and Regular Expressions）

在你了解如何把这些新的 shell 编程知识应用起来之前，我们来看几个其他非常有用的命令，虽然它们不是 shell 的一部分，但在编写 shell 脚本时非常有用。在此过程中我们还将了解正则表达式，一种在 Linux 及其相关程序中大量出现的模式匹配功能。

---

## find 命令（The find Command）

你要学习的第一个命令是 find。这个命令用于查找文件，非常有用，但对 Linux 新手来说可能有点难用，部分原因是它需要选项、测试和操作类型参数，而一个参数的结果可能会影响随后的参数处理。

在深入了解选项、测试和参数之前，我们来看一个非常简单的例子，用于在你的本地机器上查找文件 test。以 root 身份执行以确保你有权限搜索整个系统：

```sh
# find / -name test -print
/usr/bin/test
#
```

根据你的安装情况，你可能会发现其他名为 test 的文件。如你所料，这意味着“从 / 开始搜索名为 test 的文件，然后打印出文件名。”是不是很简单？当然。

然而，它在我们的机器上运行花了一些时间，而且我们网络上的 Windows 机器上的磁盘也发出了响声。这是因为我们的 Linux 机器通过 SAMBA 挂载了 Windows 机器的文件系统的一部分。看起来它也被搜索了，尽管我们知道要找的文件会在 Linux 机器上。

这就用到了第一个选项。如果你指定 `-mount`，你可以告诉 find 不要搜索挂载的目录：

```sh
# find / -mount -name test -print
/usr/bin/test
#
```

我们仍然在我们的机器上找到了文件，但这次更快了，而且没有搜索其他挂载的文件系统。

---

find 命令的完整语法如下：

```
find [path] [options] [tests] [actions]
```

path 部分很简单：你可以使用绝对路径，例如 /bin，或相对路径，例如 ..。如果需要，你还可以指定多个路径——例如，`find /var /home`。

有几个选项；主要的如下表所示：

| Option             | Meaning                                                   |
|--------------------|-----------------------------------------------------------|
| -depth             | 在查看目录本身之前搜索其内容                             |
| -follow            | 跟随符号链接                                               |
| -maxdepths N       | 搜索目录的深度最多为 N 层                                 |
| -mount (或 -xdev)  | 不要搜索其他文件系统中的目录                             |

---

现在是测试。可以给 find 提供大量测试，每个测试返回 true 或 false。当 find 工作时，它会依次检查它找到的每个文件，并按顺序应用每个测试。如果测试返回 false，find 就停止对当前文件进行处理并继续下一个；如果测试返回 true，find 就对当前文件执行下一个测试或操作。下表列出的测试只是最常见的；请参考手册页了解 find 可以使用的所有测试：

| Test             | Meaning                                             |
|------------------|-----------------------------------------------------|
| -atime N         | 文件在 N 天前最后被访问                             |
| -mtime N         | 文件在 N 天前最后被修改                             |
| -name pattern    | 文件名（不含路径）匹配提供的模式，模式必须加引号   |
| -newer otherfile | 文件比 otherfile 更新                               |
| -type C          | 文件是类型为 C 的文件，常见为 "d" 或 "f"            |
| -user username   | 文件归指定用户名的用户所有                         |

---

你也可以使用运算符组合测试。大多数有两种形式：短形式和长形式，如下表所示：

| Operator, Short Form | Operator, Long Form | Meaning                  |
|----------------------|---------------------|--------------------------|
| !                    | -not                | 取反测试结果             |
| -a                   | -and                | 两个测试都必须为 true    |
| -o                   | -or                 | 任意一个测试为 true 即可 |

你可以使用括号强制测试和运算符的优先级。由于括号对 shell 有特殊含义，你还必须使用反斜杠转义括号。此外，如果你使用模式匹配文件名，必须加引号，以防 shell 展开通配符而不是将其直接传递给 find。例如，如果你想写一个测试“比文件 X 更新或名称以下划线开头”，你可以写成：

```
\( -newer X -o -name "_*" \)
```

我们将在下一个 “How it Works” 部分后展示一个示例。

---

### 🧪 试试看：使用 find 和测试（Using find with Tests）

尝试在当前目录中查找比文件 while2 更新的文件：

```sh
$ find . -newer while2 -print
.
./elif3
./words.txt
./words2.txt
./_trap
$
```

---

这看起来不错，除了你还找到了当前目录，这是你不想要的。你只想找常规文件，于是添加了一个额外的测试 `-type f`：

```sh
$ find . -newer while2 -type f -print
./elif3
./words.txt
./words2.txt
./_trap
$
```

---

### 它是如何工作的（How It Works）

它是如何工作的？你指定 find 应该在当前目录（.）中搜索，比文件 while2 更新的文件（`-newer while2`），并且如果该测试通过，则还测试该文件是否是常规文件（`-type f`）。最后，你使用了你已经学过的操作 `-print`，只是为了确认找到了哪些文件。

现在查找文件名以下划线开头或比文件 while2 更新的文件，但两者都必须是常规文件。这将展示如何使用括号组合测试：

```sh
$ find . \( -name "_*" -or -newer while2 \) -type f -print
./elif3
./words.txt
./words2.txt
./_break
./_if
./_set
./_shift
./_trap
./_unset
./_until
$
```

这不难，对吧？你必须转义括号，使其不被 shell 处理，并为 * 加引号以确保它被直接传递给 find。

现在你能可靠地搜索文件了，我们来看当找到匹配文件时可以执行的操作。这只是最常见操作的列表；完整的请参考手册页。

| Action         | Meaning                                                                 |
|----------------|-------------------------------------------------------------------------|
| -exec command  | 执行一个命令。最常用操作之一。必须以 `\;` 结尾                          |
| -ok command    | 类似 -exec，但会在执行前提示用户确认。也必须以 `\;` 结尾               |
| -print         | 打印文件名                                                              |
| -ls            | 使用 `ls -dils` 命令列出当前文件信息                                    |


`-exec` 和 `-ok` 命令将其后续参数作为命令行的一部分，直到以 `\;` 结尾为止。实际上，`-exec` 和 `-ok` 正在执行一个嵌入命令，因此嵌入命令必须以转义分号结尾，以便 `find` 命令知道何时应该停止查找命令行选项，转而执行该命令。魔法字符串 `{}` 是一个特殊类型的参数，用于 `-exec` 或 `-ok` 命令，它会被替换为当前文件的完整路径。

这个解释可能不太容易理解，但一个例子会让事情更清楚。来看一个使用安全命令 `ls` 的简单例子：

```sh
$ find . -newer while2 -type f -exec ls -l {} \;
-rwxr-xr-x  1 rick  rick   275 Feb  8 17:07 ./elif3
-rwxr-xr-x  1 rick  rick   336 Feb  8 16:52 ./words.txt
-rwxr-xr-x  1 rick  rick  1274 Feb  8 16:52 ./words2.txt
-rwxr-xr-x  1 rick  rick   504 Feb  8 18:43 ./_trap
$
```

如你所见，`find` 命令非常有用；只需要一点练习就能很好地使用它。不过，这种练习是值得的，因此请尝试 `find` 命令。

---

## grep 命令

第二个非常有用的命令是 `grep`，一个不寻常的名称，代表通用正则表达式搜索器。你使用 `find` 来搜索系统中的文件，但你使用 `grep` 来搜索字符串。实际上，在使用 `find` 时，将 `grep` 作为 `-exec` 后传入的命令是很常见的。

`grep` 命令接受选项、要匹配的模式和要搜索的文件，格式如下：

```sh
grep [options] PATTERN [FILES]
```

如果未提供文件名，它将搜索标准输入。

让我们从查看 `grep` 的主要选项开始。我们只列出主要选项；完整列表请参阅手册页。

| Option | Meaning                                                                 |
|--------|-------------------------------------------------------------------------|
| -c     | 不打印匹配行，而是打印匹配行的数量                                     |
| -E     | 启用扩展表达式                                                         |
| -h     | 不在每行输出前加上文件名                                               |
| -i     | 忽略大小写                                                              |
| -l     | 仅列出包含匹配行的文件名，不输出匹配行本身                             |
| -v     | 反转匹配，选择不匹配模式的行                                           |

---

### 🧪 试试看：基本 grep 用法（Basic grep Usage）

来看 `grep` 的一些简单匹配示例：

```sh
$ grep in words.txt
When shall we three meet again.  In thunder, lightning, or in rain?
I come, Graymalkin!
$ grep -c in words.txt words2.txt
words.txt:2
words2.txt:14
$ grep -c -v in words.txt words2.txt
words.txt:9
words2.txt:16
$
```

---

### 它是如何工作的（How It Works）

第一个例子没有使用任何选项；它只是搜索文件 `words.txt` 中的字符串 “in”，并打印出所有匹配的行。由于你只搜索一个文件，因此不会显示文件名。

第二个例子统计两个不同文件中匹配行的数量。在这种情况下，文件名会被打印出来。

最后，使用 `-v` 选项反转搜索，并统计两个文件中不匹配的行数。

---

## 正则表达式（Regular Expressions）

如你所见，`grep` 的基本用法非常容易掌握。现在是时候看看正则表达式的基础知识了，它可以让你进行更复杂的匹配。如本章前面所述，正则表达式在 Linux 和许多其他开源语言中都被使用。你可以在 `vi` 编辑器中使用它们，也可以在编写 Perl 脚本时使用，基本原理在任何地方都通用。

在使用正则表达式时，某些字符会以特殊方式处理。以下是最常用的一些：

| Character | Meaning                                     |
|-----------|---------------------------------------------|
| ^         | 匹配行的开头                                |
| $         | 匹配行的结尾                                |
| .         | 匹配任意单个字符                            |
| [ ]       | 方括号中包含一个字符范围，表示可以匹配其中任意字符，例如 a-e，或使用 `^` 开头表示取反 |

---

如果你想将这些字符作为“普通”字符使用，可以在它们前面加反斜杠。例如，如果你想查找一个字面意义上的 `$` 字符，只需使用 `\$`。

还有一些可以在方括号中使用的有用特殊匹配模式，如下表所示：

| Match Pattern   | Meaning                                     |
|------------------|---------------------------------------------|
| [:alnum:]       | 字母数字字符                                |
| [:alpha:]       | 字母                                        |
| [:ascii:]       | ASCII 字符                                 |
| [:blank:]       | 空格或制表符                                |
| [:cntrl:]       | ASCII 控制字符                              |
| [:digit:]       | 数字                                        |
| [:graph:]       | 可见字符（非空白）                          |
| [:lower:]       | 小写字母                                    |
| [:print:]       | 可打印字符                                  |
| [:punct:]       | 标点符号                                    |
| [:space:]       | 空白字符，包括垂直制表符                    |
| [:upper:]       | 大写字母                                    |
| [:xdigit:]      | 十六进制数字                                |

此外，如果还指定了 `-E` 选项以启用扩展匹配，还可以使用其他字符控制匹配的完成方式（如下表）。在 grep 中也必须使用 `\` 来转义这些字符。

| Option   | Meaning                                             |
|----------|-----------------------------------------------------|
| ?        | 匹配可选但最多一次                                 |
| *        | 匹配零次或多次                                     |
| +        | 匹配一次或多次                                     |
| {n}      | 必须匹配 n 次                                      |
| {n,}     | 必须匹配 n 次或更多次                              |
| {n,m}    | 必须匹配 n 到 m 次之间（包括 n 和 m）              |

---

这看起来有点复杂，但如果你分阶段学习，你会发现它其实没有最初看起来那么复杂。掌握正则表达式的最好方法就是多试几次：

**1.** 先找以字母 e 结尾的行。你可能猜到要使用特殊字符 `$`：

```sh
$ grep e$ words2.txt
Art thou not, fatal vision, sensible
I see thee yet, in form as palpable
Nature seems dead, and wicked dreams abuse
$
```

如你所见，这会找到以字母 e 结尾的行。

**2.** 现在假设你要找以字母 a 结尾的单词。为此，你需要使用方括号中的特殊匹配字符。在这种情况下，你使用 `[[:blank:]]`，它测试空格或制表符：

```sh
$ grep a[[:blank:]] words2.txt
Is this a dagger which I see before me,
A dagger of the mind, a false creation,
Moves like a ghost. Thou sure and firm-set earth,
$
```

**3.** 现在找出以 Th 开头的三个字母的单词。在这种情况下，你需要使用 `[[:space:]]` 来界定单词结尾，并用 `.` 匹配一个额外字符：

```sh
$ grep Th.[[:space:]] words2.txt
The handle toward my hand? Come, let me clutch thee.
The curtain'd sleep; witchcraft celebrates
Thy very stones prate of my whereabout,
$
```

**4.** 最后，使用扩展 grep 模式来搜索正好是 10 个字符长的小写单词。通过指定字符范围从 a 到 z，并重复 10 次来完成：

```sh
$ grep -E [a-z]\{10\} words2.txt
Proceeding from the heat-oppressed brain?
And such an instrument I was to use.
The curtain'd sleep; witchcraft celebrates
Thy very stones prate of my whereabout,
$
```

这只是触及了正则表达式最重要部分的一小部分。就像 Linux 中的大多数内容一样，有大量文档可以帮助你深入了解，但学习正则表达式的最佳方式就是多尝试。

---

## 命令执行（Command Execution）

当你编写脚本时，通常需要捕获命令执行的结果，以便在脚本中使用；也就是说，你想执行一个命令并将其输出放入变量中。

你可以使用前面在 set 命令示例中介绍的 `$(command)` 语法来完成此操作。还有一种旧语法，使用反引号 `` `command` ``，仍然很常见。

> **注意：在旧形式中，使用的是反引号（`），不是我们在早些时候引用变量时使用的单引号（'），用于防止变量展开。仅当你需要非常高的可移植性时，才使用这种形式编写 shell 脚本。**

所有新脚本都应使用 `$(...)` 形式，它是为了避免一些复杂规则而引入的，这些规则涵盖了 `$`、`\` 和 `` ` `` 在反引号命令中的使用。如果在 `` `...` `` 中使用了反引号，它必须用反斜杠 `\` 转义。这些相对隐晦的字符常常让程序员困惑，有时甚至有经验的 shell 程序员也不得不反复实验才能正确引用反引号命令。

`$(command)` 的结果仅是该命令的输出结果。请注意，这不是命令的返回状态，而是字符串输出，如下所示：

```sh
#!/bin/sh

echo The current directory is $PWD
echo The current users are $(who)

exit 0
```

由于当前目录是一个 shell 环境变量，所以第一行不需要使用这种命令执行结构。然而，`who` 的结果确实需要这个结构才能在脚本中可用。

如果你想将结果放入变量中，只需像这样分配即可：

```sh
whoisthere=$(who)
echo $whoisthere
```

将命令结果放入脚本变量的能力非常强大，因为它使你可以轻松使用现有命令生成脚本并捕获其输出。如果你发现自己试图将标准输出的命令结果转换为一组参数传递给另一个程序，你可能会发现命令 `xargs` 可以为你完成此操作。请查阅手册了解详细信息。

有时会出现一个问题，即你要调用的命令在你想要的文本之前输出了一些空格，或输出了比你需要的更多内容。在这种情况下，你可以使用前面介绍的 `set` 命令，如下所示。


## 算术扩展（Arithmetic Expansion）

我们之前已经使用了 `expr` 命令，它允许处理一些简单的算术命令，但它的执行相对较慢，因为需要启动一个新的 shell 来处理 `expr` 命令。

一个更新且更优的方法是使用 `$((...))` 扩展。通过将你希望计算的表达式放入 `$((...))` 中，你可以更高效地执行简单的算术运算：

```sh
#!/bin/sh

x=0
while [ "$x" -ne 10 ]; do
  echo $x
  x=$(($x+1))
done

exit 0
```

> **注意：这与 `x=$(...)` 命令形式略有不同。双括号形式用于算术替换；而单括号形式（先前介绍的）用于执行命令并获取其输出。**

---

## 参数扩展（Parameter Expansion）

你已经看过了参数赋值和扩展的最简单形式：

```sh
foo=fred
echo $foo
```

当你想在变量末尾附加额外字符时，可能会出现问题。假设你想写一个简短的脚本来处理名为 `1_tmp` 和 `2_tmp` 的文件。你可能会这样做：

```sh
#!/bin/sh

for i in 1 2
do
  my_secret_process $i_tmp
done
```

但在每次循环中，你将得到如下输出：

```
my_secret_process: too few arguments
```

出了什么问题？

问题是 shell 试图替换变量 `$i_tmp` 的值，而这个变量并不存在。Shell 不会将其视为错误；它只是替换成空字符串，因此没有参数传递给 `my_secret_process`。

为了保护变量中 `$i` 的扩展部分，你需要使用花括号将 `i` 括起来，如下所示：

```sh
#!/bin/sh

for i in 1 2
do
  my_secret_process ${i}_tmp
done
```

在每次循环中，`i` 的值将被替换为 `${i}`，从而得到实际的文件名。你将参数的值插入到字符串中。

你可以在 shell 中执行许多参数替换操作。这些替换通常为许多参数处理问题提供了优雅的解决方案。以下是一些常见替换方式：

| 参数扩展形式             | 描述                                                                 |
|--------------------------|----------------------------------------------------------------------|
| `${param:-default}`      | 如果 `param` 为 null，则将其设置为 `default` 的值                   |
| `${#param}`              | 返回 `param` 的长度                                                 |
| `${param%word}`          | 从末尾开始，删除最小部分与 `word` 匹配的 `param` 内容，返回其余部分 |
| `${param%%word}`         | 从末尾开始，删除最大部分与 `word` 匹配的 `param` 内容，返回其余部分 |
| `${param#word}`          | 从开头开始，删除最小部分与 `word` 匹配的 `param` 内容，返回其余部分 |
| `${param##word}`         | 从开头开始，删除最大部分与 `word` 匹配的 `param` 内容，返回其余部分 |

这些替换在处理字符串时非常有用。最后四种替换（删除字符串的一部分）在处理文件名和路径时尤其有用，如下面的例子所示。

---

### 🧪 试试看：参数处理（Parameter Processing）

下面脚本的每一部分都展示了参数匹配操作符的使用方式：

```sh
#!/bin/sh

unset foo
echo ${foo:-bar}

foo=fud
echo ${foo:-bar}

foo=/usr/bin/X11/startx
echo ${foo#*/}
echo ${foo##*/}

bar=/usr/local/etc/local/networks
echo ${bar%local*}
echo ${bar%%local*}

exit 0
```

这将产生如下输出：

```
bar
fud
usr/bin/X11/startx
startx
/usr/local/etc
/usr
```

---

### 它是如何工作的（How It Works）

第一个语句 `${foo:-bar}` 返回值 `bar`，因为在该语句执行时 `foo` 没有值。变量 `foo` 保持不变，因为它仍未被设置。

> `${foo:=bar}` 会将变量设置为 `$foo`。该字符串操作符检查 `foo` 是否存在且不为 null。如果不为 null，则返回其值；否则将其设置为 `bar` 并返回该值。  
> `${foo:?bar}` 将打印 `foo: bar` 并在 `foo` 不存在或为 null 时中止命令！  
> 最后，`${foo:+bar}` 会在 `foo` 存在且不为 null 时返回 `bar`。选择可真多！

`${foo#*/}` 表达式匹配并移除最左边的 `/`（记住 `*` 匹配零个或多个字符）。`${foo##*/}` 匹配并尽可能多地移除，因此会移除最右边的 `/` 及其之前的所有内容。

`${bar%local*}` 表达式从右向左匹配字符，直到匹配第一个 `local`（后跟任意数量的字符）为止；而 `${bar%%local*}` 则尽可能多地从右向左匹配，直到找到最左边的 `local`。

由于 UNIX 和 Linux 都高度依赖过滤器这一理念，一个操作的结果必须经常手动重定向。比如，你想使用 `cjpeg` 程序将一个 GIF 文件转换为 JPEG 文件：

```sh
$ cjpeg image.gif > image.jpg
```

有时你可能想对大量文件执行这种操作。你如何自动化重定向？像这样就行：

```sh
#!/bin/sh

for image in *.gif
do
  cjpeg $image > ${image%.gif}jpg
done
```

该脚本（命名为 `giftojpeg`）会为当前目录中的每个 GIF 文件创建一个 JPEG 文件。

---

## Here Documents（此处文档）

从 shell 脚本向命令传递输入的一种特殊方式是使用 here 文档。这种文档允许一个命令像从文件或键盘读取一样执行，实际上却是从脚本中获取输入。

Here 文档以 `<<` 开头，后跟一个特殊的字符序列，并在文档末尾重复该序列。`<<` 是 shell 的重定向标记，在此处强制命令输入为 here 文档。该特殊序列充当标记，告诉 shell here 文档从哪里开始和结束。该标记序列不能出现在传递给命令的行中，因此最好用容易记住和不常见的标记。

---

### 🧪 试试看：使用 Here 文档（Using Here Documents）

最简单的例子是向 `cat` 命令传递输入：

```sh
#!/bin/sh

cat <<!FUNKY!
hello
this is a here
document
!FUNKY!
```

这将产生如下输出：

```
hello
this is a here
document
```

Here 文档可能看起来有点奇怪，但它们非常强大，因为它们允许你调用交互式程序（如编辑器）并传递一些预定义的输入。不过，它们更常用于从脚本内部输出大量文本，如你前面所见，避免必须为每一行使用 echo 语句。你可以在标识符两侧使用感叹号（!）以确保没有歧义。

如果你希望以预定的方式处理文件中的多行，可以使用 `ed` 行编辑器，并在 shell 脚本中使用 here 文档向其传递命令。

---

### 🧪 试试看：Here 文档的另一个用法（Another Use for a Here Document）

**1.** 创建一个名为 `a_text_file` 的文件，内容如下：

```
That is line 1
That is line 2
That is line 3
That is line 4
```

**2.** 你可以使用 here 文档和 `ed` 编辑器组合来编辑该文件：

```sh
#!/bin/sh

ed a_text_file <<!FunkyStuff!
3
d
.,\$s/is/was/
w
q
!FunkyStuff!

exit 0
```

如果你运行这个脚本，文件内容将变为：

```
That is line 1
That is line 2
That was line 4
```

---

### 它是如何工作的（How It Works）

Shell 脚本调用 `ed` 编辑器，并将所需命令传递给它：移动到第 3 行，删除该行，然后用当前行的内容替换它（因为第 3 行被删除，现在的当前行变成了原来的最后一行）。这些 `ed` 命令取自脚本中的 here 文档——也就是标记 `!FunkyStuff!` 之间的行。

> **注意：在 here 文档中使用 `\` 是为了保护 `$` 不被 shell 展开。`\` 转义了 `$`，所以 shell 不会尝试将 `$s/is/was/` 展开为它的值（因为它并没有值）。相反，shell 会将文本 `\$` 原样传递为 `$`，然后由 `ed` 编辑器解释。**

---

## 调试脚本（Debugging Scripts）

调试 shell 脚本通常相当容易，但并没有专门的工具可用。本节将快速总结一些常用方法。

当出现错误时，shell 通常会打印出包含错误的那一行的行号。如果错误并不明显，你可以添加一些额外的 `echo` 语句来显示变量内容和测试代码片段，只需将它们输入 shell 交互式地运行即可。

由于脚本是解释执行的，修改和重试脚本时没有编译开销。跟踪更复杂错误的主要方法是设置各种 shell 选项。你可以通过命令行选项或使用 `set` 命令来设置。下表总结了这些选项：

| Command Line Option | set Option         | Description                                             |
|---------------------|--------------------|---------------------------------------------------------|
| `sh -n <script>`    | `set -o noexec`<br>`set -n` | 只检查语法错误；不执行命令                  |
| `sh -v <script>`    | `set -o verbose`<br>`set -v` | 在运行命令之前回显命令                         |
| `sh -x <script>`    | `set -o xtrace`<br>`set -x`  | 在处理完命令后回显命令                         |
| `sh -u <script>`    | `set -o nounset`<br>`set -u` | 使用未定义变量时报错                           |

你可以使用 `-o` 打开 `set` 选项标志，使用 `+o` 关闭，也可以使用简写版本。

你可以使用 `xtrace` 选项实现简单的执行跟踪。初步检查时可以使用命令行选项，但为了更细致的调试，你可以在脚本中问题代码周围设置 `xtrace` 标志（打开和关闭执行跟踪）。执行跟踪会在执行每一行之前打印该行，变量已展开。

使用以下命令开启 `xtrace`：

```sh
set -o xtrace
```

使用以下命令关闭 `xtrace`：

```sh
set +o xtrace
```

展开级别（默认）由每行开头的 `+` 数量表示。你可以通过设置 shell 配置文件中的 `PS4` 变量将其更改为更有意义的内容。

在 shell 中，你也可以通过捕捉 `EXIT` 信号来查看程序退出时的状态，例如在脚本开头加入如下行：

```sh
trap 'echo Exiting: critical variable = $critical_variable' EXIT
```

