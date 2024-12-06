#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <limits>

using namespace std;
using namespace sf;

// Taille des cellules pour l'affichage
const int cellSize = 10; // en pixels/coté

// Classe représentant une cellule
class Cellule
{
protected:
    bool isalive;
    int x, y;
    int ok = 1; // variable pour le voisinnage

public:
    Cellule(bool alive = false, int pos_x = 0, int pos_y = 0) : isalive(alive), x(pos_x), y(pos_y) {}

    bool getetat() const { return isalive; }
    void setetat(bool etat) { isalive = etat; }
    int getx() const { return x; }
    int gety() const { return y; }

    int countvoisinsvivants(const vector<vector<Cellule>> &grille)
    {

        int voisinalive = 0;
        int ligne = grille.size();
        int colonne = grille[0].size();

        for (int dx = -ok; dx <= ok; dx++)
        {
            for (int dy = -ok; dy <= ok; dy++)
            {
                if (dx == 0 && dy == 0)
                    continue;

                int newX = x + dx;
                int newY = y + dy;

                if (newX >= 0 && newX < ligne && newY >= 0 && newY < colonne && grille[newX][newY].getetat())
                {
                    voisinalive++;
                }
            }
        }
        return voisinalive;
    }

    void updateEtat(int voisinalive)
    {
        if (!isalive && voisinalive == 3)
        {
            isalive = true;
        }
        else if (isalive && (voisinalive < 2 || voisinalive > 3))
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
                int voisinalive = cellules[i][j].countvoisinsvivants(cellules);
                newCellules[i][j].updateEtat(voisinalive);
            }
        }

        cellules = newCellules;
    }

    vector<vector<Cellule>> &getCellules() { return cellules; }
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
    RenderWindow window;
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
    RectangleShape cell;

public:
    RenduGraphique(int width, int height)
        : cell(Vector2f(cellSize - 1.0f, cellSize - 1.0f))
    {
        window.create(VideoMode(width, height), "Jeu de la Vie");
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

class newmotif : public Cellule
{
public:
    static void addcarre(Grille &grille, int startX, int startY)
    {
        bool placingSquare = true;
        while (placingSquare)
        {
            cout << "Indiquez les coordonnées de votre carré x puis y (appuyez sur Entrée après chaque valeur): " << endl;

            // Ensure we get valid input
            while (!(cin >> startX) || !(cin >> startY))
            {
                cin.clear();                                         // Clear error flags
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard invalid input
                cout << "Entrée invalide. Réessayez." << endl;
            }

            // Get a reference to the cellules in the grid
            vector<vector<Cellule>> &cellules = grille.getCellules();

            // Validate coordinates before adding
            if (startX < 0 || startY < 0 ||
                startX + 1 >= cellules.size() ||
                startY + 1 >= cellules[0].size())
            {
                cout << "Coordonnées invalides. Réessayez." << endl;
                continue;
            }

            // Add the square
            cellules[startX][startY].setetat(true);
            cellules[startX][startY + 1].setetat(true);
            cellules[startX + 1][startY].setetat(true);
            cellules[startX + 1][startY + 1].setetat(true);

            placingSquare = false; // Exit the placement loop
        }
    }

    static void addblinker(Grille &grille, int startX, int startY)
    {
        bool placingBlinker = true;
        while (placingBlinker)
        {
            cout << "Indiquez les coordonnées de votre clignotant x puis y (appuyez sur Entrée après chaque valeur): " << endl;

            while (!(cin >> startX) || !(cin >> startY))
            {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Entrée invalide. Réessayez." << endl;
            }

            vector<vector<Cellule>> &cellules = grille.getCellules();

            if (startX < 0 || startY < 0 ||
                startX + 1 >= cellules.size() ||
                startY + 1 >= cellules[0].size())
            {
                cout << "Coordonnées invalides. Réessayez." << endl;
                continue;
            }

            cellules[startX][startY].setetat(true);
            cellules[startX + 1][startY].setetat(true);
            cellules[startX + 2][startY].setetat(true);

            placingBlinker = false;
        }
    }

    static void addGlider(Grille &grille, int startX, int startY)
    {
        bool placingGlider = true;
        while (placingGlider)
        {
            cout << "Indiquez les coordonnées de votre glider x puis y (appuyez sur Entrée après chaque valeur): " << endl;

            while (!(cin >> startX) || !(cin >> startY))
            {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Entrée invalide. Réessayez." << endl;
            }

            vector<vector<Cellule>> &cellules = grille.getCellules();

            if (startX < 0 || startY < 0 ||
                startX + 1 >= cellules.size() ||
                startY + 1 >= cellules[0].size())
            {
                cout << "Coordonnées invalides. Réessayez." << endl;
                continue;
            }

            cellules[startX][startY].setetat(true);
            cellules[startX + 1][startY].setetat(true);
            cellules[startX + 2][startY].setetat(true);
            cellules[startX + 2][startY + 1].setetat(true);
            cellules[startX + 1][startY + 2].setetat(true);

            placingGlider = false;
        }
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
            // Votre code existant
        }
        else if (choix == 2)
        {
            while (rendu->window.isOpen())
            {
                // Check for events
                sf::Event event;
                while (rendu->window.pollEvent(event))
                {
                    // Close window handling
                    if (event.type == sf::Event::Closed)
                    {
                        rendu->window.close();
                        return;
                    }

                    // Check for 'c' key press
                    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::C)
                    {
                        int startX = 0, startY = 0;

                        // Pause the simulation
                        newmotif::addcarre(grille, startX, startY);
                    }

                    // Check for 'b' key press
                    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::B)
                    {
                        int startX = 0, startY = 0;

                        // Pause the simulation
                        newmotif::addblinker(grille, startX, startY);
                    }

                    // Check for 'g' key press
                    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::G)
                    {
                        int startX = 0, startY = 0;

                        // Pause the simulation
                        newmotif::addGlider(grille, startX, startY);
                    }
                }

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

    cout << "Indiquez la taille du voisinnage ? " << endl;
    int ok = 1;
    cin >> ok;

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
        fileHandler.loadGrille(grille, "GliderCanon.txt");

        // Initialiser le rendu graphique avec les dimensions détectées
        int width = grille.getColonne() * cellSize; // Largeur de la fenêtre
        int height = grille.getLigne() * cellSize;  // Hauteur de la fenêtre
        RenduGraphique rendu(width, height);

        // Lancer le jeu
        JeuDeLaVie jeu(&rendu, &fileHandler, "GliderCanon.txt", Maxiterations, wait, choix);
        jeu.simulate();
    }
    catch (const exception &e)
    {
        cerr << "Erreur : " << e.what() << endl;
    }

    return 0;
}