#include<iostream>
using namespace std;
struct Maison{
    string cle;
    string type;
    string standing;
    int nb_chambres;
    int nb_toilettes;
    string photo;
    string description;
    
};
struct bucket{
    Maison*m;
    bucket*suiv;
};