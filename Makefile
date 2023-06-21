projet : 
	rm -rf build
	mkdir build
	gcc main.c -o build/main -lX11
	gcc fenetre.c -o build/fenetre -lX11
	gcc dialogue.c -o build/dialogue -pthread -lX11
	gcc dialogue2.c -o build/dialogue2 -pthread -lX11
	./build/main
