Avant tout il faudra installer les librairies néscessaires au projet avec la commande ci-dessous depuis un terminal :

```
pacman -S mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image mingw-w64-x86_64-SDL2_ttf mingw-w64-x86_64-SDL2_mixer
```


Pour Pouvoir compiler le projet il faudra depuis la racine lancer la commande suivante dans un terminal prenant en charge les commandes gcc :

```
gcc src/main.c -o bin/prog -I include -L lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
```


Pour Pouvoir lancer le projet il faudra depuis la racine lancer la commande suivante dans un terminal :

```
./bin/prog.exe
```

Le jeu se joue à l'aide des flèches droites et gauche.
