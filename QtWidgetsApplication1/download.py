import os
import re

currentPath=os.path.abspath(__file__)
currentDir=os.path.dirname(currentPath)

def clearFiles(filepath):
    # 遍历下载的文件
    for root,dir,files in os.walk(filepath):
        for filename in files:
             if('license' not in filename):
                file_path = os.path.join(root, filename)
                os.remove(file_path)

downloadDir = os.path.join(currentDir, "license")

if not os.path.exists(downloadDir):
    os.makedirs(downloadDir)

#递归下载网页所有的文件
os.system(f'cd /d {downloadDir} && {currentDir}/wget-1.20-win64/wget.exe --recursive --no-parent --reject=ico http://192.168.139.39:8080/')

#删除index网页
clearFiles(downloadDir)


"""
def clearFiles(filepath):
    # 遍历下载的文件
    for root,dir,files in os.walk(filepath):
        for filename in files:
            if('index' in filename or 'img' in filename):
                file_path = os.path.join(root, filename)
                os.remove(file_path)
"""
