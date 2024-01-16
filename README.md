Pour pouvoir compiler il faut se rendre à la racine du projet puis tapper la commande suivante dans un terminal incluant les commandes gcc :

```
gcc src/*.c -o bin/prog -I include -L lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf
```

Pour Pouvoir lancer le projet il faudra depuis la racine lancer la commande suivante dans un terminal :

```
./bin/prog.exe
```