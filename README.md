# digital-forensics-file-copy
This program will creates a new file from an existing file.  The old file could be in any directory, with the new file will be in the root directory.

### Contributors
* David Thigpen 
* Chase Cloutier
* David Stenson
* Eric Larson

### General Steps
1. Allocate a new inode (unused)
2. Copy all information from the old file inode to the new file inode.
3. Make a new name for the new file.
4. Link the new file  inode in the root directory inode.
5. Do ls in the root directory.  The file name you created should show in ls command.
6. Open the file to make sure everything is same as in old file. 

### Compiling Instructions
1. Download  the module:
```
cd digital-forensics/src/linux
git clone https://github.com/dthigpen/digital-forensics-file-copy.git copyfile
```
2. Add the module to the Makefile sections as you would any module:
	- .PHONY section
	- all config
	- new config for copyfile
	- clean config

### Note
This project cannot currently compile and run on its own. it requires the utd digital-forensics repository for its dependecies
