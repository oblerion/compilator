#!/bin/bash
DSRC="src/"
LUALINUX="/home/desnot/GD/lua-5.4.2/lua_linux"
LUAWIN="/home/desnot/GD/lua-5.4.2/lua_win"
LUAWEB="/home/desnot/GD/lua-5.4.2/lua_web"
CC=clang
if [ -f "src/main.cpp" ];then
	CC=clang++
fi
CFLAGS="-g -Wall -L${LUALINUX}/lib -llua -lraylib -lGL -lm -lpthread -ldl -lrt -lX11"
INC="-I${LUALINUX}/include" 
EXEC="main"
DOBJ="build/linux64/obj/"

RAYLIBWIN="/home/desnot/GD/raylib-5.0_win32_mingw-w64"
RAYLIBWEB="/home/desnot/GD/raylib-5.0_webassembly"
CC2=i686-w64-mingw32-gcc
CFLAGS2="-g -Wall -lm -L${LUAWIN}/lib -llua -L${RAYLIBWIN}/lib -lraylib -lopengl32 -lgdi32 -lwinmm -mwindows -static-libgcc" 
if [ -f "src/main.cpp" ];then
	CC2=i686-w64-mingw32-g++
	CFLAGS2="-g -Wall -lm -L${LUAWIN}/lib -llua -L${RAYLIBWIN}/lib -lraylib -lopengl32 -lgdi32 -lwinmm -mwindows -static-libgcc -static-libstdc++" 
fi
INC2="-I${LUAWIN}/include -I${RAYLIBWIN}/include -I/usr/i696-w64-mingw32/include"
EXEC2="main.exe"
DOBJ2="build/win86/obj/"

TCC="";
TCFLAGS="";
TINC="";
TEXEC="";
TLINK=" ";
TDOBJ=" ";

function init()
{
	TCC="$1";#compilo
	TINC="$2";#inlcude dir
	TCFLAGS="$3";#flag/ lib dir
	TEXEC="$4";#out exec
	TDOBJ="$5";#obj dir 
}
function comp(){
	local out;

	if [ -f "src/main.c" ];then
		cd $DSRC
		for i in *.c ;do
			out="$TDOBJ${i:0:-2}.o"
			rm -f $out;
			echo rm -f $out;
			command $TCC -c $i $TINC -g -Wall -o ../$out;
			echo $TCC -c $i $TINC -o ../$out;
			TLINK="$TLINK $out";
		done
	elif [ -f "src/main.cpp" ];then
		cd $DSRC
		for i in *.cpp ;do
			out="$TDOBJ${i:0:-4}.o"
			rm -f $out;
			echo rm -f $out;
			command $TCC -c $i $TINC -g -Wall -o ../$out;
			echo $TCC -c $i $TINC -o ../$out;
			TLINK="$TLINK $out";
		done
	fi
	cd ..
}
function link(){
	if [ -f $TEXEC ];then
		rm $TEXEC
	fi
	command $TCC $TLINK $TCFLAGS "-o" $TEXEC; 
	echo $TCC $TLINK $TCFLAGS "-o" $TEXEC; 
	if [ -f $EXEC ];then
		command ./$TEXEC;
	else 
		echo " "
		echo "[[ compile error ]]"
	fi;
}

if [ $# -eq 0 ];then 
# default
	rm -f $TEXEC;
	init "$CC" "$INC" "$CFLAGS" "$EXEC" "$DOBJ";
	comp;
	link;
elif [ "$1" == "w" ];then
	init "$CC2" "$INC2" "$CFLAGS2" "$EXEC2" "$DOBJ2";
	comp;
	link;
elif [ "$1" == "web" ];then
	DEXEC="build/web"
#	emcc -o game.html main.c -Os -Wall -I${RAYLIBWEB}/include -L${RAYLIBWEB}/lib -llibraylib.a -s USE_GLFW=3 -DPLATFORM_WEB
	for n in $DEXEC/* ;do
		if [ $n != "$DEXEC/favicon.ico" ] && [ $n != "$DEXEC/index.html" ];then
			rm -f $n
		fi
	done
# can craft with void ./asset dir
	if [ -f "src/main.cpp" ];then
		cd src;
		for i in "*.cpp" ;do
			em++ -c $i -I$RAYLIBWEB/include -Isrc;
			echo em++ -c $i -I../include;
		done;
		cd ..;
		em++ -o build/web/index.js src/*.o -Os -Wall $RAYLIBWEB/lib/libraylib.a -I. -Iinclude -L. -Llib -s USE_GLFW=3 -s ASYNCIFY -DPLATFORM_WEB --preload-file ./asset;
		echo "em++ -o build/web/index.js src/*.o -Os -Wall $RAYLIBWEB/lib/libraylib.a -I. -Iinclude -L. -Llib -s USE_GLFW=3 -s ASYNCIFY -DPLATFORM_WEB --preload-file ./asset";
	elif [ -f "src/main.c" ];then
		cd src;
		for i in "*.c" ;do
			emcc -c $i -I$RAYLIBWEB/include -Isrc;
			echo emcc -c $i -I../include;
		done;
		cd ..;
		emcc -o build/web/index.js src/*.o -Os -Wall $RAYLIBWEB/lib/libraylib.a -I. -Iinclude -L. -Llib -s USE_GLFW=3 -s ASYNCIFY -DPLATFORM_WEB --preload-file ./asset;
		echo "emcc -o build/web/index.js src/*.o -Os -Wall $RAYLIBWEB/lib/libraylib.a -I. -Iinclude -L. -Llib -s USE_GLFW=3 -s ASYNCIFY -DPLATFORM_WEB --preload-file ./asset";
	fi

	if [ "$2" = "z" ]; then
		cd build/web;
		zip -o ../../index.zip *
	fi
fi
