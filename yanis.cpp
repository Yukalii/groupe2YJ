#ifndef JEU_DE_LA_VIE_H
#define JEU_DE_LA_VIE_H

#include <vector>
#include <string>

class JeuDeLaVie {
private:
    Grille grille;
    int iterations;

public:
    void Charger(const std::string& nomFichier);
    void simulation();
    void AffichageConsole();
    void AffichageGraphique();
};

class Grille {
private:
    std::vector<Cellule> cellules;

public:
    std::vector<Cellule> getNextEtat();
    void updateGrille();
    int getTaille();
};

class Cellule {
private:
    bool estVivante;
    int x, y;

public:
    int countVoisinsVivants();
    void updateEtat();
};

class InterfaceGraphique {
public:
    void drawGrille();
    void updateAffichage();
    void gereEvenements();
};

int main(){


    return 0;
}

#endif // JEU_DE_LA_VIE_H