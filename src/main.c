/*
    GNU/Linux et MacOS
        > gcc src/main.c -o prog $(sdl2-config --cflags --libs)
    Windows
        > gcc src/main.c -o prog -I include -L lib -lmingw32 -lSDL2main -lSDL2
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <time.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mouse.h>
#include <SDL_mixer.h>
#define MAX_SCORES 10

int afficherMenu(SDL_Renderer *renderer);
int lancerJeu(SDL_Renderer *renderer);

TTF_Font* police = NULL;

typedef struct {
    SDL_Texture *texture;
    SDL_Rect rect;
} Branche;

// Fonction pour charger une image et créer une texture
SDL_Texture* chargerTexture(SDL_Renderer *renderer, const char *cheminImage) {
    SDL_Surface *surface = IMG_Load(cheminImage);
    if (surface == NULL) {
        fprintf(stderr, "Erreur lors du chargement de l'image : %s\n", SDL_GetError());
        return NULL;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (texture == NULL) {
        fprintf(stderr, "Erreur lors de la création de la texture : %s\n", SDL_GetError());
        return NULL;
    }

    return texture;
}

void ajouterBrancheAleatoire(Branche branches[], SDL_Texture* textureBranche) {
    int nbRand = rand() % 3 + 1;
    switch(nbRand) {
        case 1:
            // Ne pas ajouter de branche
            break;
        case 2:
            // Ajouter une nouvelle branche à gauche
            branches[0] = branches[1];
            branches[1] = branches[2];
            branches[2] = branches[3];
            branches[3] = branches[4];
            branches[4].texture = textureBranche;
            branches[4].rect.x = 175;  // Position initiale sur le tronc
            branches[4].rect.y = -120;  // Position en haut du tronc
            branches[4].rect.w = 150;  // Ajustez la largeur en fonction de vos besoins
            branches[4].rect.h = 28;  // Ajustez la hauteur en fonction de vos besoins
            break;
        case 3:
            // Ajouter une nouvelle branche à droite
            branches[0] = branches[1];
            branches[1] = branches[2];
            branches[2] = branches[3];
            branches[3] = branches[4];
            branches[4].texture = textureBranche;
            branches[4].rect.x = 475;  // Position initiale sur le tronc
            branches[4].rect.y = -120;  // Position en haut du tronc
            branches[4].rect.w = 150;  // Ajustez la largeur en fonction de vos besoins
            branches[4].rect.h = 28;  // Ajustez la hauteur en fonction de vos besoins
            break;
    }
}

int detecterCollision(SDL_Rect rectJoueur, SDL_Rect rectBranche) {
    return (rectJoueur.x < rectBranche.x + rectBranche.w &&
            rectJoueur.x + rectJoueur.w > rectBranche.x &&
            rectJoueur.y < rectBranche.y + rectBranche.h &&
            rectJoueur.y + rectJoueur.h > rectBranche.y);
}

// Définition des thèmes supportés
const char* supportedThemes[] = {"default", "forest", "space", "ocean"};
const int supportedThemesCount = sizeof(supportedThemes) / sizeof(supportedThemes[0]);


// Vérifie si un thème est supporté
bool isThemeSupported(const char* theme) {

    for (int i = 0; i < supportedThemesCount; i++) {

        if (strcmp(theme, supportedThemes[i]) == 0) {

            return true;

        }

    }

    return false;
    
}

// Définition des skins supportés
const char* supportedSkins[] = {"default", "timberman", "cow"};
const int supportedSkinsCount = sizeof(supportedSkins) / sizeof(supportedSkins[0]);

// Vérifie si un skin est supporté
bool isSkinSupported(const char* skin) {

    for (int i = 0; i < supportedSkinsCount; i++) {

        if (strcmp(skin, supportedSkins[i]) == 0) {

            return true;

        }

    }

    return false;
    
}


// Définition des thèmes supportés
const char* supportedThemes[] = {"default", "forest", "space", "ocean"};
const int supportedThemesCount = sizeof(supportedThemes) / sizeof(supportedThemes[0]);

// Vérifie si un thème est supporté
bool isThemeSupported(const char* theme) {

    for (int i = 0; i < supportedThemesCount; i++) {

        if (strcmp(theme, supportedThemes[i]) == 0) {

            return true;

        }

    }

    return false;
    
}

// Définition des skins supportés
const char* supportedSkins[] = {"default", "timberman", "cow"};
const int supportedSkinsCount = sizeof(supportedSkins) / sizeof(supportedSkins[0]);

// Vérifie si un skin est supporté
bool isSkinSupported(const char* skin) {

    for (int i = 0; i < supportedSkinsCount; i++) {

        if (strcmp(skin, supportedSkins[i]) == 0) {

            return true;

        }

    }

    return false;
    
}


// Lit le fichier de configuration et récupère les valeurs
void readConfigFile(char* theme, char* skin, double* difficulty, double* speed) {

    // Définition constantes

    const char defaultTheme[] = "default";
    const char defaultSkin[] = "default";
    const double defaultDifficulty = 1.0, minDifficulty = 0.10, maxDifficulty = 10.0;
    const double defaultSpeed = 1.0, minSpeed = 0.10, maxSpeed = 10.0;
    const int maxKeyLength = 20, maxValueLength = 40;

    // Initialisation avec des valeurs par défaut
    strncpy(theme, defaultTheme, sizeof(defaultTheme));
    strncpy(skin, defaultSkin, sizeof(defaultSkin));
    *difficulty = defaultDifficulty;
    *speed = defaultSpeed;

    FILE* f = fopen("config/timberman.config", "r"); // Ouverture en lecture seule du fichier de config

    if (f == NULL) {

        printf("Erreur lors de l'ouverture du fichier de configuration : %s\n", strerror(errno)); // Affichage de l'erreur si le fichier n'a pas pu être ouvert

        return;

    }

    char line[100];
    char key[maxKeyLength + 1], value[maxValueLength + 1];

    while (fgets(line, sizeof(line), f)) { // Lecture du fichier ligne par ligne

        if (sscanf(line, "%20s = %40s", key, value) == 2) { // Lecture de la ligne et vérification du format

            // Vérification et traitement pour le thème
            if (strncmp(key, "theme", 20) == 0) {

                if (isThemeSupported(value)) {

                    strncpy(theme, value, maxValueLength);
                    theme[maxValueLength] = '\0';

                } else {

                    printf("Theme non pris en charge : %s\n\n Le theme par defaut a ete applique !\n\n", value);

                    strncpy(theme, defaultTheme, maxValueLength);
                    theme[maxValueLength] = '\0';

                }

            }

            // Vérification et traitement pour le skin
            if (strncmp(key, "skin", 20) == 0) {

                if (isSkinSupported(value)) {

                    strncpy(skin, value, maxValueLength);
                    skin[maxValueLength] = '\0';

                } else {

                    printf("Skin non pris en charge : %s\n\n Le skin par defaut a ete applique !\n\n", value);

                    strncpy(skin, defaultSkin, maxValueLength);
                    skin[maxValueLength] = '\0';

                }

            }

            // Vérification et traitement pour la difficulté
            if (strncmp(key, "difficulty", 10) == 0) {

                *difficulty = atof(value);

                if (*difficulty < minDifficulty || *difficulty > maxDifficulty) {

                    printf("Erreur: La difficulte doit être comprise entre %.2f et %.2f ! Passage à la valeur par defaut (%.2f)\n\n", minDifficulty, maxDifficulty, defaultDifficulty);

                    *difficulty = defaultDifficulty;

                }

            }

            // Vérification et traitement pour la vitesse
            if (strncmp(key, "speed", 5) == 0) {

                *speed = atof(value);

                if (*speed < minSpeed || *speed > maxSpeed) {

                    printf("Erreur: La vitesse doit etre comprise entre %.2f et %.2f ! Passage a la valeur par defaut (%.2f)\n\n", minSpeed, maxSpeed, defaultSpeed);

                    *speed = defaultSpeed;

                }

            }

        }

    }

    fclose(f); // Fermeture du fichier de config
}


// Vérifie l'existence du fichier de configuration et le crée s'il n'existe pas
void verifyConfigFileExistence(const char* fichierConfig) {

    FILE* f = fopen(fichierConfig, "r"); // Ouverture en lecture seule du fichier de config

    if (f == NULL) {

        f = fopen(fichierConfig, "w"); // Création du fichier de config s'il n'existe pas

        if (f == NULL) {

            printf("Erreur lors de la creation du fichier de configuration : %s\n", strerror(errno)); // Affichage de l'erreur si le fichier n'a pas pu être créé

            return;

        }

        // Ecriture des valeurs par défaut dans le fichier de config

        fprintf(f, "theme = default\n");
        fprintf(f, "skin = default\n");
        fprintf(f, "difficulty = 1.00\n");
        fprintf(f, "speed = 1.00\n");

        fclose(f); // Fermeture du fichier de config

    } else {

        fclose(f); // Fermeture du fichier de config

    }

}



// Fonction pour comparer les entiers par ordre décroissant pour qsort
int comparerIntDesc(const void* a, const void* b) {
    return (*(int*)b - *(int*)a);
}

void verifyHighScoreFileExistence(int score) {

    FILE* f = fopen("highscore/highscore.txt", "a+");  // Ouvre le fichier en mode lecture/écriture

    if (f == NULL) {
        printf("Erreur lors de l'ouverture du fichier de score : %s\n", strerror(errno));
        return;
    }

    // Lire les scores existants
    int scores[MAX_SCORES];
    int numScores = 0;

    // Vérifier si le score existe déjà
    while (fscanf(f, "%d", &scores[numScores]) == 1 && numScores < MAX_SCORES) {
        if (score == scores[numScores]) {
            fclose(f);
            return;  // Le score existe déjà, pas besoin de faire plus
        }
        numScores++;
    }

    // Ajouter le nouveau score uniquement s'il est supérieur au plus bas des scores actuels
    if (numScores < MAX_SCORES || score > scores[MAX_SCORES - 1]) {
        scores[numScores++] = score;

        // Trier les scores par ordre décroissant
        qsort(scores, numScores, sizeof(int), comparerIntDesc);

        // Tronquer le fichier
        freopen("highscore/highscore.txt", "w", f);

        // Écrire de nouveau les 10 meilleurs scores dans le fichier
        for (int i = 0; i < numScores && i < MAX_SCORES; i++) {
            fprintf(f, "%d\n", scores[i]);
        }
    }

    fclose(f);
}

int afficherPopup(SDL_Renderer* renderer, int score) {
    // Charger la texture pour la popup de Game Over
    int quit = 0;
    while(quit == 0){
        SDL_Texture* textureGameOver = chargerTexture(renderer, "src/images/GameOver.png");
        SDL_Rect rectGameOver = {264.5, 62, 271, 576};
        // Dessiner la popup
        SDL_RenderCopy(renderer, textureGameOver, NULL, &rectGameOver);

        SDL_Texture *texturePlayAgain = chargerTexture(renderer, "src/images/playagain.png");
        SDL_Rect rectPlayAgain = {306.25, 400, 187.5, 62.5};
        SDL_RenderCopy(renderer, texturePlayAgain, NULL, &rectPlayAgain);

        SDL_Texture *textureMainMenu = chargerTexture(renderer, "src/images/menu.png");
        SDL_Rect rectMainMenu = {306.25, 500, 187.5, 62.5};
        SDL_RenderCopy(renderer, textureMainMenu, NULL, &rectMainMenu);

        // Afficher le score
        TTF_Font* scoreFont = TTF_OpenFont("src/fonts/KOMIKAP_.ttf", 40);
        SDL_Color couleurTexte = { 255, 255, 255 }; // Blanc
        char scoreTexte[50];
        snprintf(scoreTexte, sizeof(scoreTexte), "Score: %d", score);
        SDL_Surface* surfaceScore = TTF_RenderText_Solid(scoreFont, scoreTexte, couleurTexte);
        SDL_Texture* textureScore = SDL_CreateTextureFromSurface(renderer, surfaceScore);
        SDL_Rect rectScore = { 300, 225, surfaceScore->w, surfaceScore->h };
        SDL_RenderCopy(renderer, textureScore, NULL, &rectScore);

        // boutton cliquable
        SDL_PumpEvents();
        int x, y;
        Uint32 buttons = SDL_GetMouseState(&x, &y);

        if (buttons & SDL_BUTTON(SDL_BUTTON_LEFT) &&
            x >= rectMainMenu.x && x < rectMainMenu.x + rectMainMenu.w &&
            y >= rectMainMenu.y && y < rectMainMenu.y + rectMainMenu.h) {
            printf("Bouton MainMenu cliqué!\n");
            quit = afficherMenu(renderer);
            return quit;
        }

        // Vérifier si le clic gauche de la souris se produit sur le bouton "PlayAgain"
        if (buttons & SDL_BUTTON(SDL_BUTTON_LEFT) &&
            x >= rectPlayAgain.x && x < rectPlayAgain.x + rectPlayAgain.w &&
            y >= rectPlayAgain.y && y < rectPlayAgain.y + rectPlayAgain.h) {
            printf("Bouton PlayAgain cliqué!\n");
            return 1;
            // Réinitialiser le jeu ici, par exemple, en remettant le personnage à sa position initiale
        }
        SDL_RenderPresent(renderer);


        // Libérer les ressources de la popup
        SDL_FreeSurface(surfaceScore);
        SDL_DestroyTexture(textureScore);
        TTF_CloseFont(scoreFont);
        SDL_DestroyTexture(textureGameOver);
        SDL_DestroyTexture(textureMainMenu);
        SDL_DestroyTexture(texturePlayAgain);
        SDL_RenderPresent(renderer);
    }
}

int main(int argc, char *argv[]) {

    // Déclaration des variables de configuration

    char theme[100], skin[100];
    double difficulty, speed;

    // Vérification de l'existence du fichier de configuration et création si nécessaire
    verifyConfigFileExistence("config/timberman.config");

    // Lecture du fichier de configuration et récupération des valeurs
    readConfigFile(theme, skin, &difficulty, &speed);

    // Pour debug - Affichage des valeurs de configuration
    printf("\n\nParametres de configuration :\n\n");
    printf("Theme : %s\n", theme);
    printf("Skin : %s\n", skin);
    printf("Coefficient de difficulte : %.2f\n", difficulty);
    printf("Coefficient d'acceleration de la vitesse : %.2f\n", speed);
  
    
    // Initialisation de SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Erreur d'initialisation de SDL : %s\n", SDL_GetError());
        return 1;
    }

    // Initialisation de SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        fprintf(stderr, "Erreur d'initialisation de SDL_image : %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    // Initialisation de SDL_ttf
    if (TTF_Init() == -1) {
        fprintf(stderr, "Erreur d'initialisation de SDL_ttf : %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
	    fprintf(stderr, "Erreur lors de l'initialisation de SDL_mixer : %s\n", Mix_GetError());
	    // Gestion de l'erreur
	}

    // Créer la fenêtre
    SDL_Window *fenetre = SDL_CreateWindow("TimberMan", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 700, SDL_WINDOW_SHOWN);
    if (fenetre == NULL) {
        fprintf(stderr, "Erreur lors de la creation de la fenêtre : %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Créer le renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(fenetre, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        fprintf(stderr, "Erreur lors de la creation du renderer : %s\n", SDL_GetError());
        SDL_DestroyWindow(fenetre);
        SDL_Quit();
        return 1;
    }

    // Charger la police (remplacez "Chemin/vers/la/police.ttf" par le chemin réel de votre police)
    police = TTF_OpenFont("src/fonts/KOMIKAP_.ttf", 24);
    if (police == NULL) {
        fprintf(stderr, "Erreur lors du chargement de la police : %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    //Charger musique
    Mix_Music* musiqueDeFond = Mix_LoadMUS("src/sound/foretSenonches.wav");
    Mix_VolumeMusic(MIX_MAX_VOLUME / 2);
    Mix_PlayMusic(musiqueDeFond, -1); // -1 pour répéter la musique indéfiniment

    Mix_Chunk* SonCouperBois = Mix_LoadWAV("src/sound/treeChopSound.wav");
    Mix_VolumeChunk(SonCouperBois, MIX_MAX_VOLUME / 5);

    // Charger les texturess
    SDL_Texture *textureFond = chargerTexture(renderer, "src/images/background.png");
    SDL_Texture *texturePersonnage = chargerTexture(renderer, "src/images/playerTest3.png");
    SDL_Texture *textureArbre = chargerTexture(renderer, "src/images/tree.png");
    SDL_Texture* textureBranche = chargerTexture(renderer, "src/images/branch.png");

    SDL_Texture* textureHacheTape = chargerTexture(renderer, "src/images/playerTest4.png");

    SDL_Texture *textureBuche = chargerTexture(renderer, "src/images/particule.png");

    SDL_Texture* textureTimerBorder = chargerTexture(renderer, "src/images/TimerBorder.png");
    SDL_Rect rectTimerBorder = { 350, 10, 100, 34 };

    SDL_Rect rectBuche = { 325, 500, 150, 60 };  // Ajustez la position et la taille

    SDL_Rect rectPersonnage = { 470, 440, 150, 150 };
    SDL_Rect rectHache = { rectPersonnage.x + 20, rectPersonnage.y - 30, 50, 50 };
    // Orientation de la hache
    SDL_RendererFlip flipHache = SDL_FLIP_NONE;

    Branche branches[5];

    // Initialisation des branches
    for (int i = 0; i < 4; ++i) {
        int nbRand = rand() % 3 + 1;
        switch(nbRand) {
            case 1:
                break;
            case 2:
                branches[i].texture = textureBranche;
                branches[i].rect.x = 175;  // Position initiale sur le tronc
                branches[i].rect.y = i * 120;  // Espacement vertical de 120 pixels entre chaque branche
                branches[i].rect.w = 150;  // Ajustez la largeur en fonction de vos besoins
                branches[i].rect.h = 28;  // Ajustez la hauteur en fonction de vos besoins
                break;
            case 3:
                branches[i].texture = textureBranche;
                branches[i].rect.x = 475;  // Position initiale sur le tronc
                branches[i].rect.y = i * 120;  // Espacement vertical de 120 pixels entre chaque branche
                branches[i].rect.w = 150;  // Ajustez la largeur en fonction de vos besoins
                branches[i].rect.h = 28;  // Ajustez la hauteur en fonction de vos besoins
                break;
        }
    }

    int Jouer = 0;
   
    // Effacer le renderer
    SDL_RenderClear(renderer);

    // Afficher le menu
    Jouer = afficherMenu(renderer);

    // Rafraîchir l'écran
    SDL_RenderPresent(renderer);

    // score
    int score = 0;
    int tapeEnCours = 0;
    int tempsRestant = 3000;
    Uint32 startTime; // Variable pour stocker le temps initial en millisecondes
	Uint32 currentTime; // Variable pour stocker le temps actuel en millisecondes
	int timing = 0;
	int tempsBloque = 1;
    // Boucle principale du jeu
    SDL_Event event;
    srand(time(NULL));
    while (Jouer == 1) {
    	// Vérifiez si le temps est écoulé
    	if(tempsBloque == 0){
    		currentTime = SDL_GetTicks();
	    	currentTime = currentTime - timing;
	    	Uint32 elapsedTime = currentTime - startTime;
    		tempsRestant = 3000 - elapsedTime;
		    if (tempsRestant <= 0) {
		        // Le temps est écoulé, affichez Game Over et effectuez d'autres actions nécessaires
		        timing = 0;
		        tempsBloque = 1;
		        Jouer = afficherPopup(renderer, score); // Utilisez votre fonction afficherPopupGameOver
		        verifyHighScoreFileExistence(score);
		        score = 0;
		        tempsRestant = 3000; // Réinitialisez le temps
		        startTime = SDL_GetTicks(); // Réinitialisez le temps de départ
		    }
		}

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                Jouer = 0;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        // Déplacer le personnage à gauche de l'arbre
                        rectPersonnage.x = 175;  // Position à gauche de l'arbre
                        rectHache.x = 200;
                        if(tempsBloque == 1){
                        	startTime = SDL_GetTicks();
                        	tempsBloque = 0;
                        }
                        if(tempsRestant <= 6000){
                        	timing += 500;
                        }
                        // Ajouter une nouvelle branche en haut du tronc
                        ajouterBrancheAleatoire(branches, textureBranche);
                        score++;
                        Mix_PlayChannel(-1, SonCouperBois, 0);
                        break;
                    case SDLK_RIGHT:
                        // Déplacer le personnage à droite de l'arbre
                        rectPersonnage.x = 470;  // Position à droite de l'arbre
                        rectHache.x = 455;
                        if(tempsBloque == 1){
                        	startTime = SDL_GetTicks();
                        	tempsBloque = 0;
                        }
                        if(tempsRestant <= 6000){
                        	timing += 500;
                        }
                        // Ajouter une nouvelle branche en haut du tronc
                        ajouterBrancheAleatoire(branches, textureBranche);
                        score++;
                        Mix_PlayChannel(-1, SonCouperBois, 0);
                        break;
                    // Ajoutez d'autres cas pour d'autres touches si nécessaire
                }
                printf("%d\n",score);
                for (int i = 0; i < 5; ++i) {
                    if(branches[i].rect.y < 480){
                        branches[i].rect.y += 120;
                    }else{
                        branches[i].rect.y += 600;
                    }
                }
                for (int i = 0; i < 5; ++i) {
                    if (detecterCollision(rectPersonnage, branches[i].rect)) {
                        printf("Aie!\n");
                        tempsRestant = 3000; // Réinitialisez le temps
                        timing = 0;
                        tempsBloque = 1;
                        Jouer = afficherPopup(renderer, score);

                        verifyHighScoreFileExistence(score);
                        startTime = SDL_GetTicks();
                        score = 0;
                        // Ajoutez ici le code pour gérer la collision (par exemple, arrêter le jeu)
                    }
                }
                tapeEnCours = 1;
            }
        }

        // Effacer le renderer
        SDL_RenderClear(renderer);

        // Dessiner le fond
        SDL_Rect rectFond = { 0, 0, 800, 700 };  // Taille de la fenêtre
        SDL_RenderCopy(renderer, textureFond, NULL, &rectFond);

        // Dessiner un arbre
        SDL_Rect rectArbre = { 325, 0, 150, 600 };  // Exemple de position et de taille
        SDL_RenderCopy(renderer, textureArbre, NULL, &rectArbre);

        // Dessiner les branches
        for (int i = 0; i < 5; ++i) {
            SDL_RendererFlip flipBranche = SDL_FLIP_NONE;
            if (branches[i].rect.x == 175) {
                // Inverser l'image si le personnage regarde à gauche
                flipBranche = SDL_FLIP_HORIZONTAL;
            }
            SDL_RenderCopyEx(renderer, branches[i].texture, NULL, &branches[i].rect, 0, NULL, flipBranche);
        }

        // Dessiner le personnage
        SDL_RendererFlip flip = SDL_FLIP_NONE;
        if (rectPersonnage.x == 175) {
            // Inverser l'image si le personnage regarde à gauche
            flip = SDL_FLIP_HORIZONTAL;
        }
        // Dessiner le personnage avec la hache
        if (tapeEnCours == 1) {
        	if (rectPersonnage.x == 175) {
        		rectPersonnage.x += 25;
        	}else{
        		rectPersonnage.x -= 25;
        	}
        	SDL_RenderCopyEx(renderer, textureBuche, NULL, &rectBuche, 0, NULL, SDL_FLIP_NONE);
	        SDL_RenderCopyEx(renderer, textureHacheTape, NULL, &rectPersonnage, 0, NULL, flip);
	    	if (rectPersonnage.x == 200) {
        		rectPersonnage.x -= 25;
        	}else{
        		rectPersonnage.x += 25;
        	}
	    } else {
	        SDL_RenderCopyEx(renderer, texturePersonnage, NULL, &rectPersonnage, 0, NULL, flip);
	    }

        // Afficher le score
        TTF_Font* scoreFont = TTF_OpenFont("src/fonts/KOMIKAP_.ttf", 40);
        SDL_Color couleurTexte = { 255, 255, 255 }; // Blanc
        char scoreTexte[50];
        snprintf(scoreTexte, sizeof(scoreTexte), "%d", score);
        SDL_Surface* surfaceScore = TTF_RenderText_Solid(scoreFont, scoreTexte, couleurTexte);
        SDL_Texture* textureScore = SDL_CreateTextureFromSurface(renderer, surfaceScore);
        SDL_Rect rectScore = { 20, 15, surfaceScore->w, surfaceScore->h };
        SDL_RenderCopy(renderer, textureScore, NULL, &rectScore);


        SDL_RenderCopy(renderer, textureTimerBorder, NULL, &rectTimerBorder);
        // Dessinez la barre rouge en fonction du temps restant
	    SDL_Rect rectBarre = { 355, 15, tempsRestant / 74, 24 }; // Ajustez la taille et la position selon vos besoins
	    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Couleur rouge
	    SDL_RenderFillRect(renderer, &rectBarre);

        SDL_FreeSurface(surfaceScore);
        SDL_DestroyTexture(textureScore);
        TTF_CloseFont(scoreFont);

        // Rafraîchir l'écran
        SDL_RenderPresent(renderer);
        SDL_Delay(100);
        tapeEnCours = 0;
    }

    // Libération des ressourcesv
    Mix_FreeMusic(musiqueDeFond);
    Mix_FreeChunk(SonCouperBois);
    SDL_DestroyTexture(textureTimerBorder);
    SDL_DestroyTexture(textureBuche);
    SDL_DestroyTexture(textureHacheTape);
    SDL_DestroyTexture(texturePersonnage);
    SDL_DestroyTexture(textureArbre);
    SDL_DestroyTexture(textureBranche);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(fenetre);
    TTF_CloseFont(police);
    Mix_HaltMusic();
	Mix_CloseAudio();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}


int afficherMenu(SDL_Renderer *renderer) {
    int quit = 0;
    while(quit == 0){
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                return 2; // Quitter le jeu si la fenêtre est fermée depuis le menu
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_RETURN:
                        return 2; // Quitter le menu si la touche Entrée est enfoncée
                        break;
                    // Ajoutez d'autres cas pour d'autres touches du menu si nécessaire
                }
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Couleur noire
        SDL_RenderClear(renderer);

        // Charger l'image de fond du menu
        SDL_Texture *textureMenu = chargerTexture(renderer, "src/images/menu_backround.png");
        SDL_Rect rectMenu = {0, 0, 800, 700};  // Ajustez la taille de la fenêtre
        SDL_RenderCopy(renderer, textureMenu, NULL, &rectMenu);

        // Charger les textures des boutons
        SDL_Texture *textureBoutonStart = chargerTexture(renderer, "src/images/start.png");
        SDL_Texture *textureBoutonHighScore = chargerTexture(renderer, "src/images/highscore.png");
	SDL_Texture *textureIconeSettings = chargerTexture(renderer, "src/images/settings_icone.png");
        SDL_Texture *textureBoutonExit = chargerTexture(renderer, "src/images/exit.png");

        int largeurBouton = 202.5;
        int hauteurBouton = 72;

	int largeurIcone = 108;
        int hauteurIcone = 108;

        // Exemple : Dessiner le bouton "High Score"
        SDL_Rect rectBoutonHighScore = {300, 200, largeurBouton, hauteurBouton};  // Ajustez la position et la taille
        SDL_RenderCopy(renderer, textureBoutonHighScore, NULL, &rectBoutonHighScore);

	// Exemple : Dessiner le bouton "Settings"
        SDL_Rect rectIconeSettings = {700, 0, largeurIcone, hauteurIcone};  // Ajustez la position et la taille
        SDL_RenderCopy(renderer, textureIconeSettings, NULL, &rectIconeSettings);

        // Exemple : Dessiner le bouton "Start"
        SDL_Rect rectBoutonStart = {265, 300, 270, 96};  // Ajustez la position et la taille
        SDL_RenderCopy(renderer, textureBoutonStart, NULL, &rectBoutonStart);

        // Exemple : Dessiner le bouton "Exit"
        SDL_Rect rectBoutonExit = {300, 424, largeurBouton, hauteurBouton};  // Ajustez la position et la taille
        SDL_RenderCopy(renderer, textureBoutonExit, NULL, &rectBoutonExit);

        // Détecter les clics de souris
        SDL_PumpEvents();
        int x, y;
        Uint32 buttons = SDL_GetMouseState(&x, &y);

        // Vérifier si le clic gauche de la souris se produit sur le bouton "Start"
        if (buttons & SDL_BUTTON(SDL_BUTTON_LEFT) && x >= rectBoutonStart.x && x < rectBoutonStart.x + rectBoutonStart.w &&
            y >= rectBoutonStart.y && y < rectBoutonStart.y + rectBoutonStart.h) {
            printf("Bouton Start clique!\n");
            return 1;
        }

        if (buttons & SDL_BUTTON(SDL_BUTTON_LEFT) && x >= rectBoutonExit.x && x < rectBoutonExit.x + rectBoutonExit.w &&
            y >= rectBoutonExit.y && y < rectBoutonExit.y + rectBoutonExit.h) {
            printf("Bouton exit clique!\n");
            return 2;
        }

	// if (buttons & SDL_BUTTON(SDL_BUTTON_LEFT) && x >= rectBoutonHighScore.x && x < rectBoutonHighScore.x + rectBoutonHighScore.w &&
        //     y >= rectBoutonHighScore.y && y < rectBoutonHighScore.y + rectBoutonHighScore.h) {
        //     printf("Bouton exit clique!\n");
        //     return 3;
        // }

        // if (buttons & SDL_BUTTON(SDL_BUTTON_LEFT) && x >= rectIconeSettings.x && x < rectIconeSettings.x + rectIconeSettings.w &&
        //     y >= rectIconeSettings.y && y < rectIconeSettings.y + rectIconeSettings.h) {
        //     printf("Bouton exit clique!\n");
        //     return 4;
        // }

        SDL_RenderPresent(renderer);

        // Libérer les textures des boutons
        SDL_DestroyTexture(textureBoutonStart);
        SDL_DestroyTexture(textureBoutonHighScore);
	SDL_DestroyTexture(textureIconeSettings);
        SDL_DestroyTexture(textureBoutonExit);
        SDL_DestroyTexture(textureMenu);
    }
}
