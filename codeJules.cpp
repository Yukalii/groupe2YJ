#include <SFML/Graphics.hpp> //Pour l'interface graphique
#include <vector>            //Pour les vecteurs
#include <fstream>           //Pour manipuler les fichier input/output
#include <iostream>          //Pour les entrées/sorties console (cin par exemple)
#include <stdexcept>         //Pour gérer les exceptions standard
#include <string>            //Pour les strings
#include <thread>            //Pour le sleep(attendre sans rien faire)

using namespace std;
using namespace sf;

// Taille des cellules pour l'affichage
const int cellSize = 10; // en pixels/coté

// Classe représentant une cellule
class Cellule
{
private:
    bool isalive;
    int x, y;

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

// Classe représentant la grille
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

        // Lecture des dimensions de la grille
        file >> ligne >> colonne;
        cellules.resize(ligne, vector<Cellule>(colonne));

        // Lecture des cellules ligne par ligne
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

// Interface pour le rendu
class IRendu
{
public:
    virtual void render(const Grille &grille, float wait) = 0;
    virtual ~IRendu() = default;
};

// Interface pour la gestion des fichiers
class IFileHandler
{
public:
    virtual void loadGrille(Grille &grille, const string &filename) = 0;
    virtual void saveGrille(const Grille &grille, const string &filename) = 0;
    virtual ~IFileHandler() = default;
};

// Rendu graphique
class RenduGraphique : public IRendu
{
private:
    RenderWindow window;
    RectangleShape cell;

public:
    RenduGraphique(int width, int height)
        : window(VideoMode(width, height), "Jeu de la Vie"),
          cell(Vector2f(cellSize - 1.0f, cellSize - 1.0f))
    {
        cell.setFillColor(Color::White);
    }

    void render(const Grille &grille, float wait) override
    {
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
        sleep(milliseconds(wait * 1000));
    }
};

// Gestion des fichiers
class FileHandler : public IFileHandler
{
public:
    void loadGrille(Grille &grille, const string &filename) override
    {
        grille.loadFromFile(filename);
    }

    void saveGrille(const Grille &grille, const string &filename) override
    {
        grille.writeToFile(filename);
    }
};

// Classe principale du jeu
class JeuDeLaVie
{
private:
    Grille grille;
    IRendu *rendu;
    IFileHandler *fileHandler;
    int Maxiterations;
    float wait;
    int choix;
    int iter;

public:
    JeuDeLaVie(IRendu *r, IFileHandler *fh, const string &filename, int maxiter, float twait, int tchoix) : rendu(r), fileHandler(fh), Maxiterations(maxiter), wait(twait), choix(tchoix)
    {
        fileHandler->loadGrille(grille, filename);
    }

    void simulate()
    {
        if (choix == 1)
        {
            for (int iter = 0; iter < Maxiterations; ++iter)
            {
                fileHandler->saveGrille(grille, "iteration_out_" + to_string(iter) + ".txt");
                grille.updateGrille();
            }
        }
        else if (choix == 2)
        {
            while (1)
            {
                fileHandler->saveGrille(grille, "iterationfinale.txt");
                grille.updateGrille();
                rendu->render(grille, wait);
            }
        }
    }
};

// Fonction principale
int main()
{
    int Maxiterations, choix;
    float wait;

    cout << "Voulez-vous lancer en mode Console(1) ou Graphique(2) ? " << endl;
    cin >> choix;

    try
    {
        if (choix == 1)
        {
            cout << "Nombre d'itérations : ";
            cin >> Maxiterations;
            wait = 0;
        }

        else if (choix == 2)
        {
            cout << "Temps entre itérations (en secondes) : ";
            cin >> wait;
        }
        else
        {
            cerr << "Choix invalide !" << endl;
        }

        // Charger la grille pour obtenir ses dimensions
        FileHandler fileHandler;
        Grille grille;
        fileHandler.loadGrille(grille, "input.txt");

        // Initialiser le rendu graphique avec les dimensions détectées
        int width = grille.getColonne() * cellSize; // Largeur de la fenêtre
        int height = grille.getLigne() * cellSize;  // Hauteur de la fenêtre
        RenduGraphique rendu(width, height);

        // Lancer le jeu
        JeuDeLaVie jeu(&rendu, &fileHandler, "input.txt", Maxiterations, wait, choix);
        jeu.simulate();
    }
    catch (const exception &e)
    {
        cerr << "Erreur : " << e.what() << endl;
    }

    return 0;
}