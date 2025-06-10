#include<iostream>
#include"bucket.h"
using namespace std;
class Tablehachage{
   private:
    int nmax;
    bucket**tab;
    int nb_courant;
    public:
    Tablehachage(int);
    ~Tablehachage();
    void insertion(string&,Maison*);
    Maison *get(string&);
    void suppression(string&);
    bool contient(string&);
    int size();
    bool est_vide();
    void charger();
    int taille_max();
    void sauvegarder();
        void affiche();
    unsigned int f1(string&);
    unsigned int f2(string&);
    unsigned int f3(string&); 
};
 