// TEST VOLONTAIREMENT FAUTIF : exécuter avec AddressSanitizer pour observer le double-free.
#include <cstddef>

struct TableauDynamique {
    double* donnees{nullptr};
    std::size_t taille{0};
    std::size_t capacite{0};
};
void liberer(TableauDynamique& t)
{
    delete[] t.donnees;
    t.donnees = nullptr;
    t.taille = 0;
    t.capacite = 0;
}
int main()
{
    TableauDynamique notes;
    notes.donnees = new double[2]{1.0, 2.0};
    notes.taille = 2;
    notes.capacite = 2;

    TableauDynamique copie = notes; // copie superficielle : même adresse de données
    liberer(copie);
    liberer(notes); // erreur intentionnelle : tentative de libération du même bloc
}
