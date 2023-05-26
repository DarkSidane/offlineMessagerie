#include <stdlib.h>
#include <stdio.h>
#define TRUE 1
#define FALSE 0

//Taille max d'une chaîne de caractères
#define S_MAX 20

int main()
{
	int running = TRUE;
	int enregistre = TRUE;
	char arg[S_MAX] = "";
	char nom[S_MAX] = "";

	while (running == TRUE)
	{
		// Boucle d'execution de notre programme
		char c[S_MAX];
		printf("Entrez une commande (e, p, q) \n");
		scanf("%s %s", &c[0], &arg);
		printf("%c_%s", c[0], arg);
		fflush(stdin);
		switch (c[0]) 
		{	
		case 'e' :
				// Si on veut enregistrer le nom d'utilisateur 
				printf("CATCH : %s \n", arg);
				if (arg != "")
				{
					printf("Votre nom d'utilisateur est %s\n", arg);
					printf("Enregistrement en cours...\n");
					enregistre = TRUE;
				}
				else 
					printf("Format incorrect, il faut mettre un nom d'utilisateur après le e");
			break;
		case 'p' : 
		if (enregistre==TRUE)
			printf("Ouverture de la fenêtre de dialogue\n");
		else 
			printf("Vous n'êtes pas enregistré ! \n");
		break;
		case 'q' : 
		printf("Au revoir.\n");
		running = FALSE;
		break;
		default : 
		printf("Veuillez écrire une lettre entre e, p ou q\n");
		break;
	}
	printf("\n===================================\n");
}
return EXIT_SUCCESS;
}
