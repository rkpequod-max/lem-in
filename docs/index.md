# Lem-in - Documentation Officielle

## 🐜 Bienvenue sur la documentation du projet Lem-in

Ce site documente le projet **Lem-in** de l'école 42, un algorithme de résolution de parcours de fourmis dans une ferme.

---

## 📖 Navigation

- **[Documentation Technique Complète](LEM_IN_EXPLICATIF.md)** - Guide détaillé de toutes les structures et fonctions
- **[Code Source](https://github.com/rkpequod-max/lem-in)** - Dépôt GitHub du projet

---

## 🎯 Aperçu du Projet

**Lem-in** est un projet qui consiste à faire passer un nombre donné de fourmis d'un point de départ à un point d'arrivée dans le **minimum de tours**, en évitant les collisions.

### Règles Principes

- Une seule fourmi par salle à la fois
- Les fourmis se déplacent simultanément
- Les liens sont bidirectionnels
- Objectif : minimiser le nombre de tours totaux

### Concepts Clés

- **Algorithme BFS** (Breadth-First Search) pour trouver les chemins
- **Gestion de flux** (Flow Network) pour optimiser le mouvement
- **Évitation de collisions** par synchronisation des mouvements
- **Optimisation** du nombre total de tours via programmation linéaire simplifiée

---

## 🚀 Utilisation Rapide

```bash
# Compiler le projet
make

# Exécuter avec un fichier d'entrée
./lem-in < input.txt

# Mode debug basique
./lem-in --d < input.txt

# Mode debug avancé (chemins détaillés)
./lem-in --dp < input.txt
```

---

## 📁 Structure du Projet

```
lem-in/
├── include/           # En-têtes avec structures et prototypes
│   ├── lem_in.h       # Header principal
│   ├── farm.h         # Structures de la ferme
│   ├── graph.h        # Structures du graphe
│   └── solve.h        # Structures de résolution
├── src/
│   ├── lem_in.c       # Point d'entrée (main)
│   ├── farm/          # Parsing et validation (FSM)
│   ├── graph/         # Gestion des salles et liens
│   └── solve/         # Algorithmes BFS et flux de fourmis
├── libft/             # Bibliothèque utilitaire personnalisée
└── docs/              # Documentation GitHub Pages (ce site)
```

---

## 📦 Structures de Données Principales

### t_room - Salle (Noeud du Graphe)
```c
typedef struct s_room {
    char *name;           // Nom de la salle (ex: "1", "A", "start")
    t_coord c;            // Coordonnées (x, y)
    int ant;              // ID de la fourmi présente (0 = vide)
    int start;            // 1 = point de départ
    int end;              // 1 = point d'arrivée
    t_list *lnks;         // Liste des voisins connectés
    struct s_room *f_to;  // Salle suivante dans le flux (forward)
    struct s_room *f_fm;  // Salle précédente dans le flux (from)
    struct s_room *next;  // Salle suivante dans la liste globale
} t_room;
```

### t_bfs - Noeud de Parcours BFS
```c
typedef struct s_bfs {
    int path_size;      // Distance depuis le départ
    int send_ants;      // Fourmis à envoyer sur ce chemin
    int min_ants;       // Configuration optimale calculée
    t_room **room;      // Double pointeur vers la salle
    struct s_bfs *prev; // Parent dans l'arbre BFS (backtracking)
    struct s_bfs *next; // Frère dans la file BFS
} t_bfs;
```

---

## 🔍 Algorithmes Détaillés

### 1. Parsing (Finite State Machine)

| État | Description | Transition |
|------|-------------|------------|
| 0 | Lecture du nombre de fourmis | → 1 après première salle valide |
| 1 | Lecture des salles | → 2 après premier lien valide |
| 2 | Lecture des liens | Fin quand EOF |

**Commandes spéciales :**
- `##start` : Marque la prochaine salle comme départ
- `##end` : Marque la prochaine salle comme arrivée
- `#...` : Commentaires ignorés

### 2. BFS (Breadth-First Search)

```
1. Initialiser la file avec le noeud START
2. Tant que la file n'est pas vide :
   a. Défiler le noeud courant
   b. Pour chaque voisin non visité :
      - Vérifier les contraintes de flux (f_to/f_fm)
      - Si voisin == END → Chemin trouvé !
      - Sinon → Enfiler avec path_size + 1
3. Backtracking via 'prev' pour reconstruire le chemin
4. Marquer le chemin avec f_to/f_fm
```

### 3. Gestion du Flux (Flow Network)

Après chaque BFS réussi, la fonction `update_flow()` marque le chemin :

```c
// Backtracking depuis END vers START
current->f_fm = previous;  // Marque le retour
previous->f_to = current;  // Marque l'avant
```

Cela crée des **autoroutes virtuelles** où les fourmis circulent sans collision.

### 4. Optimisation du Nombre de Tours

**Formule clé :** `tours = longest_path_length + num_ants - 1`

L'algorithme `path_config()` teste toutes les combinaisons de chemins pour trouver le minimum global.

**Stratégie :**
- Chemins courts reçoivent plus de fourmis
- Chemins longs reçoivent moins de fourmis
- Équilibre pour minimiser le temps total

---

## 📝 Exemple Complet

### Input
```
4

##start
1 0 0
2 1 0
3 0 1
4 1 1
##end
5 2 0

1-2
1-3
2-5
3-5
```

### Output
```
4

##start
1 0 0
2 1 0
3 0 1
4 1 1
##end
5 2 0

1-2
1-3
2-5
3-5

L1-2 L2-3
L1-5 L2-5
L3-2 L4-3
L3-5 L4-5
```

**Analyse :**
- Tour 1 : 2 fourmis partent (L1→2, L2→3)
- Tour 2 : 2 fourmis arrivent + 2 nouvelles partent
- Tour 3 : 2 dernières fourmis arrivent
- **Total : 3 tours** ✅

---

## ⚠️ Pièges Courants et Solutions

| Erreur | Symptôme | Solution |
|--------|----------|----------|
| Fuites mémoire | Valgrind rapporte des fuites | Libérer chaque `malloc` avec `destruct_*` |
| Collisions de fourmis | Sortie invalide | Vérifier `room->ant == 0` avant déplacement |
| Multiple start/end | Segfault ou comportement erratique | Valider exactement un de chaque avec `endpoints()` |
| Overflow int | Comportement imprévisible | Utiliser `exceeds_int()` pour valider les nombres |
| Liens duplicates | Chemins incorrects | Tester avec `is_linked()` avant `add_link()` |
| Commentaires mal gérés | Parsing échoue | Ignorer toute ligne commençant par `#` sauf `##start`/`##end` |

---

## 🛠️ Outils Recommandés

### Debug et Validation
- **Valgrind** : `valgrind --leak-check=full ./lem-in < input.txt`
- **GDB/LLDB** : Debug pas à pas avec breakpoints
- **Norminette** : `norminette src/ include/` pour respecter la norme 42

### Visualisation
- [42-lem-in-pusher](https://github.com/McNuggetsFr/42-lem-in-pusher) - Visualiseur graphique
- [lem-in-gui](https://github.com/Glagan/lem-in-gui) - Autre visualiseur populaire

### Générateurs de Cartes
- `generator_map` (inclus dans le projet)
- `push_swap_generator` adapté pour lem-in

---

## 📊 Complexité et Performance

| Opération | Complexité Temporelle | Complexité Spatiale |
|-----------|----------------------|---------------------|
| Parsing | O(n + m) | O(n + m) |
| BFS unique | O(n + m) | O(n) |
| Découverte de k chemins | O(k × (n + m)) | O(k × n) |
| Simulation de flux | O(tours × n) | O(n) |
| **Total** | **O(k × (n + m) + tours × n)** | **O(n + m)** |

Où :
- n = nombre de salles
- m = nombre de liens
- k = nombre de chemins découverts
- tours = nombre de tours de simulation

---

## 🔗 Liens Utiles

- [Sujet Officiel 42](https://cdn.intra.42.fr/pdf/pdf/63124/fr.subject.pdf)
- [Dépôt GitHub](https://github.com/rkpequod-max/lem-in)
- [Documentation Technique Détaillée](LEM_IN_EXPLICATIF.md)
- [Wikipedia - BFS Algorithm](https://en.wikipedia.org/wiki/Breadth-first_search)
- [Wikipedia - Flow Network](https://en.wikipedia.org/wiki/Flow_network)

---

*Documentation générée automatiquement via GitHub Actions - Projet Lem-in © 42 School*
