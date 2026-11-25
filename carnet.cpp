// ============================================================================
//  TP 4 — Carnet de notes en mode menu
//  Cours POO C++ — Chapitre 4 : Fonctions
//
//  Compilation (GCC 14+, C++23) :
//      g++ -std=c++23 -Wall -Wextra -Wpedantic -o carnet carnet.cpp
//  Exécution :
//      ./carnet
// ============================================================================

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <print>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

// Contrainte 1 : tout le code, sauf main, est dans l'espace de noms carnet.
namespace carnet {

// ---------------------------------------------------------------------------
//  Constantes et types
// ---------------------------------------------------------------------------
inline constexpr double noteMin{0.0};
inline constexpr double noteMax{20.0};
inline constexpr double seuilValidation{10.0};

enum class Ordre { Croissant, Decroissant };

/// Résultats statistiques (contrainte 5 : renvoyés dans une structure).
struct Statistiques {
    double minimum{};
    double maximum{};
    double moyenne{};
    int nbValidees{};
};

// ---------------------------------------------------------------------------
//  Saisie (lireEntier et lireNote, repris du chapitre 3 — contrainte 8)
// ---------------------------------------------------------------------------
namespace {

struct EntierLu {
    bool valide{false};
    int valeur{0};
};

struct ReelLu {
    bool valide{false};
    double valeur{0.0};
};

std::string lireLigne(std::string_view invite)
{
    std::cout << invite;
    std::string ligne;
    std::getline(std::cin, ligne);
    return ligne;
}

[[nodiscard]] EntierLu versEntier(const std::string& texte)
{
    std::istringstream flux{texte};
    int valeur{};
    char reste{};
    // Valide si un entier est lu ET qu'il ne reste aucun caractère derrière.
    const bool valide = (flux >> valeur) && !(flux >> reste);
    return {valide, valeur};
}

[[nodiscard]] ReelLu versReel(std::string texte)
{
    // Accepte la virgule décimale : 12,5 -> 12.5
    for (char& c : texte) {
        if (c == ',') {
            c = '.';
        }
    }
    std::istringstream flux{texte};
    double valeur{};
    char reste{};
    const bool valide = (flux >> valeur) && !(flux >> reste);
    return {valide, valeur};
}

} // namespace

[[nodiscard]] bool estNoteValide(double note)
{
    return note >= noteMin && note <= noteMax;
}

/// Lit un entier compris entre min et max (redemande tant que c'est invalide).
/// Si le flux d'entrée est fermé (fin de fichier), renvoie min.
[[nodiscard]] int lireEntier(std::string_view invite, int min, int max)
{
    while (std::cin) {
        const auto [valide, valeur] = versEntier(lireLigne(invite));
        if (!valide) {
            std::println("  Saisie invalide : entrez un nombre entier.");
        } else if (valeur < min || valeur > max) {
            std::println("  La valeur doit être comprise entre {} et {}.", min, max);
        } else {
            return valeur;
        }
    }
    return min;
}

/// Lit une note valide (entre 0 et 20). Accepte 12.5 ou 12,5.
/// Si le flux d'entrée est fermé (fin de fichier), renvoie noteMin.
[[nodiscard]] double lireNote(std::string_view invite)
{
    while (std::cin) {
        const auto [valide, note] = versReel(lireLigne(invite));
        if (!valide) {
            std::println("  Saisie invalide : entrez un nombre (ex. 12.5 ou 12,5).");
        } else if (!estNoteValide(note)) {
            std::println("  La note doit être comprise entre {} et {}.", noteMin, noteMax);
        } else {
            return note;
        }
    }
    return noteMin;
}

// ---------------------------------------------------------------------------
//  Calculs : AUCUNE entrée-sortie (contrainte 3)
// ---------------------------------------------------------------------------

/// Minimum, maximum, moyenne et nombre de notes validées (>= 10).
/// @pre notes n'est pas vide
[[nodiscard]] Statistiques calculerStatistiques(const std::vector<double>& notes)
{
    const auto [mini, maxi] = std::ranges::minmax(notes);

    double somme{0.0};
    int validees{0};
    for (double n : notes) {
        somme += n;
        if (n >= seuilValidation) {
            ++validees;
        }
    }
    return {mini, maxi, somme / static_cast<double>(notes.size()), validees};
}

/// Ajoute le bonus à chaque note, en gardant les notes dans [0 ; 20].
/// Contrainte 6 : utilise une lambda.
void appliquerBonus(std::vector<double>& notes, double bonus)
{
    std::ranges::for_each(notes, [bonus](double& n) {
        n = std::clamp(n + bonus, noteMin, noteMax);
    });
}

/// Trie les notes dans l'ordre demandé. Contrainte 6 : utilise une lambda.
void trier(std::vector<double>& notes, Ordre ordre)
{
    if (ordre == Ordre::Croissant) {
        std::ranges::sort(notes, [](double a, double b) { return a < b; });
    } else {
        std::ranges::sort(notes, [](double a, double b) { return a > b; });
    }
}

// ---------------------------------------------------------------------------
//  Interface utilisateur (une fonction par option du menu — contrainte 2)
// ---------------------------------------------------------------------------

void afficherNotes(const std::vector<double>& notes)
{
    if (notes.empty()) {
        std::println("Aucune note.");
        return;
    }
    for (std::size_t i{0}; i < notes.size(); ++i) {
        std::println("{:>3}. {:6.2f}", i + 1, notes[i]);
    }
}

void ajouter(std::vector<double>& notes)
{
    notes.push_back(lireNote("Note à ajouter : "));
    std::println("Note ajoutée.");
}

void modifier(std::vector<double>& notes)
{
    if (notes.empty()) {
        std::println("Aucune note à modifier.");
        return;
    }
    afficherNotes(notes);
    const int rang = lireEntier("Numéro de la note : ", 1, static_cast<int>(notes.size()));
    notes[static_cast<std::size_t>(rang - 1)] = lireNote("Nouvelle valeur : ");
    std::println("Note modifiée.");
}

void supprimer(std::vector<double>& notes)
{
    if (notes.empty()) {
        std::println("Aucune note à supprimer.");
        return;
    }
    afficherNotes(notes);
    const int rang = lireEntier("Numéro de la note : ", 1, static_cast<int>(notes.size()));
    notes.erase(notes.begin() + (rang - 1));
    std::println("Note supprimée.");
}

void trierNotes(std::vector<double>& notes)
{
    if (notes.empty()) {
        std::println("Aucune note à trier.");
        return;
    }
    const int sens = lireEntier("1. Croissant  2. Décroissant : ", 1, 2);
    trier(notes, sens == 1 ? Ordre::Croissant : Ordre::Decroissant);
    std::println("Notes triées.");
}

void afficherStatistiques(const std::vector<double>& notes)
{
    if (notes.empty()) {
        std::println("Aucune note.");
        return;
    }
    // Contrainte 5 : liaison structurée.
    const auto [mini, maxi, moyenne, validees] = calculerStatistiques(notes);
    std::println("Min {:.2f} | Max {:.2f} | Moyenne {:.2f} | Validées {}/{}",
                 mini, maxi, moyenne, validees, notes.size());
}

void bonus(std::vector<double>& notes)
{
    if (notes.empty()) {
        std::println("Aucune note.");
        return;
    }
    appliquerBonus(notes, lireNote("Bonus à ajouter (0 à 20) : "));
    std::println("Bonus appliqué (les notes sont plafonnées à {}).", noteMax);
}

[[nodiscard]] int afficherMenu()
{
    std::println("\n=== Carnet de notes ===");
    std::println("1. Ajouter une note");
    std::println("2. Afficher les notes");
    std::println("3. Modifier une note");
    std::println("4. Supprimer une note");
    std::println("5. Trier les notes (croissant / décroissant)");
    std::println("6. Statistiques");
    std::println("7. Appliquer un bonus");
    std::println("0. Quitter");
    return lireEntier("Votre choix : ", 0, 7);
}

} // namespace carnet

// ---------------------------------------------------------------------------
//  main : seul élément hors de l'espace de noms carnet
// ---------------------------------------------------------------------------
int main()
{
    std::vector<double> notes;
    int choix{};

    do {
        choix = carnet::afficherMenu();
        switch (choix) {
        case 1: carnet::ajouter(notes);             break;
        case 2: carnet::afficherNotes(notes);       break;
        case 3: carnet::modifier(notes);            break;
        case 4: carnet::supprimer(notes);           break;
        case 5: carnet::trierNotes(notes);          break;
        case 6: carnet::afficherStatistiques(notes); break;
        case 7: carnet::bonus(notes);               break;
        default:                                    break;
        }
    } while (choix != 0);

    std::println("Au revoir !");
    return 0;
}
