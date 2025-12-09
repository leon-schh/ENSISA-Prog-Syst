# Theme 7 - Système : Environnement d'un processus

Ce dossier contient tous les exercices du Thème 7 sur l'environnement d'un processus en programmation système.

## Exercices

### Exercice 7.1 - Variable environ et fonction getenv
- **Fichier** : `ex7_1.c`
- **Objectif** : Comprendre la structure de données `environ` et implémenter une fonction `getenv`
- **Description** : 
  - Dessine la structure de données de la variable globale `environ`
  - Implémente une fonction `mon_getenv` qui recherche une variable d'environnement
  - Teste la fonction avec différentes variables d'environnement

### Exercice 7.2 - Fonction system
- **Fichier** : `ex7_2.c`
- **Objectif** : Implémenter une version de la fonction `system`
- **Description** :
  - Utilise `/bin/sh` avec l'option `-c` pour exécuter les commandes
  - Gère les codes de retour et les erreurs
  - Compare avec la vraie fonction `system` de la bibliothèque

### Exercice 7.3 - Fonction isatty
- **Fichier** : `ex7_3.c`
- **Objectif** : Déterminer si un descripteur de fichier correspond à un terminal
- **Description** :
  - Implémente `mon_isatty` en utilisant `fstat` et `S_ISCHR`
  - Donne des exemples de programmes dont le comportement dépend de la redirection
  - Montre des exemples d'adaptation de sortie selon le terminal

### Exercice 7.4 - Fonction my_ttyname
- **Fichier** : `ex7_4.c`
- **Objectif** : Trouver le nom du fichier terminal correspondant à un descripteur
- **Description** :
  - Utilise `st_dev` et `st_ino` pour identifier les fichiers
  - Cherche dans `/dev`, `/dev/pts`, et `/dev/shm`
  - Compare avec la vraie fonction `ttyname`

### Exercice 7.5 - Fonction getcwd
- **Fichier** : `ex7_5.c`
- **Objectif** : Implémenter `getcwd` en utilisant la traversée des inodes
- **Description** :
  - Remonte l'arbre des répertoires en utilisant les inodes
  - Trouve le nom de chaque répertoire en cherchant dans le répertoire parent
  - S'arrête au répertoire racine (inode 2)

## Compilation

Tous les exercices peuvent être compilés avec :

```bash
gcc -o ex7_1 ex7_1.c
gcc -o ex7_2 ex7_2.c
gcc -o ex7_3 ex7_3.c
gcc -o ex7_4 ex7_4.c
gcc -o ex7_5 ex7_5.c
```

## Exécution

Pour exécuter chaque exercice :

```bash
./ex7_1
./ex7_2
./ex7_3
./ex7_4
./ex7_5
```

## Fonctionnalités testées

- **Exercice 7.1** : Variables d'environnement, gestion de chaînes, recherche dans des tableaux
- **Exercice 7.2** : Processus fils, `exec`, gestion des signaux, codes de retour
- **Exercice 7.3** : Attributs de fichiers, types de périphériques, interfaces adaptatives
- **Exercice 7.4** : Inodes, périphériques de caractères, recherche dans les répertoires
- **Exercice 7.5** : Navigation dans l'arbre des répertoires, gestion mémoire, parcours d'inodes

## Notes techniques

- Tous les exercices gèrent correctement les cas d'erreur
- Les comparaisons avec les fonctions de bibliothèque standard permettent de vérifier l'exactitude
- Les exemples d'utilisation montrent les cas pratiques d'application
- La gestion mémoire est appropriée (libération des ressources allouées)