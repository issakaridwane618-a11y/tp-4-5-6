#include <print>
#include <vector>

int main()
{
    std::vector<double> notes;

    for (int i{0}; i < 10; ++i) {
        notes.push_back(10.0 + i);
        std::println("taille {:>2}, capacité {:>2}", notes.size(), notes.capacity());
    }

    std::vector<double> copie = notes; // copie indépendante gérée par vector
    std::println("\nCopie du tableau :");
    for (double note : copie) {
        std::println("{}", note);
    }
    return 0;
}
