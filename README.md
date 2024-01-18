Pour Pouvoir compiler le projet il faudra depuis la racine lancer la commande suivante dans un terminal prenant en charge les commandes gcc :

```
gcc src/*.c -o bin/prog -I include -L lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
```

Pour Pouvoir lancer le projet il faudra depuis la racine lancer la commande suivante dans un terminal :

```
./bin/prog.exe
```
