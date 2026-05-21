# Lem-in - Documentation

## 🐜 Bienvenue sur la documentation du projet Lem-in

Ce site documente le projet **Lem-in** de l'école 42, un algorithme de résolution de parcours de fourmis dans une ferme.

---

## 📖 Navigation

- **[Documentation Technique Complète](LEM_IN_EXPLICATIF.md)** - Guide détaillé de toutes les structures et fonctions
- **[Code Source](https://github.com/rkpequod-max/lem-in)** - Dépôt GitHub du projet

---

## 🎯 Aperçu du Projet

**Lem-in** est un projet qui consiste à faire passer un nombre donné de fourmis d'un point de départ à un point d'arrivée dans le minimum de tours, en évitant les collisions.

### Concepts Clés

- **Algorithme BFS** (Breadth-First Search) pour trouver les chemins
- **Gestion de flux** pour optimiser le mouvement des fourmis
- **Évitation de collisions** par synchronisation des mouvements
- **Optimisation** du nombre total de tours

---

## 🚀 Utilisation Rapide

```bash
# Compiler le projet
make

# Exécuter avec un fichier d'entrée
./lem-in < input.txt

# Mode debug
./lem-in --d < input.txt
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
│   ├── farm/          # Gestion de l'input et validation
│   ├── graph/         # Gestion des salles et liens
│   └── solve/         # Algorithmes de résolution
├── libft/             # Bibliothèque utilitaire
└── docs/              # Documentation (ce site)
```

---

## 🔗 Liens Utiles

- [Sujet Officiel 42](https://cdn.intra.42.fr/pdf/pdf/63124/fr.subject.pdf)
- [Dépôt GitHub](https://github.com/rkpequod-max/lem-in)
- [Documentation Technique Détaillée](LEM_IN_EXPLICATIF.md)

---

*Document généré automatiquement via GitHub Actions*
