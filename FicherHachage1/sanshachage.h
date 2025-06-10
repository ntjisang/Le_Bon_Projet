#ifndef SANSHACHAGE_H
#define SANSHACHAGE_H
#include "bucket.h"
#include <QDebug>


class SansHachage
{
private :
    bucket **tab;
    int nmax;
    int tcourant;
public:
    SansHachage(int);
    ~SansHachage();
    void insertion(string&,Maison*);
    void charge();
    void suppression(string&);
    Maison *get(string&);
    bool contient(string&);
     int taillemax();
      bucket* Tab(int);
    int size();
    bool est_vide();
    void charger();
    void affiche();
    void sauvegarder();
};

#endif // SANSHACHAGE_H
