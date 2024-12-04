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
private:          // foe"jge
    bool isalive; // kfoejf
    int x, y;

public:
    Cellule(bool alive = false, int pos_x = 0, int pos_y = 0): isalive(alive), x(pos_x), y(pos_y) {} //Constructeur de "Cellule" qui associe les variables aux variables temporaires

    bool getetat() const { return isalive; } // Méthode qui retourne l'état de la variable isalive(true/false) GETTER
    void setetat(bool etat) { isalive = etat; } // Méthode qui définie l'état de la variable isalive à etat(true/false) SETTER
    int getx() const { return x; } //" " " " pour retourner x
    int gety() const { return y; } //" " " " pour retourner y

    int countvoisinsvivants(const vector<vector<Cellule>> &grille){ //compte le nombre de cellules vivantes autour d'une cellule avec vector.
        
        int voisinalive = 0;                         // set de base à 0
        int ligne = grille.size();                   // récupere la taille de la grille grâce à .size() et l'associe à ligne
        int colonne = grille[0].size();              // " " Le [0] récupère sa longueur, qui représente le nombre total de colonnes de la grille.

        for (int dx = -1; dx <= 1; dx++)             //On int dx à -1 et max à <= 1 pour avoir 1 x de moins, le x et 1 x de plus que la cellule que l'on veut analyser
        {
            for (int dy = -1; dy <= 1; dy++)           // Même chose pour y
            {
                if (dx == 0 && dy == 0)                // Si x et y sont = 0 cela signifie que c'est la case dont on veut connaitre les voisins donc on continue
                    continue;

                int newX = x + dx;      // On init newX qui prend l'addition de x et de la cellule actuellement analysé (dx) 
                int newY = y + dy;      // Pareil pour y

                if (newX >= 0 && newX < ligne && newY >= 0 && newY < colonne && grille[newX][newY].getetat()) // Si la valeur de newX et newY sont pas négative et < lignet et colonne (donc hors de la grille) 
                                                                                                              //et que la valeur de cette celulle est true(vivante) alors on incrémente le compteur de voisins vivants
                {
                    voisinalive++; // +1 voisin vivant
                }
            }
        }
        return voisinalive; //retourne le nombre de voisins vivants de la cellule concernée à l'appel de la fonction "countvoisinsvivants()"
    }

    void updateEtat(int voisinalive)        //Méthode faisant appliquer la règle de vie ou de mort de la cellule 
    {
        if (!isalive && voisinalive == 3)       // si la cellule est morte et possède 3 voisines vivantes alors elle prend vie
        {               
            isalive = true;                     //Sinon elle meurt
        }
        else if (isalive && (voisinalive < 2 || voisinalive > 3))   //Sinon si elle est en vie et que le nombre de voisines vivantes est 1 ou 4 ou + 
        {   
            isalive = false;                                        //Elle meurt(ou reste morte)
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
                int voisinalive = cellules[i][j].countvoisinsvivants(cellules);
                newCellules[i][j].updateEtat(voisinalive);
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