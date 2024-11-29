#include <SFML/Graphics.hpp>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread> 

using namespace std;

// Taille des cellules pour l'affichage
const int cellSize = 10; // Définit la taille (en pixels) d'une cellule dans la fenêtre graphique.

// Classe représentant une cellule dans la grille
class Cellule {
private:
    bool isalive; // Indique si la cellule est vivante (`true`) ou morte (`false`).
    int x, y;     // Coordonnées de la cellule dans la grille.

public:
    // Constructeur par défaut initialisant la cellule comme morte et à une position donnée.
    Cellule(bool envie = false, int pos_x = 0, int pos_y = 0)
        : isalive(envie), x(pos_x), y(pos_y) {}

    // Accesseurs pour obtenir ou modifier l'état et les coordonnées de la cellule.
    bool getetat() const { return isalive; }
    void setetat(bool etat) { isalive = etat; }
    int getx() const { return x; }
    int gety() const { return y; }

    // Compte les cellules voisines vivantes autour d'une cellule donnée.
    int countvoisinsvivants(const vector<vector<Cellule>>& grille) {
        int voisinenvie = 0; // Compteur des voisins vivants.
        int ligne = grille.size();
        int colonne = grille[0].size();

        // Parcours des 8 voisins potentiels (en incluant la diagonale).
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                if (dx == 0 && dy == 0) continue; // Ignore la cellule elle-même.

                int newX = x + dx; // Coordonnée x du voisin.
                int newY = y + dy; // Coordonnée y du voisin.

                // Vérifie que le voisin est dans les limites et s'il est vivant.
                if (newX >= 0 && newX < ligne && newY >= 0 && newY < colonne && grille[newX][newY].getetat()) {
                    voisinenvie++;
                }
            }
        }
        return voisinenvie; // Retourne le nombre de voisins vivants.
    }

    // Met à jour l'état de la cellule en fonction des règles du jeu.
    void updateEtat(int voisinenvie) {
        if (!isalive && voisinenvie == 3) { // Naissance d'une cellule morte si exactement 3 voisins vivants.
            isalive = true;
        } else if (isalive && (voisinenvie < 2 || voisinenvie > 3)) { // Mort par isolement ou surpopulation.
            isalive = false;
        }
    }
};

// Classe représentant la grille du jeu
class Grille {
private:
    vector<vector<Cellule>> cellules; // Matrice des cellules.
    int ligne, colonne;               // Dimensions de la grille.

public:
    // Constructeur initialisant une grille vide avec des dimensions données.
    Grille(int l = 0, int c = 0) : ligne(l), colonne(c) {
        cellules.resize(ligne, vector<Cellule>(colonne));
    }

    // Charge une grille à partir d'un fichier contenant l'état initial.
    void loadFromFile(const string& filename) {
        ifstream file(filename);
        if (!file) {
            throw runtime_error("Impossible d'ouvrir le fichier"); // Lève une exception si le fichier est inaccessible.
        }

        file >> ligne >> colonne; // Lit les dimensions de la grille.
        cellules.resize(ligne, vector<Cellule>(colonne));

        // Initialise les cellules avec les valeurs du fichier.
        for (int i = 0; i < ligne; ++i) {
            for (int j = 0; j < colonne; ++j) {
                int etat;
                file >> etat;
                cellules[i][j] = Cellule(etat == 1, i, j);
            }
        }
    }

    // Met à jour la grille entière en appliquant les règles du jeu à chaque cellule.
    void updateGrille() {
        vector<vector<Cellule>> newCellules = cellules; // Copie de l'état actuel.

        for (int i = 0; i < ligne; ++i) {
            for (int j = 0; j < colonne; ++j) {
                int voisinenvie = cellules[i][j].countvoisinsvivants(cellules);
                newCellules[i][j].updateEtat(voisinenvie); // Applique les règles du jeu.
            }
        }

        cellules = newCellules; // Met à jour la grille avec le nouvel état.
    }

    // Retourne la grille complète ou ses dimensions.
    const vector<vector<Cellule>>& getCellules() const { return cellules; }
    int getLigne() const { return ligne; }
    int getColonne() const { return colonne; }

    // Sauvegarde l'état actuel de la grille dans un fichier texte.
    void writeToFile(const string& filename) const {
        ofstream file(filename);
        if (!file) {
            throw runtime_error("Impossible d'ouvrir le fichier de sortie");
        }

        file << ligne << " " << colonne << endl;

        // Écrit l'état de chaque cellule dans le fichier.
        for (const auto& ligne : cellules) {
            for (const auto& cellule : ligne) {
                file << (cellule.getetat() ? 1 : 0) << " ";
            }
            file << endl;
        }

        file.close();
    }
};

// Classe principale du jeu
class JeuDeLaVie {
private:
    Grille grille;       // Grille du jeu.
    int Maxiterations;   // Nombre maximal d'itérations.

public:
    JeuDeLaVie(const string& filename, int maxIter) : Maxiterations(maxIter) {
        grille.loadFromFile(filename); // Charge la grille initiale depuis un fichier.
    }

    // Simule le jeu graphiquement et sauvegarde les états dans des fichiers texte.
    void simulateGraphicWithSave() {
        cout << "Quel mode voulez-vous pour la sauvegarde: (1) itération finale, (2) toutes les itérations" << endl;
        int choix;
        cin >> choix;

        int width = grille.getColonne() * cellSize; // Largeur de la fenêtre graphique.
        int height = grille.getLigne() * cellSize;  // Hauteur de la fenêtre graphique.

        sf::RenderWindow window(sf::VideoMode(width, height), "Jeu de la Vie");
        sf::RectangleShape cell(sf::Vector2f(cellSize - 1.0f, cellSize - 1.0f));
        cell.setFillColor(sf::Color::White);

        // Sauvegarde finale ou par itération.
        if (choix == 1) {
            for (int iter = 0; iter < Maxiterations; ++iter) {
                grille.writeToFile("iteration_finale.txt");
                grille.updateGrille();
                render(window, cell);
            }
        } else if (choix == 2) {
            for (int iter = 0; iter < Maxiterations; ++iter) {
                grille.writeToFile("iteration_" + to_string(iter) + ".txt");
                grille.updateGrille();
                render(window, cell);
            }
        }
    }

    // Rendu graphique de la grille.
    void render(sf::RenderWindow& window, sf::RectangleShape& cell) {
        window.clear();
        const auto& cellules = grille.getCellules();
        for (int i = 0; i < grille.getLigne(); ++i) {
            for (int j = 0; j < grille.getColonne(); ++j) {
                if (cellules[i][j].getetat()) {
                    cell.setPosition(j * cellSize, i * cellSize);
                    window.draw(cell);
                }
            }
        }
        window.display();
        sf::sleep(sf::milliseconds(100)); // Pause entre les itérations.
    }
};

// Fonction principale
int main() {
    try {
        cout << "Indiquez le nombre d'itérations voulues " << endl;
        int Maxiterations;
        cin >> Maxiterations;

        JeuDeLaVie jeu("input.txt", Maxiterations);
        jeu.simulateGraphicWithSave();
    } catch (const exception& e) {
        cerr << "Erreur : " << e.what() << endl;
        return 1;
    }

    cout << "Simulation terminée." << endl;
    return 0;
}
