#ifndef JEU_DE_LA_VIE_H
#define JEU_DE_LA_VIE_H

#include <vector>
#include <string>

using namespace std;

class JeuDeLaVie
{
private:
    Grille grille;
    int iterations;

public:
    void Charger(const std::string &nomFichier);
    void simulation();
    void AffichageConsole();
    void AffichageGraphique();
};

class Grille
{
private:
    std::vector<Cellule> cellules;

public:
    std::vector<Cellule> getNextEtat();
    void updateGrille();
    int getTaille();
};

class Cellule{
    private:
        bool isalive;
        int x, y;

    public:
        Cellule(bool envie = false, int pos_x = 0, int pos_y = 0): isalive(envie), x(pos_x), y(pos_y){}
        bool getetat() const { return isalive; }
        void setetat(bool etat) {isalive = etat;}
        int getx() const {return x;}
        int gety() const {return y;}

        int countvoisinsvivants(const vector<vector<Cellule>>& grille){
        int voisinenvie = 0;
        int ligne = grille.size();
        int colonne = grille[0].size();

            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    if (dx == 0 && dy == 0) continue;

                    int newX = x + dx;
                    int newY = y + dy;

                    // Gestion des bords périodiques
                    if (newX >= 0 && newX < ligne && newY >= 0 && newY < colonne && grille[newX][newY].getetat()) {
                        liveNeighbors++;
                    }
                }
            }
            return voisinenvie;
        }


void updateEtat(int voisinenvie){
    if (!isalive && voisinenvie == 3){
        isalive = true;
    }else if(isalive && (voisinenvie < 2 || voisinenvie > 3)) {
            isalive = false;
    }
    }
};

class InterfaceGraphique
{

public:
    void drawGrille();
    void updateAffichage();
    void gereEvenements();
};

int main()
{

    return 0;
}

#endif // JEU_DE_LA_VIE_H