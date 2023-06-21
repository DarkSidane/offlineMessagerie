projet : 
	rm -rf build
	mkdir build
	gcc main.c -o build/main -lX11
	gcc dialogue.c -o build/dialogue -pthread -lX11
	./build/main
