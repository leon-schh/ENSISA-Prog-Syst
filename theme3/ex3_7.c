// Exercice 3.7 : Explications des erreurs dans les fonctions

// f1 : Le problème est que si val == 0, p pointe vers NULL, sinon vers &val.
// Mais val est une variable locale, donc quand la fonction retourne, val est détruite,
// et p pointe vers une adresse invalide. De plus, si val != 0, retourner &val est dangereux
// car val est locale à la fonction.

// f2 : La fonction retourne un pointeur vers tab, qui est un tableau local.
// Quand la fonction retourne, tab est détruit, donc le pointeur pointe vers une zone mémoire
// qui n'existe plus. De plus, strcpy(tab, "abc") écrit dans tab qui n'a que 3 caractères,
// mais "abc" fait 4 caractères avec le \0, donc débordement.

// f3 : Dans printf("%s\n", p), p est un pointeur sur char*, donc %s attend un char*,
// mais p est char**, donc cela affiche l'adresse au lieu de la chaîne.
// Il faut printf("%s\n", *p);

// f4 : La boucle for (i = 0 ; *(tab + i) ; i++) est correcte, mais la suivante
// for ( ; *tab ; tab ++) modifie tab, qui est un paramètre passé par valeur,
// donc la modification n'affecte que la copie locale, pas l'original.
// De plus, tab est modifié, donc la boucle peut ne pas fonctionner correctement.

// f5 : chaine est un pointeur vers une chaîne constante "abcdef", qui est en lecture seule.
// for (i = 0 ; i < sizeof(chaine) ; i += pas) sizeof(chaine) est sizeof(char*),
// qui est 8 sur 64 bits, donc i va de 0 à 7, et chaine[i] = '-' tente d'écrire
// dans la mémoire constante, ce qui cause une erreur de segmentation.
// De plus, strcpy(tab, chaine) copie la chaîne modifiée dans tab.