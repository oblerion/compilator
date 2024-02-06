## Describle
Automatic compile simple raylib project,
config free/fast scan.

## Need
### on linux
```sh
sudo apt install clang mingw-w64 emscripten
```
### on window
gcc mingw

## Start
### Config dir 
- linux : deb package install in /opt/compilator
- window : C://compilator
> it contains lib for build linux64/win32/win64/web

### Run
launch compilator / compilator.exe in project dir 
#### on linux
```sh
compilator
```
```sh
compilator --target win32
```
```sh
compilator --target web 
```
## Working log
### on ubuntu / clang
### on window / gcc / wingw
#### 0.3 linux
- [x] run with path project
- [x] deb archive
- [x] web build
- [ ] full suport c++ sys lib
#### 0.3 windows
- [ ] testing

