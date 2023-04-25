'''
Coding: utf-8
Author: vector-wlc
Date: 2023-03-18 22:22:35
Description: 
'''
"""
Coding: utf-8
Author: vector-wlc
Date: 2021-12-06 23:44:11
Description: 
"""

import shutil
import os
import time
import sys

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Please input the input and outout dir")
        exit()
    src = sys.argv[1]
    dst = sys.argv[2]
    isCopyToCloud = len(sys.argv) > 3 and sys.argv[3] == "true"

    # 更新 AvZ 安装目录的文件
    for dir in ["inc", "src"]:
        shutil.rmtree(dst + dir)
        shutil.copytree(src + dir, dst + dir)

    for file in ["bin/injector.exe", "bin/libavz.a"]:
        shutil.copyfile(src + file, dst + file)

    # 打包版本文件
    avz_zip_name = time.strftime(src + "release/nightly/%Y_%m_%d.zip", time.localtime())
    cmd = "7z a -tzip " + avz_zip_name + " " + dst + "inc " + dst + "src " + dst + "bin"
    print(cmd)
    os.system(cmd)

    if not isCopyToCloud:
        exit()
    # 将所有的文件同步到云
    print("Copy To Cloud")
    dst = os.environ.get("CloudPath") + "/AsmVsZombies/"
    ignore_file = [".cache", ".vs", "bin", "build"]
    for file in os.listdir(src):
        if file not in ignore_file:
            try:
                shutil.copytree(src + file, dst + file, dirs_exist_ok=True)
            except NotADirectoryError:
                shutil.copyfile(src + file, dst + file)
    print("Copy To Cloud Done")
