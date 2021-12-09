'''
Coding: utf-8
Author: vector-wlc
Date: 2021-12-06 23:44:11
Description: 
'''

import shutil
import os
import time

# 更新 AvZ 安装目录的文件
if(os.path.exists("./bin/libavz_inject.dll")):
    os.remove("./bin/libavz_inject.dll")

for dir in ["inc", "src", "bin"]:
    shutil.copytree(dir, "d:/Software/AsmVsZombies/" + dir, dirs_exist_ok=True)

# 打包版本文件
avz_zip_name = time.strftime("./release/ahpha/%Y_%m_%d.zip", time.localtime())
cmd = "7z a -tzip " + avz_zip_name + " ./inc ./src ./bin"
print(cmd)
os.system(cmd)
