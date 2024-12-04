#include <SFML/Graphics.hpp>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread> // Pour gérer les pauses

using namespace std;
using namespace sf;

// Taille des cellules pour l'affichage
const int cellSize = 10;

// Classe représentant une cellule dans la grille
class Cellule
{
private:
    bool isalive; // Indique si la cellule est vivante ou non
    int x, y;     // Coordonnées de la cellule dans la grille

public:
    Cellule(bool envie = false, int pos_x = 0, int pos_y = 0)
        : isalive(envie), x(pos_x), y(pos_y) {}

    bool getetat() const { return isalive; }
    void setetat(bool etat) { isalive = etat; }
    int getx() const { return x; }
    int gety() const { return y; }

    int countvoisinsvivants(const vector<vector<Cellule>> &grille)
    {
        int voisinenvie = 0;
        int ligne = grille.size();
        int colonne = grille[0].size();

        for (int dx = -1; dx <= 1; dx++)
        {
            for (int dy = -1; dy <= 1; dy++)
            {
                if (dx == 0 && dy == 0)
                    continue;

                int newX = x + dx;
                int newY = y + dy;

                if (newX >= 0 && newX < ligne && newY >= 0 && newY < colonne && grille[newX][newY].getetat())
                {
                    voisinenvie++;
                }
            }
        }
        return voisinenvie;
    }

    void updateEtat(int voisinenvie)
    {
        if (!isalive && voisinenvie == 3)
        {
            isalive = true;
        }
        else if (isalive && (voisinenvie < 2 || voisinenvie > 3))
        {
            isalive = false;
        }
    }
};

// Classe représentant la grille du jeu
class Grille
{
private:
    vector<vector<Cellule>> cellules;
    int ligne, colonne;

public:
    Grille(int l = 0, int c = 0) : ligne(l), colonne(c)
    {
        cellules.resize(ligne, vector<Cellule>(colonne));
    }

    void loadFromFile(const string &filename)
    {
        ifstream file(filename);
        if (!file)
        {
            throw runtime_error("Impossible d'ouvrir le fichier");
        }

        file >> ligne >> colonne;
        cellules.resize(ligne, vector<Cellule>(colonne));

        for (int i = 0; i < ligne; ++i)
        {
            for (int j = 0; j < colonne; ++j)
            {
                int etat;
                file >> etat;
                cellules[i][j] = Cellule(etat == 1, i, j);
            }
        }
    }

    void updateGrille()
    {
        vector<vector<Cellule>> newCellules = cellules;

        for (int i = 0; i < ligne; ++i)
        {
            for (int j = 0; j < colonne; ++j)
            {
                int voisinenvie = cellules[i][j].countvoisinsvivants(cellules);
                newCellules[i][j].updateEtat(voisinenvie);
            }
        }

        cellules = newCellules;
    }

    const vector<vector<Cellule>> &getCellules() const { return cellules; }
    int getLigne() const { return ligne; }
    int getColonne() const { return colonne; }

    // Sauvegarde de l'état de la grille dans un fichier
    void writeToFile(const string &filename) const
    {
        ofstream file(filename);
        if (!file)
        {
            throw runtime_error("Impossible d'ouvrir le fichier de sortie");
        }

        file << ligne << " " << colonne << endl;

        for (const auto &ligne : cellules)
        {
            for (const auto &cellule : ligne)
            {
                file << (cellule.getetat() ? 1 : 0) << " ";
            }
            file << endl;
        }

        file.close();
    }
};

// Classe principale du jeu
class JeuDeLaVie
{
private:
    Grille grille;     // Grille du jeu
    int Maxiterations; // Nombre maximal d'itérations

public:
    JeuDeLaVie(const string &filename, int maxIter) : Maxiterations(maxIter)
    {
        grille.loadFromFile(filename);
    }

    void simulateGraphicWithSave()
    {
        int width = grille.getColonne() * cellSize;
        int height = grille.getLigne() * cellSize;

        RenderWindow window(VideoMode(width, height), "Jeu de la Vie");
        RectangleShape cell(Vector2f(cellSize - 1.0f, cellSize - 1.0f));
        cell.setFillColor(Color::White);

        // Simulation avec sauvegarde de chaque itération dans un fichier
        for (int iter = 0; iter < Maxiterations; ++iter)
        {
            // Sauvegarder l'état courant dans un fichier
            /* string outputFile = "iteration_" + to_string(iter) + ".txt";
             grille.writeToFile(outputFile);*/
            string outputFile = "iteration_finale.txt";
            grille.writeToFile(outputFile);

            // Mettre à jour la grille
            grille.updateGrille();

            // Afficher la grille
            Event event;
            while (window.pollEvent(event))
            {
                if (event.type == Event::Closed)
                    window.close();
            }

            window.clear();
            const auto &cellules = grille.getCellules();
            for (int i = 0; i < grille.getLigne(); ++i)
            {
                for (int j = 0; j < grille.getColonne(); ++j)
                {
                    if (cellules[i][j].getetat())
                    {
                        cell.setPosition(j * cellSize, i * cellSize);
                        window.draw(cell);
                    }
                }
            }
            window.display();

            // Pause entre les itérations
            sleep(milliseconds(200));
        }
    }
};

// Fonction principale
int main()
{
    try
    {
        // Demande du nombre d'itérations à l'utilisateur
        cout << "Indiquez le nombre d'itération(s) voulue(s) pour votre jeu de la vie" << endl;
        int Maxiterations;
        cin >> Maxiterations;

        JeuDeLaVie jeu("input.txt", Maxiterations);
        jeu.simulateGraphicWithSave();
    }
    catch (const exception &e)
    {
        cerr << "Erreur : " << e.what() << endl;
        return 1;
    }

    cout << "Simulation terminée. Consultez les fichiers générés." << endl;
    return 0;
}