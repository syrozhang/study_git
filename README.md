# 如何使用git:

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

git checkout <name>：切换分支

git checkout -b <name>：创建+切换分支

git merge <name>：合并某分支到当前分支

git merge --no-ff：禁用fast-forward模式的合并，fast-forward模式合并会丢失分支信息

git branch -d <name>：删除分支

git log --graph --pretty=oneline --abbrev-commit：查看分支合并图

git stash：保存工作现场

git stash list：查看保存的工作现场

git stash apply：恢复工作现场但是不删除stash信息

git stash drop：删除stash信息

git stash pop：恢复工作现场的同时删除stash信息

git branch -D <name>：强行删除某个分支
