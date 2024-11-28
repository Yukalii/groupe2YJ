#ifndef JEU_DE_LA_VIE_H
#define JEU_DE_LA_VIE_H

#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>
#include <iostream>

using namespace std;

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
                        voisinenvie++;
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

class Grille
{
private:
    vector<vector<Cellule>> cellules;
    int ligne, colonne;

public:
    Grille(int l = 0, int c = 0) : ligne(l), colonne(c) {
            cellules.resize(ligne, vector<Cellule>(colonne));
        }

        void loadFromFile(const string& filename) {
            ifstream file(filename);
            if (!file) {
                throw runtime_error("Impossible d'ouvrir le fichier");
            }

            file >> ligne >> colonne;
            cellules.resize(ligne, vector<Cellule>(colonne));

            for (int i = 0; i < ligne; ++i) {
                for (int j = 0; j < colonne; ++j) {
                    int etat;
                    file >> etat;
                    cellules[i][j] = Cellule(etat == 1, i, j);
                }
            }
        }
    vector<vector<Cellule>>& getCellules() { return cellules; }
    const vector<vector<Cellule>>& getCellules() const { return cellules; }

    void updateGrille() {
        vector<vector<Cellule>> newCellules = cellules;

        for (int i = 0; i < ligne; ++i) {
            for (int j = 0; j < colonne; ++j) {
                int voisinenvie = cellules[i][j].countvoisinsvivants(cellules);
                newCellules[i][j].updateEtat(voisinenvie);
            }
        }

        cellules = newCellules;
    }

    void writeToFile(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file) {
            throw std::runtime_error("Impossible d'ouvrir le fichier de sortie");
        }
        
        file << ligne << " " << colonne << std::endl;
        
        for (const auto& ligne : cellules) {
            for (const auto& cellule : ligne) {
                file << (cellule.getetat() ? 1 : 0) << " ";
            }
            file << std::endl;
        }
        
        file.close();
    }
};

class JeuDeLaVie
{
private:
    Grille grille;
    int Maxiterations;

public:
    JeuDeLaVie(const string& filename, int maxIter = 2):Maxiterations(maxIter) {
        grille.loadFromFile(filename);
    }

    void simulateConsole() {
        for (int iter = 0; iter < Maxiterations; iter++) {
            // Écriture de l'état courant
            std::string outputFile = "iteration_" + std::to_string(iter) + ".txt";
            grille.writeToFile(outputFile);

            // Mise à jour de la grille
            grille.updateGrille();
        }
    }

    void simulateGraphic() {
        // À compléter avec l'implémentation de l'interface graphique
    }

    void simulate() {
        simulateConsole();
    }
};

int main() {
    try {
        JeuDeLaVie jeu("input.txt");
        jeu.simulate();
    } catch (const std::exception& e) {
        std::cerr << "Erreur : " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

#endif // JEU_DE_LA_VIE_H