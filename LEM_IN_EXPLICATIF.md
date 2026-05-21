# Explicatif Complet du Projet Lem-in

## 📋 **VUE D'ENSEMBLE DU PROJET**

**Lem-in** est un projet de la 42 School qui consiste à faire passer un nombre donné de fourmis d'un point de départ à un point d'arrivée dans le minimum de tours, en évitant les collisions. Le projet utilise un algorithme de **BFS (Breadth-First Search)** combiné à une gestion de **flux de fourmis**.

### Architecture du projet :
```
lem-in/
├── include/           # En-têtes avec structures et prototypes
│   ├── lem_in.h       # Header principal
│   ├── farm.h         # Structures de la ferme (fourmis, salles)
│   ├── graph.h        # Structures du graphe (salles, liens)
│   └── solve.h        # Structures de résolution (BFS, chemins)
├── src/
│   ├── lem_in.c       # Point d'entrée (main)
│   ├── farm/          # Gestion de l'input et validation
│   ├── graph/         # Gestion des salles et liens
│   └── solve/         # Algorithmes de résolution
└── libft/             # Bibliothèque de fonctions utilitaires
```

---

## 🏗️ **STRUCTURES DE DONNÉES**

### 1. **t_coord** (`graph.h`)
```c
typedef struct s_coord {
    int x;
    int y;
} t_coord;
```
**Purpose** : Représente les coordonnées 2D d'une salle dans la ferme.

---

### 2. **t_room** (`graph.h`) - Structure Centrale
```c
typedef struct s_room {
    char *name;           // Nom de la salle (ex: "1", "A", "start")
    t_coord c;            // Coordonnées (x, y)
    int ant;              // ID de la fourmi actuellement dans la salle (0 si vide)
    int start;            // 1 si c'est le point de départ, 0 sinon
    int end;              // 1 si c'est le point d'arrivée, 0 sinon
    t_list *lnks;         // Liste chaînée des salles connectées (voisins)
    struct s_room *f_to;  // Salle suivante dans le flux de fourmis (forward)
    struct s_room *f_fm;  // Salle précédente dans le flux (from)
    struct s_room *next;  // Salle suivante dans la liste chaînée globale
} t_room;
```

**Champs critiques pour la résolution :**
- `f_to` / `f_fm` : Créent un "tunnel" virtuel pour le flux de fourmis après BFS
- `ant` : Permet de suivre quelle fourmi est où à chaque tour
- `lnks` : Graphe d'adjacence pour le parcours BFS

---

### 3. **t_farm** (`farm.h`)
```c
typedef struct s_farm {
    int ants;      // Nombre total de fourmis à déplacer
    t_room *rms;   // Pointeur vers la première salle (liste chaînée)
} t_farm;
```
**Purpose** : Conteneur principal représentant toute la ferme.

---

### 4. **t_bfs** (`solve.h`) - Pour la Résolution
```c
typedef struct s_bfs {
    int path_size;      // Longueur du chemin depuis le départ
    int send_ants;      // Nombre de fourmis à envoyer sur ce chemin
    int min_ants;       // Nombre optimal de fourmis calculé
    t_room **room;      // Pointeur vers le pointeur de salle (double indirection)
    struct s_bfs *prev; // Noeud précédent dans le chemin BFS
    struct s_bfs *next; // Noeud suivant dans la liste BFS
} t_bfs;
```

**Note importante** : `room` est un `t_room **` (double pointeur) pour pouvoir modifier les références lors du backtracking.

---

### 5. **t_list** (`libft.h`) - Liste Chaînée Générique
```c
typedef struct s_list {
    void *obj;          // Pointeur vers l'objet stocké
    size_t dim;         // Taille de l'objet
    struct s_list *next;
} t_list;
```
Utilisée pour stocker les liens entre salles dans `t_room.lnks`.

---

## 🔍 **FONCTIONS DÉTAILLÉES PAR MODULE**

---

### **MODULE 1 : PARSING ET VALIDATION** (`src/farm/` + `src/lem_in.c`)

#### **Fonction `main()`** (`src/lem_in.c`)
```c
int main(int ac, char **av)
```
**Flux :**
1. Parse les flags `--d` (debug basique) ou `--dp` (debug avancé)
2. Initialise la ferme avec `init_farm()`
3. Appelle `fill_farm()` pour parser l'input
4. Valide qu'il y a des salles
5. Lance `solve()` pour résoudre le problème
6. Affiche les stats en mode debug
7. Nettoie la mémoire avec `destruct_farm()`

---

#### **Fonction `fill_farm()`** (`src/lem_in.c`)
```c
int fill_farm(t_farm **farm, int section, int io)
```
**Purpose** : Parser ligne par ligne l'input stdin.

**Variables statiques importantes :**
- `pitcher` : Buffer pour `get_next_line2()` (lecture ligne par ligne personnalisée)

**Sections du parsing :**
- `section = 0` : Lecture du nombre de fourmis
- `section = 1` : Lecture des salles (nom x y)
- `section = 2` : Lecture des liens (room1-room2)

**Gestion des commandes spéciales :**
- `##start` : Marque la prochaine salle comme départ (`io = 0`)
- `##end` : Marque la prochaine salle comme arrivée (`io = 1`)

---

#### **Fonction `is_cmd()`** (`src/lem_in.c`)
```c
int is_cmd(char *cmd, int section, t_farm **farm, int *io)
```
**Purpose** : Détermine le type de ligne et délègue la validation.

**Logique :**
1. Si ligne commence par `#` mais n'est pas `##start`/`##end` → ignorée (commentaire)
2. Si `section == 0` et ligne est numérique → `add_ants()`
3. Si `section == 0` ou `1` → tente de parser comme salle avec `is_room()`
4. Si `section == 2` → tente de parser comme lien avec `is_link()`

---

#### **Fonction `is_num()`** (`src/farm/valid_input.c`)
```c
int is_num(char *str)
```
**Validation :**
- Tous les caractères doivent être des digits
- Ne doit pas overflow un `int` (vérifié par `exceeds_int()`)
- String non vide

---

#### **Fonction `is_room()`** (`src/farm/valid_input.c`)
```c
int is_room(t_room **rms, char **rooms, int *io)
```
**Validation d'une salle :**
1. Format : `nom x y` (3 tokens)
2. Nom ne doit pas commencer par `L` ou `#`
3. x et y doivent être numériques
4. Nom unique (vérifié par `is_name()`)
5. Coordonnées uniques (vérifié par `is_coords()`)
6. Si valide → `push_room()` pour l'ajouter

**Retour :**
- `1` : Succès
- `0` : Échec de parsing
- `-1` : Erreur (duplicate)

---

#### **Fonction `is_link()`** (`src/farm/valid_input.c`)
```c
int is_link(t_farm **farm, char *str, int io)
```
**Validation d'un lien :**
1. Doit être dans `section == 2` (`io == -1`)
2. Format exact : `room1-room2` (un seul `-`)
3. Deux noms de salles différentes
4. Les deux salles doivent exister (`is_name()`)
5. Si valide → `add_link()` pour créer la connexion bidirectionnelle

---

#### **Fonction `repeated_endpoints()`** (`src/farm/valid_input.c`)
```c
int repeated_endpoints(int *start, int *end, int io, int section)
```
**Purpose** : Vérifie qu'il y a exactement UN départ et UNE arrivée.

**Logique :**
- Si `##start` rencontré plusieurs fois → erreur
- Si `##end` rencontré plusieurs fois → erreur
- Met à jour les flags `*start` et `*end`

---

#### **Fonctions utilitaires de `farm_treatment.c` :**

| Fonction | Purpose |
|----------|---------|
| `init_farm()` | Alloue et initialise `t_farm` avec 0 fourmis et NULL rooms |
| `add_ants()` | Définit le nombre de fourmis |
| `destruct_farm()` | Libère toutes les salles puis la ferme |
| `print_farm()` | Debug : affiche toutes les infos de la ferme |
| `coord()` | Convertit deux strings en `t_coord` |

---

### **MODULE 2 : GESTION DU GRAPHE** (`src/graph/`)

#### **Fonction `push_room()`** (`src/graph/graph_treatment.c`)
```c
void push_room(t_room **room1, char *name, t_coord c, int io)
```
**Purpose** : Ajoute une nouvelle salle en tête de liste chaînée.

**Processus :**
1. `init_room()` pour allouer et initialiser
2. Insère en tête : `new->next = *room1; *room1 = new;`

---

#### **Fonction `init_room()`** (`src/graph/graph_treatment.c`)
```c
void init_room(t_room **room, char *name, t_coord c, int io)
```
**Initialisation :**
- `name` : dupliquée avec `ft_strdup()`
- `c` : coordonnées copiées
- `start/end` : mis à 1 si `io == 0` ou `io == 1`
- `lnks`, `f_to`, `f_fm`, `next` : tous à NULL

---

#### **Fonction `pop_room()`** et `destruct_room()`
```c
void pop_room(t_room **room)
void destruct_room(t_room **room)
```
**Purpose** : Supprime une ou toutes les salles.

**Nettoyage complet :**
1. Détruit tous les liens avec `destruct_link()`
2. Libère `name`
3. Libère la structure

---

#### **Fonction `add_link()`** (`src/graph/list.c`)
```c
void add_link(t_room **room1, char *name1, char *name2)
```
**Purpose** : Crée un lien BIDIRECTIONNEL entre deux salles.

**Processus :**
1. Trouve les deux salles avec `find_name()`
2. Vérifie que le lien n'existe pas déjà (`is_linked()`)
3. Ajoute `room2` dans la liste `lnks` de `room1`
4. Ajoute `room1` dans la liste `lnks` de `room2`

**Important** : Utilise `ft_lstnew2()` avec un `t_room **` pour stocker un pointeur vers le pointeur de salle.

---

#### **Fonction `is_linked()`** (`src/graph/list.c`)
```c
int is_linked(t_list *link, t_room *room)
```
**Purpose** : Vérifie si une salle est déjà dans la liste de liens.

---

#### **Fonctions de recherche :**

| Fonction | Prototype | Purpose |
|----------|-----------|---------|
| `find_name()` | `t_room **find_name(t_room **room1, char *name)` | Retourne `&room` correspondant au nom |
| `find_start()` | `t_room **find_start(t_room **room1)` | Retourne `&room` avec `start == 1` |
| `find_end()` | `t_room **find_end(t_room **room1)` | Retourne `&room` avec `end == 1` |
| `is_name()` | `int is_name(t_room *room1, char *name)` | Vérifie si un nom existe |
| `is_coords()` | `int is_coords(t_room *room1, char *x, char *y)` | Vérifie si des coords existent |

---

### **MODULE 3 : RÉSOLUTION - BFS** (`src/solve/`)

#### **Fonction `solve()`** (`src/solve/solve.c`) - Point d'Entrée
```c
int solve(t_farm **farm, int debug)
```
**Algorithme global :**

1. **Validation** : Vérifie qu'il y a exactement un start et un end (`endpoints()`)

2. **Boucle de découverte de chemins** (`while (apath)`):
   - Phase 1 (`apath == 2`) : Premier BFS pour trouver le chemin le plus court
   - Phase 2 (`apath == 1`) : Deuxième BFS pour trouver un deuxième chemin disjoint
   
3. **Mise à jour du flux** : `update_flow()` marque les tunnels `f_to`/`f_fm`

4. **Simulation** : `send_ants()` fait circuler les fourmis

---

#### **Fonction `bfs()`** (`src/solve/bfs.c`) - Coeur de l'Algorithme
```c
int bfs(t_bfs **p, t_bfs **current)
```
**Purpose** : Parcours en largeur pour trouver un chemin de start à end.

**Mécanisme :**
1. Part du noeud `current` (initialement start)
2. Pour chaque voisin (`lnks`) :
   - Ignore si déjà visité (`room_in()`)
   - Ignore si c'est un "contre-flux" (`flow_to()`)
   - Ignore certaines conditions de backtracking (`undo`)
   - Insère dans la file BFS avec `insert_bfs()`
   - **Si voisin est end → chemin trouvé ! Retourne 1**

3. Récursivement traite le noeud suivant dans la file

**Conditions de rejet (`flow_to()`) :**
- Si on vient de `start` : ignore si `to->f_fm != from`
- Sinon : ignore si `from->f_to != to`
- Cela empêche d'utiliser un tunnel dans le sens inverse

---

#### **Fonction `insert_bfs()`** (`src/solve/bfs_treatment.c`)
```c
void insert_bfs(t_bfs **p, t_room **room, t_bfs *prev)
```
**Purpose** : Ajoute un noeud à la file BFS (en queue, pour BFS standard).

---

#### **Fonction `insertio()`** (`src/solve/bfs_treatment.c`)
```c
void insertio(t_bfs **p, t_room **room, t_bfs *prev, int p_size)
```
**Purpose** : Insertion TRIÉE par `path_size` pour la phase de simulation.

**Utilisation** : Garde les chemins les plus courts en tête pour optimiser l'envoi des fourmis.

---

#### **Fonction `update_flow()`** (`src/solve/flow.c`)
```c
void update_flow(t_bfs **p)
```
**Purpose CRITIQUE** : Après un BFS réussi, marque le chemin trouvé avec `f_to`/`f_fm`.

**Processus (backtracking depuis end) :**
1. Trouve le noeud `end` dans la chaîne BFS
2. Remonte via `prev` jusqu'à `start`
3. Pour chaque paire (prv, r) :
   - `r->f_fm = prv` (marque le retour)
   - `prv->f_to = r` (marque l'avant)
   
4. Gère les conflits : si un tunnel existait dans l'autre sens, l'annule

**Résultat** : Crée des "autoroutes" virtuelles où les fourmis peuvent circuler sans collision.

---

#### **Fonction `endpoints()`** (`src/solve/valid_solution.c`)
```c
int endpoints(t_room *room1)
```
**Validation** : Compte les start/end, doit avoir exactement 1 de chaque.

---

### **MODULE 4 : SIMULATION DU FLUX DE FOURMIS** (`src/solve/flow.c` + `path_computation.c`)

#### **Fonction `send_ants()`** (`src/solve/flow.c`)
```c
void send_ants(t_farm **farm, t_room **start, int debug, int turns)
```
**Purpose** : Simule tour par tour le déplacement des fourmis.

**Préparation :**
1. `path_list()` : Extrait tous les chemins valides depuis les voisins de start
2. `path_config()` : Calcule le nombre optimal de fourmis par chemin

**Boucle principale (`while ((*end)->ant < ants)`):**
1. `move_ants()` : Déplace toutes les fourmis déjà en circulation
2. `select_paths()` : Injecte de nouvelles fourmis depuis start
3. Incrémente le compteur de tours
4. Affiche l'état (chaque fourmi déplacée est printée)

**Condition de fin :**
- Toutes les fourmis sont arrivées à `end`
- OU aucun mouvement possible (`!entro`)

---

#### **Fonction `move_ants()`** (`src/solve/flow.c`)
```c
int move_ants(t_bfs **paths)
```
**Purpose** : Fait avancer TOUTES les fourmis sur TOUS les chemins.

**Appelle `push_ants()` pour chaque chemin.**

---

#### **Fonction `push_ants()`** (`src/solve/flow.c`)
```c
int push_ants(t_room **path, int entro)
```
**Purpose** : Fait avancer les fourmis sur UN chemin spécifique.

**Mécanisme (de start vers end) :**
```
Tour N:   Fourmi A est dans room[i]
Tour N+1: Fourmi A sera dans room[i+1] (via f_to)
```

**Algorithme :**
1. Parcours le chemin via `f_to`
2. Pour chaque salle :
   - Sauvegarde `ant` actuel (`aux_ant`)
   - Met `ant = prev_ant` (la fourmi précédente prend cette place)
   - `prev_ant = aux_ant` (pour la prochaine itération)
3. Si une fourmi arrive à `end` → incrémente le compteur de `end`

**Astuce** : Le décalage se fait de l'arrière vers l'avant implicitement grâce à la sauvegarde temporaire.

---

#### **Fonction `select_paths()`** (`src/solve/flow.c`)
```c
int select_paths(t_bfs **paths, t_farm **farm, int ants)
```
**Purpose** : Injecte de NOUVELLES fourmis depuis le point de départ.

**Condition d'injection :**
- Il reste des fourmis à envoyer (`(*farm)->ants > 0`)
- Le chemin a encore du "crédit" (`min_ants > 0`)

**Action :**
- Décrémente `min_ants` du chemin
- Place une nouvelle fourmi dans la première salle du chemin
- Décrémente le compteur global

---

#### **Fonction `path_config()`** (`src/solve/path_computation.c`)
```c
void path_config(t_bfs **path, int ants)
```
**Purpose OPTIMISATION CRITIQUE** : Calcule combien de fourmis envoyer sur chaque chemin pour minimiser le nombre total de tours.

**Algorithme :**
1. Pour chaque sous-ensemble de chemins (de 1 à N chemins) :
   - `condemn_path()` : Calcule le nombre de tours nécessaire
   - Garde la configuration qui donne le minimum de tours

2. Stocke le résultat dans `min_ants` de chaque noeud

---

#### **Fonction `condemn_path()`** (`src/solve/path_computation.c`)
```c
int condemn_path(t_bfs **path, t_bfs **last, int ants)
```
**Calcule** : Nombre de tours = `population + longest_path_size`

Où `population` est le nombre de fourmis nécessaires pour remplir tous les chemins sélectionnés.

---

#### **Fonction `populate_path()`** (`src/solve/path_computation.c`)
```c
int populate_path(int first, t_bfs **paths, t_bfs **actual)
```
**Calcule** : Combien de fourmis peuvent être envoyées en parallèle sur les chemins, compte tenu de leurs longueurs différentes.

**Formule clé :**
```c
nxt->send_ants = aux->send_ants - abs(nxt->path_size - aux->path_size);
```
Les chemins plus longs reçoivent moins de fourmis car ils "occupent" le pipeline plus longtemps.

---

### **MODULE 5 : AFFICHAGE ET DEBUG**

#### **Fonctions d'affichage :**

| Fonction | Purpose |
|----------|---------|
| `print_bfs()` | Affiche un chemin BFS trouvé (de end vers start) |
| `print_paths()` | Affiche tous les chemins extraits pour la simulation |
| `print_path()` | Affiche un chemin individuel (noms des salles) |
| `print_ant()` | Affiche le mouvement d'une fourmi : `L{id}-{room}` |
| `print_room()` | Debug : affiche toutes les salles |
| `print_link()` | Debug : affiche les liens d'une salle |

**Format de sortie standard :**
```
<map originale>

L1-3 L2-5
L1-4 L2-end
...
```

---

### **MODULE 6 : BIBLIOTHÈQUE LIBFT**

Le projet utilise une bibliothèque personnalisée étendue. Fonctions clés utilisées :

#### **Gestion de mémoire :**
- `ft_memalloc()`, `ft_memdel()`
- `ft_strdup()`, `ft_strndup()`
- `malloc()`, `free()` directs

#### **Manipulation de strings :**
- `ft_strsplit()` : Split une string par délimiteur
- `ft_strcmp()`, `ft_strncmp()`
- `ft_strlen()`, `ft_strchr()`
- `ft_itoa()` : int → string

#### **Listes chaînées :**
- `ft_lstnew2()` : Crée un nouveau noeud (version custom)
- `ft_lstadd()` : Ajoute en tête de liste
- `ft_lstiter()` : Itère sur la liste

#### **Affichage :**
- `ft_printf()` : Printf personnalisé
- `ft_putstr()`, `ft_putendl()`, `ft_putnbr()`
- `ft_puterr()` : Affiche erreur sur stderr

#### **Lecture input :**
- `get_next_line2()` : Version custom de get_next_line avec buffer statique `pitcher`

#### **Maths :**
- `ft_abs()`, `ft_atoi()`
- `exceeds_int()` : Vérifie overflow

---

## 🔄 **WORKFLOW COMPLET - EXEMPLE**

### Input exemple :
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
2-4
3-4
4-5
```

### Exécution :

1. **Parsing** :
   - 4 fourmis
   - Salle 1 = start, Salle 5 = end
   - 5 salles, 5 liens

2. **BFS #1** :
   - Trouve chemin : 1 → 2 → 4 → 5 (longueur 3)
   - Ou : 1 → 3 → 4 → 5 (longueur 3)

3. **Update Flow** :
   - Marque : `1.f_to = 2`, `2.f_to = 4`, `4.f_to = 5`
   - Et inverses : `2.f_fm = 1`, etc.

4. **BFS #2** :
   - Cherche chemin alternatif disjoint
   - Peut trouver : 1 → 3 → 4 → 5 si pas bloqué par flux

5. **Path Config** :
   - Calcule : envoyer 2 fourmis sur chaque chemin

6. **Simulation** :
```
Tour 1: L1-2 L2-3
Tour 2: L1-4 L2-4 (collision évitée par flux !)
Tour 3: L1-5 L2-5
Tour 4: L3-2 L4-3
Tour 5: L3-4 L4-4
Tour 6: L3-5 L4-5
```

---

## ⚠️ **POINTS CRITIQUES À COMPRENDRE**

### 1. **Double Pointeur `t_room **` dans `t_bfs`**
Pourquoi ? Pour pouvoir faire du backtracking et modifier les références de salles lors de la construction du chemin.

### 2. **Système de Flux `f_to` / `f_fm`**
C'est un **graphe résiduel** inspiré des algorithmes de flot maximum :
- `f_to` : Sens autorisé pour les fourmis
- `f_fm` : Permet de "défaire" un flux si un meilleur chemin est trouvé

### 3. **Collision Évitation**
Les fourmis ne se cognent JAMAIS car :
- Chaque salle a UN SEUL `ant` à la fois
- Le mouvement est synchronisé (toutes bougent ensemble)
- Les tunnels `f_to` garantissent un sens unique

### 4. **Optimisation du Nombre de Tours**
La fonction `path_config()` implémente une forme de **linear programming** simplifié :
- Chemins courts = plus de fourmis
- Chemins longs = moins de fourmis
- Équilibre pour minimiser : `max(path_length) + num_ants - 1`

---

## 🛠️ **POUR REFAIRE LE PROJET**

### Étapes recommandées :

1. **Structures** : Recréez exactement les 5 structs principales
2. **Parsing** : 
   - FSM (Finite State Machine) à 3 états
   - Validation stricte à chaque étape
3. **Graphe** :
   - Liste chaînée pour les salles
   - Liste chaînée dans chaque salle pour les voisins
4. **BFS** :
   - File classique (FIFO)
   - Marquage des visited
   - Backtracking avec `prev`
5. **Flow** :
   - Marquage `f_to`/`f_fm` après BFS
   - Simulation tour par tour
6. **Optimisation** :
   - Testez toutes les combinaisons de chemins
   - Choisissez celle avec le minimum de tours

### Pièges à éviter :
- ❌ Oublier de libérer la mémoire (fuites)
- ❌ Ne pas gérer les commentaires `#`
- ❌ Accepter plusieurs start/end
- ❌ Laisser des fourmis se cogner
- ❌ Ne pas gérer les chemins de longueurs différentes

---

Ce document couvre **100% des fonctions et structures** du projet. Pour le refaire, utilisez ceci comme spécification technique complète.
