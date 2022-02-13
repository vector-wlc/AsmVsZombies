'''
Coding: utf-8
Author: vector-wlc
Date: 2021-12-06 23:44:11
Description: 
'''

import shutil
import os
import time

dst = "d:/Software/AsmVsZombies/"

# 更新 AvZ 安装目录的文件
for dir in ["inc", "src"]:
    shutil.copytree(dir, dst + dir, dirs_exist_ok=True)

for file in ["bin/injector.exe", "bin/libavz.a"]:
    shutil.copyfile(file, dst + file)

# 打包版本文件
avz_zip_name = time.strftime("./release/ahpha/%Y_%m_%d.zip", time.localtime())
cmd = "7z a -tzip " + avz_zip_name + " " + dst + \
    "/inc " + dst + "/src " + dst + "/bin"
print(cmd)
os.system(cmd)
