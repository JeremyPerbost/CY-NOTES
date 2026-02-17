#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <conio.h>
// Structure pour stocker les donnees d'une matiere
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <conio.h>

struct Matiere {
    std::string nom;
    double note;
    double coeff;
    bool estInconnue;
};

std::string formater(double valeur) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << valeur;
    return oss.str();
}

void calculer_MOYENNE() {
    std::ifstream fichier("notes.txt");
    std::ofstream exportFile("RESULTAT.txt");
    if (!fichier.is_open()) {
        std::cerr << "Erreur lors de l'ouverture du fichier notes.txt" << std::endl;
        return;
    }

    std::string ligne;
    std::string nomUE = "";
    std::vector<Matiere> matieresDeLUE;
    std::vector<std::string> rattrapages, simulations, ueNonValidees;
    
    double sommeToutesMoyennes = 0.0; // Inclut reelles + predictions
    int nombreTotalUE = 0;

    auto traiterFinUE = [&]() {
        if (matieresDeLUE.empty()) return;

        double sommeNotesCoeff = 0.0;
        double sommeCoeffTotal = 0.0;
        double sommeCoeffInconnus = 0.0;
        std::vector<std::string> nomsInconnus;
        bool aNoteEliminatoire = false;

        for (const auto& m : matieresDeLUE) {
            sommeCoeffTotal += m.coeff;
            if (m.estInconnue) {
                sommeCoeffInconnus += m.coeff;
                nomsInconnus.push_back(m.nom);
            } else {
                sommeNotesCoeff += (m.note * m.coeff);
                if (m.note < 6.0) {
                    aNoteEliminatoire = true;
                    rattrapages.push_back(m.nom + " [" + nomUE + "] : " + formater(m.note) + " (Note eliminatoire)");
                }
            }
        }

        nombreTotalUE++;

        if (!nomsInconnus.empty()) {
            // UE avec prediction
            double noteRequise = (10.0 * sommeCoeffTotal - sommeNotesCoeff) / sommeCoeffInconnus;
            double noteFinale = std::max(6.00, noteRequise);
            
            // Si on simule la reussite, la moyenne de cette UE sera au moins 10 (ou plus si les notes deja la sont tres hautes)
            double moyennePredite = std::max(10.0, (sommeNotesCoeff + (noteFinale * sommeCoeffInconnus)) / sommeCoeffTotal);
            sommeToutesMoyennes += moyennePredite;

            std::cout << "Moyenne " << nomUE << " (predictions) : " << formater(moyennePredite) << std::endl;
            exportFile << "Moyenne " << nomUE << " (predictions) : " << formater(moyennePredite) << "\n";

            std::string listeMatieres = "";
            for (size_t i = 0; i < nomsInconnus.size(); ++i) {
                listeMatieres += nomsInconnus[i] + (i == nomsInconnus.size() - 1 ? "" : ", ");
            }

            std::string msg = "Pour valider " + nomUE + ", il faut au moins " + formater(noteFinale) + " en [" + listeMatieres + "]";
            if (aNoteEliminatoire) msg += " (ATTENTION: UE compromise par une note < 6)";
            else if (noteRequise < 6.00) msg += " (Note de securite: 6.00 impose)";
            
            simulations.push_back(msg);
        } 
        else {
            // UE terminee
            double moyenne = sommeNotesCoeff / sommeCoeffTotal;
            sommeToutesMoyennes += moyenne;

            std::cout << "Moyenne " << nomUE << " : " << formater(moyenne) << std::endl;
            exportFile << "Moyenne " << nomUE << " : " << formater(moyenne) << "\n";

            if (moyenne < 10.0) {
                ueNonValidees.push_back("L'UE [" + nomUE + "] non validee (" + formater(moyenne) + ")");
                for (const auto& m : matieresDeLUE) 
                    if (m.note >= 6.0 && m.note < 10.0)
                        rattrapages.push_back(m.nom + " [" + nomUE + "] : " + formater(m.note) + " (UE non validee)");
            }
        }
        matieresDeLUE.clear();
    };

    while (std::getline(fichier, ligne)) {
        if (ligne.empty()) continue;
        size_t dP = ligne.find(':'), eg = ligne.find('=');
        if (dP != std::string::npos && eg != std::string::npos) {
            double c = std::stod(ligne.substr(0, dP));
            std::string n = ligne.substr(dP + 1, eg - dP - 1);
            std::string val = ligne.substr(eg + 1);
            if (val == "?") matieresDeLUE.push_back({n, 0.0, c, true});
            else matieresDeLUE.push_back({n, std::stod(val), c, false});
        } else { traiterFinUE(); nomUE = ligne; }
    }
    traiterFinUE();

    double moyenneGenerale = (nombreTotalUE > 0) ? (sommeToutesMoyennes / nombreTotalUE) : 0.0;
    
    auto imprimer = [&](std::string t) { std::cout << t << std::endl; exportFile << t << "\n"; };

    imprimer("Moyenne Generale (incluant predictions) : " + formater(moyenneGenerale));
    
    if (!simulations.empty()) {
        imprimer("\n[OBJECTIFS]");
        for (const auto& s : simulations) imprimer("-> " + s);
    }
    if (!rattrapages.empty()) {
        imprimer("\n[ALERTES ET RATTRAPAGES]");
        for (const auto& r : rattrapages) imprimer("- " + r);
    }

    std::cout << "\nBilan exporte avec succes dans RESULTAT.txt" << std::endl;
    fichier.close();
    exportFile.close();
}
void afficherTitre() {
    std::ifstream fichier("titre.txt");
    if (fichier.is_open()) {
        std::string ligne;
        while (std::getline(fichier, ligne)) {
            std::cout << ligne << std::endl;
        }
        fichier.close();
    } else {
        std::cout << "=== CY-NOTE ===" << std::endl;
    }
    std::cout << "\n" << std::endl;
}

void afficherReadme() {
    system("cls");
    std::cout << "--- README ---" << std::endl;
    std::cout << "1. Remplissez notes.txt" << std::endl;
    std::cout << "2. Format matiere: <coeff>:<matiere>=<note>" << std::endl;
    std::cout << "3. Format UE: <nomUE>" << std::endl;
    std::cout << "4. Utilisez '?' pour simuler une note manquante." << std::endl;
    std::cout << "\nAppuyez sur une touche pour revenir au menu..." << std::endl;
    _getch();
}

int main() {
    int choix = 0; // 0 pour Calculer, 1 pour Readme
    bool enCours = true;

    while (enCours) {
        system("cls"); // Nettoie l'ecran a chaque mouvement
        afficherTitre();

        // Affichage du menu avec selecteur
        std::cout << "Utilisez les fleches GAUCHE / DROITE pour choisir :" << std::endl;
        std::cout << "\n       ";
        
        if (choix == 0) std::cout << "> [ CALCULER ] <      README  ";
        else std::cout << "  CALCULER      > [  README  ] <";

        std::cout << "\n\n(Appuyez sur ENTREE pour valider, ou ESC pour quitter)" << std::endl;

        // Lecture d'une touche
        int touche = _getch();

        if (touche == 224) { // Code pour les touches speciales (fleches)
            touche = _getch();
            if (touche == 75) choix = 0; // Fleche Gauche
            if (touche == 77) choix = 1; // Fleche Droite
        } 
        else if (touche == 13) { // Touche ENTREE
            if (choix == 0) {
                system("cls");
                calculer_MOYENNE();
                std::cout << "\nAppuyez sur une touche pour revenir au menu...";
                _getch();
            } else {
                afficherReadme();
            }
        }
        else if (touche == 27) { // Touche ECHAP
            enCours = false;
        }
    }

    return 0;
}