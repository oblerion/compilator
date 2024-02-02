## Describle
Automatic compile simple raylib project,
config free/fast scan.

## Start
### Config dir 
unzip compilator_pack.zip in :
- on linux /home/$USER/.compilator
- on window C://compilator
> it contains lib for build linux64/win32/win64/web

### Run
launch compilator / compilator.exe in project dir 

## Working log
### on ubuntu / clang
### on window / gcc
#### 0.1
- [x] scan include dir -> get depends
- [x] scan source dir -> get all need to be compile
- [x] compile config 
- [x] suport lua,raylib natif
#### 0.2
- [x] draw error on UI
- [x] folder lib
- [x] i686-mingw32 lua raylib on linux
- [x] x86_64-mingw32 lua raylib on linux
- [x] fix memory error on link
#### 0.3
- [ ] web lua raylib on linux
- [ ] full suport c++ sys lib
