# 基本设置

WSL 是 **Windows Subsystem for Linux** 的缩写，是微软推出的一项功能，允许用户在 Windows 系统上运行 Linux 环境，而无需安装虚拟机或双系统（到 WSL2 其实就是微软自带的虚拟机）。作为一个 windows 下的软件，你可以在 windows 下直接设置它的一些属性，见 wsl setting。如果你不清楚什么意思，不要去修改。

**检查 WSL 版本**：
   - 打开 Powershell，使用以下命令查看当前 WSL 版本：
     ```bash
     wsl --list --verbose
     ```

**切换到 WSL 2**：
   - 如果需要将 WSL 1 的发行版切换到 WSL 2，可以运行：
     ```bash
     wsl --set-version <发行版名称> 2
     ```

这里我们需要分清以下几个对象的关系：

你的计算机（硬件），windows 操作系统（操作系统软件），wsl（虚拟硬件），liunx 发行版，比如 ubuntu （操作系统软件）这四个对象之间的关系：

你通过 windows 操作系统来使用你的 PC，而 wsl 是一个虚拟的计算机，它是 Windows 下运行的一个程序，但伪装了一整套计算机硬件，然后你需要在这个虚拟的计算机中也独立的安装一个操作系统，来控制这个虚拟计算机。这就是一个 linux 发行版，比如 ubuntu 20.04 或者 debian 12。所以在安装 wsl 时，我们需要先安装 wsl，再安装 ubuntu 或 debian。安装完成之后，我们就可以通过一个具体的 linux 发行版来控制虚拟的计算机 wsl。因为安装的是 linux 发行版，所以我们控制的方式是通过 shell，而我们需要有一个 terminal 软件来和这个 shell 交流。可以是 Windows 提供的专门用于和 wsl 下运行的 linux 发行版的 shell 交流的 terminal 软件，比如：

+ 直接运行 ubuntu 20.04 或 debian 这样的 terminal 软件；
+ 或在命令行模式下，或 powershell 下，直接运行 bash，将命令行模式变成一个 terminal；
+ 或者通过 vscode 这种更加专业的终端软件。

所以我们应该将安装了 Linux 发行版的 wsl 看作是一台完整独立的电脑。操作它的逻辑和操作一台真正通过网络连接的服务器没有本质区别。理论上，它具有独立的软件系统。所以任何软件，比如 latex，如果我们需要在 wsl 中使用，我们必须在这个系统中重新安装。

**wsl 下的 linux 怎样和 windows 系统交流数据**：
作为一台独立机器，理论上它只能通过网络和你的主机交流数据。比如你可以将作业在 wsl 下 git 到我们的作业服务器上，然后在 windows 下独立的再下载下来。

但是毕竟物理上 wsl 只是 windows 的一个程序，所以 windows 为这种操作提供了后门，也就是你在 windows 下可以直接访问虚拟机的硬盘数据。它的地址是：`\\wsl$`

它模拟了访问一个网络文件夹，而实际上是直接访问了 wsl 的硬盘文件。

基本上，我们课程的操作都是在 linux 系统下的，所以这个操作其实不常用。

我们需要选择一个好用的 windows 终端。我的个人建议是 vs code，并且在 extensions 这里安装 wsl 插件，这样你基本上可以像我一样，将 vs code 当作一个编辑器的同时，也当作终端使用。

注意在这种模式下，我们编辑器使用的任何终端软件，比如 latex，python 或者 git，都是指在 wsl 下的软件，你必须确保在 wsl 下已经安装，安装的方式是：
```
sudo apt install <软件名>
```
比如安装 latex 是：
```
sudo apt install texlive-full
```
有点大，会占据大约 7G 的空间。

为了确保安装速度合适，我们需要修改我们的 linux 安装源，具体做法是：

## 1. **修改 Ubuntu 的安装源**
Ubuntu 的安装源配置文件位于 `/etc/apt/sources.list`。

debian 也一样。

### **步骤：**
1. **备份原始的源文件**：
   在修改前，建议备份原始文件：
   ```bash
   sudo cp /etc/apt/sources.list /etc/apt/sources.list.bak
   ```

2. **编辑软件源文件**：
   使用你喜欢的文本编辑器（如 `nano` 或 `vim`）编辑 `/etc/apt/sources.list` 文件：
   ```bash
   sudo nano /etc/apt/sources.list
   ```

3. **替换默认的源为速度更快的镜像源**：
   将文件中的内容替换为新的镜像源。例如，国内用户可以使用阿里云或清华大学的镜像源（以下以 Ubuntu 20.04 为例）：

   #### 阿里云源：
   ```plaintext
   deb http://mirrors.aliyun.com/ubuntu/ focal main restricted universe multiverse
   deb http://mirrors.aliyun.com/ubuntu/ focal-security main restricted universe multiverse
   deb http://mirrors.aliyun.com/ubuntu/ focal-updates main restricted universe multiverse
   deb http://mirrors.aliyun.com/ubuntu/ focal-backports main restricted universe multiverse
   deb-src http://mirrors.aliyun.com/ubuntu/ focal main restricted universe multiverse
   deb-src http://mirrors.aliyun.com/ubuntu/ focal-security main restricted universe multiverse
   deb-src http://mirrors.aliyun.com/ubuntu/ focal-updates main restricted universe multiverse
   deb-src http://mirrors.aliyun.com/ubuntu/ focal-backports main restricted universe multiverse
   ```

   #### 清华大学源：
   ```plaintext
   deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ focal main restricted universe multiverse
   deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ focal-updates main restricted universe multiverse
   deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ focal-backports main restricted universe multiverse
   deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ focal-security main restricted universe multiverse
   deb-src https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ focal main restricted universe multiverse
   deb-src https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ focal-updates main restricted universe multiverse
   deb-src https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ focal-backports main restricted universe multiverse
   deb-src https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ focal-security main restricted universe multiverse
   ```

我们也可以使用浙大的源：mirrors.zju.edu.cn    

   **注意：**
   - 将 `focal` 替换为你当前的 Ubuntu 版本代号，例如：
     - Ubuntu 22.04 → `jammy`
     - Ubuntu 20.04 → `focal`
     - Ubuntu 18.04 → `bionic`

4. **保存并退出**：
   在 `nano` 中按 `Ctrl+O` 保存，按 `Ctrl+X` 退出。

5. **更新软件包索引**：
   修改源后，运行以下命令更新软件包列表：
   ```bash
   sudo apt update
   ```

## 2. **检查当前 Linux 发行版和版本**
如果不确定自己使用的 Linux 发行版和版本，可以运行以下命令查看：

```bash
lsb_release -a
```

或：

```bash
cat /etc/os-release
```

输出示例：
```plaintext
NAME="Ubuntu"
VERSION="20.04.6 LTS (Focal Fossa)"
ID=ubuntu
VERSION_ID="20.04"
```

根据 `NAME` 和 `VERSION`，选择适合的镜像源。

到目前位置，我们需要安装的 linux 下软件有：
git，texlive-full，g++，make，cmake

我们目前暂时不需要使用 python，但如果你想使用 python，注意也最好安装在 linux 下。
注意 python 的版本和源是独立的，为了后续使用方便，我建议安装专门用于科学计算的 python 套件：anaconda。具体方法是：

1. 访问清华大学开源网站：
```
https://mirrors.tuna.tsinghua.edu.cn/
```

2. 找到并下载 anaconda，有经验的同学也可以只下载 minicoda，小很多。在
```
https://mirrors.tuna.tsinghua.edu.cn/anaconda/archive/
```
寻找对应系统的最新版本。注意你可以选择安装在 wsl 下（linux系统），或者安装在主系统下（windows系统）。我个人选择安装在 wsl 下，所以我选择下载：
```
https://mirrors.tuna.tsinghua.edu.cn/anaconda/archive/Anaconda3-5.3.1-Linux-x86_64.sh
```
因为清华大学源目前不支持 wget，所以大家可以在 windows 中下载以后，通过 \\wsl$ 复制到 wsl 中去。

3. 安装 anaconda：
```
bash Anaconda3-5.3.1-Linux-x86_64.sh
```
注意不用 sudo。

4. 将源改为清华大学，具体见：
```
https://mirrors4.tuna.tsinghua.edu.cn/help/anaconda/
```
如果你在 wsl 或 windows 中安装了 anaconda，建议在 vs code 中安装 python 扩展，并将其和 anaconda 连接。

目前 python 不是必须的。建议不熟悉的同学可以跳过这一步。此外，事实上任何终端软件，只要能连接上 wsl，都可以用 nano 完成剩余的工作。

本质上，wsl 只是另一台机器，我们可以用同样的逻辑，连接任何一台联网的服务器，比如我们的作业服务器，我可以用 ssh 连接并控制它。目前这个功能暂时不对班里同学开放。

以上，基本上是我们本课程学习和未来应用数学领域工作的基础。所以请参考文件和 AI，尽快对齐。我们课程的目的，是让大家都能够在这种既古老又现代的环境下，开展应用数学学习和科研。因此在大家普遍掌握基本技能之前，我们不会追求太多酷炫的功能。

# git 的逻辑和应用
大家参考我在群里上传的视频，应该不会对建立并生成一个空的 git 仓库有什么困难。这也是第一周的作业任务。现在我们稍微介绍一下 git 的使用逻辑。首先假设我拿到一台新的机器，那么我可以将我的 git 仓库 clone 到本地，继续我之前的工作。

以 10.72.190.121 为例。先在 wsl 下登录服务器：
```
ssh hywang@10.72.190.121
```
这是一个空的服务器。我现在将 git 的仓库复制下来。先通过网站登录我的 git 仓库。主要是为了获取仓库地址，如果你能记住，就可以跳过这一步。

在服务器下，clone 仓库：
```
git clone http://10.72.190.121:3000/Crazyfish/mathsoft_learning.git
```
因为我们使用的是 http 协议，所以需要输入用户名密码。输完我们的仓库就已经下来了。

如果使用 ssh 协议，那么我们要做的事情更多一些，但这种方式更加安全。这里涉及到 RSA 加密。首先，我们需要一对公钥和私钥，先确定它们是否存在：
```
ls ~/.ssh
```
如果你看到有 `id_rsa.pub` 和 `id_rsa` 这两个文件，那么它们就分别是你的公钥和私钥，你可以继续使用。也有可能是其他格式的公钥和私钥：

 **密钥类型与默认文件名**

| **密钥类型** | **算法**        | **私钥文件名**    | **公钥文件名**      | **说明**                                                       |
|--------------|-----------------|-------------------|---------------------|----------------------------------------------------------------|
| RSA          | rsa             | `id_rsa`         | `id_rsa.pub`        | 最常用，兼容性高，支持 2048 位或更高（如 4096 位）。            |
| ECDSA        | ecdsa           | `id_ecdsa`       | `id_ecdsa.pub`      | 基于椭圆曲线，速度快，密钥长度较短，支持 256/384/521 位。       |
| ED25519      | ed25519         | `id_ed25519`     | `id_ed25519.pub`    | 更现代的椭圆曲线算法，推荐使用，固定 256 位，安全性高。         |
| DSA          | dsa             | `id_dsa`         | `id_dsa.pub`        | 已过时，安全性低，固定 1024 位，不推荐使用。                   |
| RSA1         | rsa1            | `id_rsa1`        | `id_rsa1.pub`       | 用于旧版 SSH 协议 1，已过时且不安全，不推荐使用。              |


**生成 RSA 密钥**
```bash
ssh-keygen -t rsa -b 4096 -C "your_email@example.com"
```

**生成 ECDSA 密钥**
```bash
ssh-keygen -t ecdsa -b 384 -C "your_email@example.com"
```

**生成 ED25519 密钥**
```bash
ssh-keygen -t ed25519 -C "your_email@example.com"
```
**建议选择的密钥类型**
- **现代场景推荐：ED25519**
  - 提供更强的安全性和更高的性能。
  - 支持的服务广泛（如 GitHub、GitLab 等）。
- **兼容性要求高：RSA**
  - 如果需要兼容较旧的系统或服务，可以选择 RSA（长度建议为 4096 位）。

一般系统默认是生成 RSA，对于我们交作业，这个默认安全性已经足够了。

```bash
ssh-keygen -t rsa -b 4096 -C "your_email@example.com"
```

**执行该命令后，会出现以下提示：**

1. **指定密钥存储路径：**
   ```plaintext
   Enter file in which to save the key (/home/username/.ssh/id_rsa):
   ```
   - **默认：** 按回车键直接使用默认路径（`~/.ssh/id_rsa`）。

2. **设置密码（可选）：**
   ```plaintext
   Enter passphrase (empty for no passphrase):
   ```
   - 如果需要为私钥设置密码，输入密码后按回车。
   - 如果不需要密码保护，直接按回车跳过。

3. **确认密码：**
   ```plaintext
   Enter same passphrase again:
   ```
   - 再次输入密码进行确认（如果设置了密码）。


对于没有经验的同学，一直拍回车就行。这样会生成的密钥对包括两部分：

- **私钥：** 默认存储在 `~/.ssh/id_rsa`，需要妥善保管，绝对不能泄露。
- **公钥：** 默认存储在 `~/.ssh/id_rsa.pub`，可以发送给需要验证你身份的服务器或服务。


你可以运行以下命令查看密钥文件：
```bash
ls ~/.ssh/
```
输出示例：
```plaintext
id_rsa  id_rsa.pub
```

运行以下命令查看公钥内容：
```bash
cat ~/.ssh/id_rsa.pub
```
输出示例：
```plaintext
ssh-rsa AAAAB3NzaC1yc2E...your_key_content... user@hostname
```
这个文件暴露是没有关系的。

接下去需要将这个公钥上传到 git 服务器。在你的远端仓库的网页界面，注意左边有一个配置，里面有一个条目：部署密钥。点击进入，标题栏用于备忘注记，根据实际情况写。一般写具体终端的名字用于区分，比如我这里可以写：190.121 server （名字有server，但现在的地位是客户端。）

然后把刚才生成的公钥文件（rsa.pub）全部复制黏贴进去，注意不要漏掉一个字符。

选择是否允许修改远程仓库（不允许就只能读，建议允许。）

然后选择部署。注意密钥列表已经更新。凡是提交了公钥的终端，都可以直接登录服务器，不需要用户名密码。我们试一下。

```
hywang@computing:~/temp$ git clone gitea@10.72.190.121:Crazyfish/mathsoft_learning.git
Cloning into 'mathsoft_learning'...
The authenticity of host '10.72.190.121 (10.72.190.121)' can't be established.
ED25519 key fingerprint is SHA256:g5K0vPmT4h+wcRhXg03YF/vtOUBdSyqOWPpKRygQmbg.
This key is not known by any other names.
Are you sure you want to continue connecting (yes/no/[fingerprint])? yes
```
 
第一次使用密钥对会出现警告，直接 yes 即可。以后就不会有警告了。大家可以自行选择使用 ssh 还是 http，在一些严格的网站，比如 github，只允许 ssh，而且必须使用足够强度的密钥。还有一个问题就是千万不要泄露密钥，特别是公钥密钥对一起泄露。你的每一台机器都应该有独立的公钥密钥对，用于区分登录的位置。

现在我们可以对仓库中的文件做一些修改，比如用 nano 稍微编辑一下 `README.md`。然后我们就会在检查到这个文件的变化：
```
hywang@computing:~/mathsoft_learning$ git status
On branch main
Your branch is up to date with 'origin/main'.

Changes not staged for commit:
  (use "git add <file>..." to update what will be committed)
  (use "git restore <file>..." to discard changes in working directory)
        modified:   README.md

no changes added to commit (use "git add" and/or "git commit -a")
hywang@computing:~/mathsoft_learning$
```

git 能检测到仓库内的文件有变化，并提示你可以选择 `"git add <file>..."` 接受这个变化，或者 `"git restore <file>..."` 放弃这个变化。注意接受错了很容易删除，但是放弃了就不能撤销，所以谨慎使用后一个选择。

这时你再次检查一下你的本地仓库：
```
hywang@computing:~/mathsoft_learning$ git status
On branch main
Your branch is up to date with 'origin/main'.

Changes to be committed:
  (use "git restore --staged <file>..." to unstage)
        modified:   README.md

hywang@computing:~/mathsoft_learning$
```
你会看到状态发生变化：从 `Changes not staged for commit:` 变成了 `Changes to be committed:` 。这个时候， git 只是对需要记录的变化打了标记，并没有真正的记录。这里顺便解释一下上面的文字：

+ `On branch main` ：你当前处于 Git 的 main 分支上。暂时你不需要知道什么是分支，保持这个默认状态就可以；
+ `Your branch is up to date with 'origin/main'.` ：当前分支（main）与远程分支（origin/main）是同步的，没有未推送或未拉取的更改。这里的 “当前分支（main）” 指你的本地仓库，而 “远程分支（origin/main）” 指你在互联网服务器上的仓库内容。这句话表明你的本地仓库和远程仓库的内容完全一致。你可能会疑惑，你不是明明对`README.md`文件做了修改么？那是因为我们目前还没有将这个修改提交到本地仓库，只是对修改本身做了确认并打了标记。
+ `Changes not staged for commit:` ：在第一次 `git status` 中，我们得到的这个回复表明本地的文件修改（指 `README.md`）还没有暂存（staged），没有打标记。而在第二次查询，我们发现这一条变为：
+ `Changes to be committed:` ：本地的修改将会被提交（到本地仓库）。这就是中间执行的 `git add` 的作用。它是打一个修改标记，或者说暂存。这里专门提示了，
+ `use "git restore --staged <file>..." to unstage` ：你可以用 `"git restore --staged <file>..."` 的方式取消这个暂存，恢复到没有用 add 打标记的状态。
+ `modified:   README.md` ：列出修改过的文件。注意 `README.md` 是原本就在仓库中的文件。如果我们增加一个新文件：

```
touch LICENSE
```

则在 `git status` 下状态是：
```
Untracked files:
  (use "git add <file>..." to include in what will be committed)
        LICENSE
```
这是一个没有被跟踪的文件，说明仓库中原本没有这个文件。把它加进去的方式也很简单，一样用 `add` 命令打上标记。

由于我们有时会产生大量我们不想跟踪的临时文件，比如常见的场合是一个 latex 项目中本质上我们只需要保存和生成最终文档有关的数据，也就是 tex 文件，图片（如果有），以及 bib 或 sty 等和参考文献或额外格式有关的文件。其他如 log，aux 等等都是中间文件，干扰我们的视线。一个聪明的办法是在工作目录的最顶层，增加一个`.gitignore` 文件，因为在 linux 下以 `.` 开头的文件是隐藏的，所以你看不见这个文件。只要打 `nano .gitignore` 就可以，如果它不存在，那么会产生一个新的，如果存在，你会看到之前的内容，可以做修改。这个文件的功能是告诉系统什么文件不要跟踪。大家可以参考：
```
# 生成的 PDF、DVI、PS 文件
*.pdf
*.dvi
*.ps

# LaTeX 编译过程中产生的中间文件
*.aux
*.bbl
*.bcf
*.blg
*.fdb_latexmk
*.fls
*.log
*.out
*.run.xml
*.synctex.gz
*.toc
*.lof
*.lot
*.nav
*.snm
*.vrb

# 临时文件和备份文件
*.bak
*.tmp
*.swp
*~
```
所有这些后缀的文件都会被无视。（用 `*.o` 和 `*.so` 在 wsl/temp 下测试效果）注意你可以把 `.gitignore` 文件本身也 add 到库里。git 对文件的追踪只会显示子目录，而不会主动探索子目录。但你可以手工用 add 将一个子目录里的文件增加进去。比如我们增加一个目录 Basic，然后在 Basic 下增加一个文件 `README.md`。回到工作目录顶层，我们会发现，Untracked files 只到 `Basic` 为止，看不见 `Basic/README.md`，但你可以手工增加：
```
hywang@computing:~/mathsoft_learning$ git add Basic/README.md
```
这样相当于将整个 Basic 目录内的所有文件都纳入检测范围。这个目录中再有新的文件，也会出现在 Untracked files 的列表中。

现在我们提交更改到本地仓库：
```
git commit -m"add Basic"
```
如果你是在一个终端上第一次使用 git，那么当你第一次 commit 时，会出现错误信息：
```
hywang@computing:~/mathsoft_learning$ git commit -m"add Basic"
Author identity unknown

*** Please tell me who you are.

Run

  git config --global user.email "you@example.com"
  git config --global user.name "Your Name"

to set your account's default identity.
Omit --global to set the identity only in this repository.

fatal: unable to auto-detect email address (got 'hywang@computing.(none)')
hywang@computing:~/mathsoft_learning$ git commit
Author identity unknown

*** Please tell me who you are.

Run

  git config --global user.email "you@example.com"
  git config --global user.name "Your Name"

to set your account's default identity.
Omit --global to set the identity only in this repository.

fatal: unable to auto-detect email address (got 'hywang@computing.(none)')
```
这各原因是 git 需要知道你是谁，所以你需要用下面命令设置：
```
  git config --global user.email "you@example.com"
  git config --global user.name "Your Name"
```
告诉 git 你的名字和 Email。因为 git 是为团队合作而设计的，因此每一次提交都必须记录提交者是谁，以便于出现问题时及时和提交者沟通。

这里 `-m"add Basic"` 的意思是增加一个更新信息备忘。因为这个命令将导致更新被提交到本地仓库，此后我们可以在仓库中追踪到每一次更新，因此需要用一个简短的备忘告诉日后的自己或合作伙伴，这一次提交的目的是什么。对于小的提交，我们可以简单的用一句话备忘。对于重要的提交，强烈建议写认真写一篇更新记录。具体做法是直接用`git commit`，不加 `-m"<备忘信息>"` 后缀，这样将导致弹出一个 nano 界面，要求你写详细的更新备忘。注意全部以 `#` 开头的行都不会被真正的记录。这个只是系统对你的提示。

一般在这里，我们可以写下新增功能：
```
feat(auth): 支持用户邮箱登录

此次改动新增了用户邮箱登录功能，以满足用户多样化的登录需求。
改动内容：
- 新增邮箱验证模块。
- 更新了登录接口的逻辑。

注意：需要数据库 schema 更新（字段 `email` 新增唯一约束）。

Issue: #45
```
或者修复 bug：
```
fix(cart): 修复购物车价格计算错误

修复了由于浮点数精度导致的购物车总价计算错误，改为使用 BigDecimal 进行精确计算。
测试覆盖率已提高至 100%。

Issue: #78
```
或者更新文档：
```
docs(readme): 更新安装步骤

更新了 README 文件中的安装步骤，增加了本地开发环境的配置说明。
```
等等。

当 `commit` 命令完成时，你的更新已经被提交到本地仓库，而不是远程仓库。这是状态会告诉你：
```
hywang@baboon:~/temp/mathsoft_learning$ git status
On branch main
Your branch is ahead of 'origin/main' by 1 commit.
  (use "git push" to publish your local commits)
```
本地仓库比远程仓库（网站）领先了一个提交。符合实际情况。这个时候，如果你对任何一次提交后悔了，你可以返回到任何一次提交之前的状态。这个具体请咨询 AI。相当于你可以做本地的 undo/redo，并且不会影响到你的远程仓库。如果你确实打算将本地仓库的更新提交到远程仓库，让二者在
```
Your branch is ahead of 'origin/main' by <some> commit.
```
的状态下保持同步，那么你可以用 `git push` 命令。这个命令将本地的仓库的更新提交到远程仓库。一个对称的命令是 `git pull`，它将远程仓库的更新下载到本地。同时更新本地的文件。比如在 A 和 B 两台机器上都有同一个项目的本地仓库，那么你可以在 A 工作，push 到远程。然后再 B 上 pull，获得 A 提供的最新更新。正常情况下，A 和 B 很可能分别对应我在办公室的电脑和我在家里的电脑。

以上的工作流程适合一个人的工作。如果多人协作，则会由更加复杂的工作流程。因此会出现分支等概念。目前我们先不讨论。但即便是个人工作，也有可能出现提交冲突。比如你在 A 上的工作忘了 push，导致你在 B 上工作的时候重复修改了一些已经在 A 上修改过的文件，但你在 B 上却 push 了更改。这样当你再回到 A 并且 pull 的时候，就会发现远程仓库中有文件和本地已经 commit 但是没有 push 的文件不一致，这就是冲突。这只是一种可能的冲突，它更多发生在个人工作情景。可以想象，如果是多人合作，冲突会更加频繁出现。冲突的出现说明系统已经无法判定远程仓库和本地仓库中不一致的内容哪一个才是正确的。因此唯一的办法是人工接入。这里甚至不能依赖 AI，因为它的解决需要个人记忆。一个可能的发生冲突的文件会被修改成：
```
<<<<<<< HEAD
这是当前分支的内容
=======
这是另一个分支的内容
>>>>>>> feature
```
你手工决定需要保留哪一个分支的内容，并且将其他提示和重复都删除。然后保存文件，重新 add，commit 并 push 一次确保问题被最终解决。如果你总能保持下面的工作习惯，则个人工作时不会发生冲突：

+ 开始工作：进入本地的工作目录，先 pull 一次，确保和远程仓库一致；
+ 每当连续工作一段时间，有一个阶段性成果，比如完成一个文件的更新，就 add 并 commit 一次；（如果是多人协作，此时建议 pull 一次）
+ 每当需要长时间脱离当前的工作环境，比如午饭、下午茶、运动或下班回家，就 push 全部的工作；

如果个人工作还出现冲突，一般是忘记了上面的一步或者多步。

测试一下冲突，和冲突的解决。比如我现在在 wsl （假设这是我办公室机器）的工作目录下认真修改了 README.md 文件，但是忘记提交了。现在我去 190.121 （模拟我家里的机器）上也修改我的工作目录下的 README.md，而且这次我提交并且 push 到了远程仓库。现在我回到 wsl，按照工作习惯我先打了一个 pull。现在问题出现了，git 发现我本地文件 README.md 已经修改了，但没有提交，且远程仓库中的 README.md 也有没有下载到这个终端的更新，所以存在冲突的可能。但此时系统还是尝试自动合并两个不一致的文件。所以它建议我先提交本地的修改。
```
hywang@computing:~/temp/mathsoft_learning$ git pull
remote: Enumerating objects: 5, done.
remote: Counting objects: 100% (5/5), done.
remote: Compressing objects: 100% (3/3), done.
remote: Total 3 (delta 1), reused 0 (delta 0), pack-reused 0
Unpacking objects: 100% (3/3), 291 bytes | 291.00 KiB/s, done.
From 10.72.190.121:Crazyfish/mathsoft_learning
   16cd3b5..59a53a9  main       -> origin/main
Updating 16cd3b5..59a53a9
error: Your local changes to the following files would be overwritten by merge:
        README.md
Please commit your changes or stash them before you merge.
Aborting
```
按照提示，先提交本地修改，在 add 和 commit 阶段都没有问题，但是在 push 的时候会出错：
```
hywang@baboon:~/mathsoft_learning$ git add README.md
hywang@baboon:~/mathsoft_learning$ git commit -m"lol"
[main f25466f] lol
 1 file changed, 1 insertion(+)
hywang@baboon:~/mathsoft_learning$ git push
Username for 'http://10.72.190.121:3000': Crazyfish
Password for 'http://Crazyfish@10.72.190.121:3000':
To http://10.72.190.121:3000/Crazyfish/mathsoft_learning.git
 ! [rejected]        main -> main (non-fast-forward)
error: failed to push some refs to 'http://10.72.190.121:3000/Crazyfish/mathsoft_learning.git'
hint: Updates were rejected because the tip of your current branch is behind
hint: its remote counterpart. Integrate the remote changes (e.g.
hint: 'git pull ...') before pushing again.
hint: See the 'Note about fast-forwards' in 'git push --help' for details.
hywang@baboon:~/mathsoft_learning$
```
这个出错是因为此时远程仓库和本地仓库都有更新，因此可能出现冲突。所以它先拒接了这一次 push，要求我先做一次 pull 以便获得远程仓库的更新并在本地对比二者的区别。照做：
```
hywang@baboon:~/mathsoft_learning$ git pull
Username for 'http://10.72.190.121:3000': Crazyfish
Password for 'http://Crazyfish@10.72.190.121:3000':
hint: You have divergent branches and need to specify how to reconcile them.
hint: You can do so by running one of the following commands sometime before
hint: your next pull:
hint:
hint:   git config pull.rebase false  # merge
hint:   git config pull.rebase true   # rebase
hint:   git config pull.ff only       # fast-forward only
hint:
hint: You can replace "git config" with "git config --global" to set a default
hint: preference for all repositories. You can also pass --rebase, --no-rebase,
hint: or --ff-only on the command line to override the configured default per
hint: invocation.
fatal: Need to specify how to reconcile divergent branches.
```
这时系统建议我确定默认情况的怎样 pull。这里建议新手选择 ` git config pull.rebase false `，也就是在默认情况下，如果远程仓库和本地仓库都有更新，尝试合并两个更新。很多时候如果我们是多人合作的项目，每个人都有分工，不会重叠修改同一个文件，那么即便本地和远程的仓库都有更新，这种更新是相容的。可以合并。然而在这个故意设计的例子中。我本地的 README.md 和远端的 README.md 不一致，而且二者都声称是最新的提交，因此系统无法判定哪一个才是我要的。此时我必须手动介入：
```
hywang@baboon:~/mathsoft_learning$ git config pull.rebase false
hywang@baboon:~/mathsoft_learning$ git pull
Username for 'http://10.72.190.121:3000': Crazyfish
Password for 'http://Crazyfish@10.72.190.121:3000':
Auto-merging README.md
CONFLICT (content): Merge conflict in README.md
Automatic merge failed; fix conflicts and then commit the result.
```
这时看一下发生冲突的文件：
```
nano README.md
```
你会发现它已经标出了冲突的部分。手工修复，去掉不需要的部分。然后重新提交并push。这是唯一从逻辑上能根本上修复冲突的办法。对于其他终端，相当于得到了一个新的远程更新，它们会在下一次 pull 中更新到你手工恢复的文件。

最后，我们说明一下实际上本地的仓库就是当前工作目录下的 .git 文件。而远程仓库，就是这个隐藏目录的一个远程在线版本。在极端情况下，只要你能将本地的 .git 目录复制出来，你就可以恢复你的本地仓库。当保持和远程仓库一致显然是一个更加安全的做法。

所以保持用 git 工作，就是保持一种合理的工作习惯，让你的工作不至于由于失误而丢失，同时也让一个多人协作的项目能够以高效和负责任的方式展开。这是从事项目研究和开发的必备技能。哪怕是纯数学的同学，使用这种流程工作，也能最大可能地确保你的论文、项目、毕业论文、以及合作者（比如你的毕业论文导师）的工作不会因为失误而丢失，同时分工和贡献明确。所以是我们必须掌握的技能。

我们继续 git 的讨论，但是把主题转移到 C 语言。

# Linux 下的 C 语言







