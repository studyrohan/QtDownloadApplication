import os
import re
import tkinter as tk

currentPath=os.path.abspath(__file__)
currentDir=os.path.dirname(currentPath)

def moveFile():
    licensefile = os.path.join(currentDir,"110.41.63.246+8080","Overdrive","overdrivelicense.h")
    savefile = os.path.join(currentDir, "overdrivelicense.h")
    os.rename(licensefile, savefile)

def clearDirectory(directory):
    for root, dirs, files in os.walk(directory):
        for dir_name in dirs:
            dir_path = os.path.join(root, dir_name)
            clearDirectory(dir_path)
        os.rmdir(root)

root = tk.Tk()
root.withdraw()

os.system(f'cd /d {currentDir} && {currentDir}/wget-1.20-win64/wget.exe --recursive --no-parent --reject=ico http://110.41.63.246:8080/Overdrive/overdrivelicense.h')

moveFile()
clearDirectory(os.path.join(currentDir, "110.41.63.246+8080"))