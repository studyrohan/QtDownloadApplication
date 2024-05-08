import os
import re
import tkinter as tk
from tkinter import filedialog
import subprocess

currentPath=os.path.abspath(__file__)
currentDir=os.path.dirname(currentPath)

def moveFile():
    licensefile = os.path.join(downloadDir,"110.41.63.246+8080","Overdrive","overdrivelicense.h")
    savefile = os.path.join(downloadDir, "overdrivelicense.h")
    os.rename(licensefile, savefile)

def clearDirectory(directory):
    for root, dirs, files in os.walk(directory):
        for file in files:
            file_path = os.path.join(root, file)
            os.remove(file_path)
        for dir_name in dirs:
            dir_path = os.path.join(root, dir_name)
            clearDirectory(dir_path)
        os.rmdir(root)

root = tk.Tk()
root.withdraw()

downloadDir = filedialog.askdirectory(initialdir=currentDir, title="choose a directory")
if not downloadDir:
    print("error,please choose a directory")
    exit()

wget_command = f'cd /d {downloadDir} && {currentDir}/wget-1.20-win64/wget.exe --recursive --no-parent --reject=ico http://110.41.63.246:8080/Overdrive/overdrivelicense.h'
subprocess.call(wget_command, shell=True)

moveFile()

clearDirectory(os.path.join(downloadDir, "110.41.63.246+8080"))
