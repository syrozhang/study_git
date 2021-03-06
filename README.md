# 如何使用git:

git config --global user.name "Your Name"：配置自己的名字

git config --global user.email "blabla@blabla.com"：配置自己的电邮

git config --global color.ui true：让git适时显示不同颜色

git config --global alias.new_cmd_name cmd_name：给git命令取别名，比如git config --global alias.ad add
，git config --global alias.cm commit

git init：初始化一个Git仓库

git add <file>：添加文件到缓存区

git commit：提交缓存区文件到当前分支

git status：工作区中被改变的文件

git diff：工作区文件和缓存区文件的区别

git diff HEAD -- file：工作区文件和当前分支文件的区别

git reset --hard commit_id | HEAD | HEAD^ | HEAD^^ | HEAD~n：HEAD指向当前版本，HEAD^指向上一版本，HEAD~n指向上n个版本

git log：查看提交历史，以便确定要回到哪个版本

git reflog：查看命令历史，以便确定要回到未来的哪个版本

git checkout -- file：丢弃工作区文件的修改

git rm file：从代码库中删除文件

ssh-keygen -t rsa -C "your_email@example.com"：创建SSH Key，在用户目录下的.ssh目录中会生成id_rsa和id_rsa.pub两个文件：id_rsa是私钥，id_rsa.pub是公钥

git remote add origin git@github.com:blabla/blabla.git：关联一个远程代码库，origin是远程库的默认名字

git push -u origin <branch_name>：第一次把本地代码库发送到远程代码库branch_name分支，之后只需要用git push origin <branch_name>即可

git clone git@github.com:blabla/blabla.git：把远程代码库复制到本地

git branch：查看分支

git branch <name>：创建分支

git branch --set-upstream branch-name origin/branch-name：关联远程代码库分支和本地分支

git checkout <name>：切换分支

git checkout -b <name>：创建+切换分支

git checkout -b <branch_name> origin/<branch_name>：创建远程代码库分支的本地分支

git merge <name>：合并某分支到当前分支

git merge --no-ff：禁用fast-forward模式的合并，fast-forward模式合并会丢失分支信息

git branch -d <name>：删除分支

git log --graph --pretty=oneline --abbrev-commit：查看分支合并图和提交记录

git stash：保存工作现场

git stash list：查看保存的工作现场

git stash apply：恢复工作现场但是不删除stash信息

git stash drop：删除stash信息

git stash pop：恢复工作现场的同时删除stash信息

git branch -D <name>：强行删除某个分支

git pull：下载远程代码库分支并合并

git tag：查看所有标签

git tag <tag_name>：给HEAD提交打标签

git tag <tag_name> <commit_id>：给commit_id提交打标签

git tag -d <tag_name>：删除本地标签

git push origin <tag_name>：上次tag_name标签

git push origin --tags：上次全部未上次的标签

git push origin :refs/tags/<tagname>：删除一个远程标签

git tag -a <tag_name> -m "blablabla"：指定标签信息

git tag -s <tag_name> -m "blablabla..."：PGP签名标签

# 如何参与一个开源项目

  1.在GitHub上，Fork希望参与的项目，然后自己的GitHub账号下就复制了这个项目

  2.然后从自己的Fork仓库下克隆到本地，这样才有权限修改和推送，若从原作者的项目克隆将因为没有权限而无法推送和修改

  3.比如人气极高的bootstrap项目，这是一个非常强大的CSS框架，你可以访问它的项目主页https://github.com/twbs/bootstrap， 点“Fork”就会在自己的账号下克隆了一个bootstrap仓库，然后，从自己的账号下clone：
        git clone git@github.com:syrozhang/bootstrap.git
  Bootstrap的官方仓库twbs/bootstrap、你在GitHub上克隆的仓库my/bootstrap，以及你自己克隆到本地电脑的仓库，他们的关系就像下图显示的那样：
              
          GitHub[Bootstrap的官方仓库twbs/bootstrap、你在GitHub上克隆的仓库my/bootstrap] ===> 本地PC[bootstrap]

如果你想修复bootstrap的一个bug，或者新增一个功能，立刻就可以开始干活，干完后，往自己的仓库推送。
如果你希望bootstrap的官方库能接受你的修改，你就可以在GitHub上发起一个pull request。
当然，对方是否接受你的pull request就不一定了。

#忽略特殊文件

在Git工作区的根目录下创建一个特殊的.gitignore文件，然后把要忽略的文件名填进去，Git就会自动忽略这些文件。不需要从头写.gitignore文件，GitHub已经为我们准备了各种配置文件，只需要组合一下就可以使用了。所有配置文件可以直接在线浏览： https://github.com/github/gitignore。
检验.gitignore的标准是git status命令是不是说working directory clean
