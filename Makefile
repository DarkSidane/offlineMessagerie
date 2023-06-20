projet : 
	rm -rf build
	mkdir build
	mkfifo build/u1u2
	mkfifo build/u2u1
	gcc main.c -o build/main -lX11
	gcc fenetre.c -o build/fenetre -lX11
	gcc dialogue.c -o build/dialogue -pthread -lX11
	./build/main
