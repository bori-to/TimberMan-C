/*
    GNU/Linux et MacOS
        > gcc src/*.c -o bin/prog $(sdl2-config --cflags --libs)
    Windows
        > gcc src/*.c -o bin/prog -I include -L lib -lmingw32 -lSDL2main -lSDL2
*/

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <SDL_mouse.h>

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

void readConfigFile(char* theme, double* difficulty, double* speed) {

    // Constantes pour les valeurs par défaut et les limites
    const char defaultTheme[] = "default";
    const double defaultDifficulty = 1.0, minDifficulty = 0.10, maxDifficulty = 10.0;
    const double defaultSpeed = 1.0, minSpeed = 0.10, maxSpeed = 10.0;
    const int maxKeyLength = 20, maxValueLength = 40;

    // Initialisation avec des valeurs par défaut
    strncpy(theme, defaultTheme, sizeof(defaultTheme));  
    *difficulty = defaultDifficulty;
    *speed = defaultSpeed;

    FILE* f = fopen("timberman.config", "r");

    if (f == NULL) {

        printf("Erreur lors de l'ouverture du fichier de configuration : %s\n", strerror(errno));

        return;

    }

    char key[maxKeyLength + 1], value[maxValueLength + 1];

    while (fscanf(f, "%20s %40s", key, value) == 2) {  // Modification ici pour vérifier le succès de fscanf

        if (strncmp(key, "theme", 5) == 0) {

            strncpy(theme, value, maxValueLength);  // Sécuriser la copie pour éviter le dépassement de tampon
            theme[maxValueLength] = '\0';  // S'assurer que la chaîne est terminée correctement

        } else if (strncmp(key, "difficulty", 10) == 0) {

            *difficulty = atof(value);

            if (*difficulty < minDifficulty || *difficulty > maxDifficulty) {

                printf("Erreur: La difficulté doit être comprise entre %.2f et %.2f ! Passage à la valeure par défaut (%.2f)\n", minDifficulty, maxDifficulty, defaultDifficulty);

                *difficulty = defaultDifficulty;

            }

        } else if (strncmp(key, "speed", 5) == 0) {

            *speed = atof(value);

            if (*speed < minSpeed || *speed > maxSpeed) {

                printf("Erreur: La vitesse doit être comprise entre %.2f et %.2f ! Passage à la valeure par défaut (%.2f)\n", minSpeed, maxSpeed, defaultSpeed);

                *speed = defaultSpeed;

            }

        }

    }

    fclose(f);

}

void verifyConfigFileExistence(const char* fichierConfig) {

    FILE* f = fopen(fichierConfig, "r");

    if (f == NULL) {

        f = fopen(fichierConfig, "w");

        if (f == NULL) {

            printf("Erreur lors de la création du fichier de configuration : %s\n", strerror(errno));

            return;

        }

        fprintf(f, "theme = default\n");
        fprintf(f, "difficulty = 1.00\n");
        fprintf(f, "speed = 1.00\n");

        fclose(f);

    } else {

        fclose(f);

    }

}


int main(int argc, char *argv[]) {

    char theme[100] = "default";
    double difficulty = 1.00;
    double speed = 1.00;

    verifyConfigFileExistence("timberman.config");
    readConfigFile(theme, &difficulty, &speed);

    // Utiliser les valeurs lues pour modifier les paramètres de votre jeu

    printf("Thème : %s\n", theme);
    printf("Coéfficient de difficulté : %.2f\n", difficulty);
    printf("Coéfficient d'accélération de la vitesse : %.2f\n", speed);


    
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

    // Créer la fenêtre
    SDL_Window *fenetre = SDL_CreateWindow("TimberMan", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 700, SDL_WINDOW_SHOWN);
    if (fenetre == NULL) {
        fprintf(stderr, "Erreur lors de la création de la fenêtre : %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Créer le renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(fenetre, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        fprintf(stderr, "Erreur lors de la création du renderer : %s\n", SDL_GetError());
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

    // Charger les textures
    SDL_Texture *textureFond = chargerTexture(renderer, "src/images/background.png");
    SDL_Texture *texturePersonnage = chargerTexture(renderer, "src/images/player.png");
    SDL_Texture* textureHache = chargerTexture(renderer, "src/images/axe.png");
    SDL_Texture *textureArbre = chargerTexture(renderer, "src/images/tree.png");
    SDL_Texture* textureBranche = chargerTexture(renderer, "src/images/branch.png");


    SDL_Rect rectPersonnage = { 485, 440, 150, 150 };
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
    int menuActif = 1;

    while (menuActif) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                menuActif = 0; // Quitter le jeu si la fenêtre est fermée depuis le menu
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_RETURN:
                        menuActif = 0; // Quitter le menu si la touche Entrée est enfoncée
                        break;
                    // Ajoutez d'autres cas pour d'autres touches du menu si nécessaire
                }
            }
        }

        // Effacer le renderer
        SDL_RenderClear(renderer);

        // Afficher le menu
        Jouer = afficherMenu(renderer);
        if(Jouer != 0){
            menuActif = 0;
        }
        // Rafraîchir l'écran
        SDL_RenderPresent(renderer);
    }
    
    // Boucle principale du jeu
    SDL_Event event;
    srand(time(NULL));
    while (Jouer == 1) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                Jouer = 0;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        // Déplacer le personnage à gauche de l'arbre
                        rectPersonnage.x = 165;  // Position à gauche de l'arbre
                        rectHache.x = 200;
                        // Ajouter une nouvelle branche en haut du tronc
                        ajouterBrancheAleatoire(branches, textureBranche);
                        break;
                    case SDLK_RIGHT:
                        // Déplacer le personnage à droite de l'arbre
                        rectPersonnage.x = 485;  // Position à droite de l'arbre
                        rectHache.x = 455;
                        // Ajouter une nouvelle branche en haut du tronc
                        ajouterBrancheAleatoire(branches, textureBranche);
                        break;
                    // Ajoutez d'autres cas pour d'autres touches si nécessaire
                }
                for (int i = 0; i < 5; ++i) {
                    branches[i].rect.y += 120;
                }
            }
        }

        for (int i = 0; i < 5; ++i) {
            if (detecterCollision(rectPersonnage, branches[i].rect)) {
                printf("Touché!\n");
                // Ajoutez ici le code pour gérer la collision (par exemple, arrêter le jeu)
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
        if (rectPersonnage.x == 165) {
            // Inverser l'image si le personnage regarde à gauche
            flip = SDL_FLIP_HORIZONTAL;
        }
        // Dessiner le personnage avec la hache
        SDL_RenderCopyEx(renderer, texturePersonnage, NULL, &rectPersonnage, 0, NULL, flip);

        // Dessiner la hache dans la main du personnage
        SDL_Rect rectHache = { rectPersonnage.x, rectPersonnage.y + 70, 152, 28 };  // Position et taille de la hache (ajustez selon vos besoins)
        SDL_RenderCopyEx(renderer, textureHache, NULL, &rectHache, 0, NULL, flip);

        // Rafraîchir l'écran
        SDL_RenderPresent(renderer);
    }

    // Libération des ressources
    SDL_DestroyTexture(textureHache);
    SDL_DestroyTexture(texturePersonnage);
    SDL_DestroyTexture(textureArbre);
    SDL_DestroyTexture(textureBranche);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(fenetre);
    TTF_CloseFont(police);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}


int afficherMenu(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Couleur noire
    SDL_RenderClear(renderer);

    // Charger l'image de fond du menu
    SDL_Texture *textureMenu = chargerTexture(renderer, "src/images/menu_backround.png");
    SDL_Rect rectMenu = {0, 0, 800, 700};  // Ajustez la taille de la fenêtre
    SDL_RenderCopy(renderer, textureMenu, NULL, &rectMenu);

    // Charger les textures des boutons
    SDL_Texture *textureBoutonStart = chargerTexture(renderer, "src/images/start.png");
    SDL_Texture *textureBoutonHighScore = chargerTexture(renderer, "src/images/highscore.png");
    SDL_Texture *textureBoutonExit = chargerTexture(renderer, "src/images/exit.png");

    int largeurBouton = 202.5;
    int hauteurBouton = 72;

    // Exemple : Dessiner le bouton "High Score"
    SDL_Rect rectBoutonHighScore = {300, 200, largeurBouton, hauteurBouton};  // Ajustez la position et la taille
    SDL_RenderCopy(renderer, textureBoutonHighScore, NULL, &rectBoutonHighScore);

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

    int jouer = 0;
    // Vérifier si le clic gauche de la souris se produit sur le bouton "Start"
    if (buttons & SDL_BUTTON(SDL_BUTTON_LEFT) && x >= rectBoutonStart.x && x < rectBoutonStart.x + rectBoutonStart.w &&
        y >= rectBoutonStart.y && y < rectBoutonStart.y + rectBoutonStart.h) {
        printf("Bouton Start cliqué!\n");
        jouer = 1;
    }

    if (buttons & SDL_BUTTON(SDL_BUTTON_LEFT) && x >= rectBoutonExit.x && x < rectBoutonExit.x + rectBoutonExit.w &&
        y >= rectBoutonExit.y && y < rectBoutonExit.y + rectBoutonExit.h) {
        printf("Bouton exit cliqué!\n");
        jouer = 2;
    }

    return jouer;
    SDL_RenderPresent(renderer);

    // Libérer les textures des boutons
    SDL_DestroyTexture(textureBoutonStart);
    SDL_DestroyTexture(textureBoutonHighScore);
    SDL_DestroyTexture(textureBoutonExit);

    SDL_DestroyTexture(textureMenu);
}
