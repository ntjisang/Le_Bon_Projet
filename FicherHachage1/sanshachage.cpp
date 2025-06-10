#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include"sanshachage.h"
SansHachage::SansHachage(int n){
        std::cout<<"appel du constructeur de SansHachage :"<<endl;
    nmax=n;
    nb_courant=0;
    tab=new bucket*[nmax];
    for(int i=0;i<nmax;i++){
        tab[i]=nullptr;
    }
}
SansHachage::~SansHachage(){
    for(int i=0;i<nmax;i++){
        bucket *courant=tab[i];
        while(courant!=nullptr){
                bucket *suiv=courant->suiv;
            delete courant->m;
            delete courant;
            courant=suiv;

        }
    }
      delete[] tab;  

}
void SansHachage::insertion(string &type,Maison*mais){
   bucket*M=new bucket;
   M->m=new Maison;
   M->m->cle=mais->cle;
   M->m->description=mais->description;
   M->m->nb_chambres=mais->nb_chambres;
   M->m->nb_toilettes=mais->nb_toilettes;
   M->m->photo=mais->photo;
   M->m->standing=mais->standing;
   M->m->type =mais->type;
   M->suiv=nullptr;
   for(int i=0; i<nmax;i++){
if(tab[i]==nullptr){
   tab[i]=M;
        tcourant++;
   }
   else{
      if(tab[i]->m->type==type){
        M->suiv=tab[i];
        tab[i]=M;
        }
        else{
            tcourant++;
        }
          }
   
    }
}

Maison* SansHachage::get(string& cl) {
    if (!contient(cl)) {
        return nullptr;
    }

    for (int i = 0; i < nmax; i++) {
        bucket* temp = tab[i];

     
        while (temp != nullptr) {
            if (temp->m->cle == cl) {
                return temp->m; 
            }
            temp = temp->suiv;
        }
    }

    return nullptr; 
}

 

void SansHachage::suppression(string& cl) {
    bool trouve = false;

    for (int i = 0; i < nmax; i++) {
        bucket* temp = tab[i];
        bucket* prev = nullptr;

        while (temp != nullptr) {
            if (temp->m->cle == cl) {
                if (prev == nullptr) {
                    tab[i] = temp->suiv;
                } else {
                    prev->suiv = temp->suiv;
                }

                delete temp->m;
                delete temp;
                trouve = true;
                break; 
            }

            prev = temp;
            temp = temp->suiv;
        }

        if (trouve) break;
    }

    if (!trouve) {
        cerr << "Erreur : suppression impossible, clé non trouvée" << endl;
        exit(1);
    }
}

bool SansHachage::contient(string& cle) {
    for (int i = 0; i < nmax; i++) {
        bucket* c = tab[i];
        while (c != nullptr) {
            if (c->m->cle == cle) {
                return true; 
            }
            c = c->suiv;
        }
    }
    return false; 

}
    int SansHachage::size(){
        return tcourant;
    }
    bool  SansHachage::est_vide(){
       if(tcourant==0){
        return true;
       }
       return false;
    }
    void SansHachage:: charger(){
{
    // Requête pour récupérer toutes les maisons d'un type donné (exemple 'Villa')
    QSqlQuery query("SELECT cle, type, standing, nb_chambres, nb_toilettes, prix, description FROM maison WHERE type = 'Villa'");
    while (query.next()) {
        Maison* maison = new Maison();
        maison->cle = query.value(0).toInt();
        maison->type = query.value(1).toString();
        maison->standing = query.value(2).toString();
        maison->nb_chambres = query.value(3).toInt();
        maison->nb_toilettes = query.value(4).toInt();
        maison->prix = query.value(5).toDouble();
        maison->description = query.value(6).toString();

        // Charger les photos associées à cette maison
        QSqlQuery photoQuery;
        photoQuery.prepare("SELECT categorie, url FROM photo WHERE cle = :cle");
        photoQuery.bindValue(":cle", maison->cle);

        if(photoQuery.exec()) {
            while(photoQuery.next()) {
                QString cat = photoQuery.value(0).toString();
                QString url = photoQuery.value(1).toString();
                // Ajouter la photo dans la maison (selon ta structure)
                maison->photo[cat] = url;  // exemple si photos est QMap<QString, QString>
            }
        }

        QString cle = maison->type;  // ou une autre clé selon besoin
        insertion(type, maison);
    }
}

    }
    int  SansHachage:: taille_max(){
        return nmax;
    }
    void  SansHachage::sauvegarder(){
    QSqlQuery queryMaison;
    QSqlQuery queryPhoto;

    for (int i = 0; i < nmax; ++i) {
        bucket *current = tab[i];
        while (current != nullptr) {
            Maison* maison = current->maison;

            // Insertion ou mise à jour de la maison
            queryMaison.prepare(
                "REPLACE INTO maison (cle, type, standing, nb_chambres, nb_toilettes, prix, description) "
                "VALUES (:cle, :type, :standing, :nb_chambres, :nb_toilettes, :prix, :description)"
            );
            queryMaison.bindValue(":cle", maison->cle);
            queryMaison.bindValue(":type", maison->type);
            queryMaison.bindValue(":standing", maison->standing);
            queryMaison.bindValue(":nb_chambres", maison->nb_chambres);
            queryMaison.bindValue(":nb_toilettes", maison->nb_toilettes);
            queryMaison.bindValue(":prix", maison->prix);
            queryMaison.bindValue(":description", maison->description);

            if (!queryMaison.exec()) {
                std::cerr << "Erreur lors de la sauvegarde maison: " 
                          << queryMaison.lastError().text().toStdString() << std::endl;
            }

            // Supprimer les photos existantes pour cette maison
            QSqlQuery delQuery;
            delQuery.prepare("DELETE FROM photo WHERE cle_maison = :cle");
            delQuery.bindValue(":cle", maison->cle);
            if (!delQuery.exec()) {
                std::cerr << "Erreur suppression photos: "
                          << delQuery.lastError().text().toStdString() << std::endl;
            }

            // Insertion des photos associées
            QMapIterator<QString, QString> it(maison->photos);
            while (it.hasNext()) {
                it.next();
                QString categorie = it.key();
                QString url = it.value();

                queryPhoto.prepare(
                    "INSERT INTO photo (cle_maison, categorie, url) "
                    "VALUES (:cle_maison, :categorie, :url)"
                );
                queryPhoto.bindValue(":cle_maison", maison->cle);
                queryPhoto.bindValue(":categorie", categorie);
                queryPhoto.bindValue(":url", url);

                if (!queryPhoto.exec()) {
                    std::cerr << "Erreur lors de la sauvegarde photo: "
                              << queryPhoto.lastError().text().toStdString() << std::endl;
                }
            }

            current = current->suivant;
        }
    }
}


void SansHachage::affiche() {
    for (int i = 0; i < nmax; i++) {
        if (tab[i]) {
            bucket* c = tab[i];
            while (c != nullptr) {
                Maison* m = c->m;
                std::cout << "Clé: " << m->cle << ", "
                          << "Type: " << m->type.toStdString() << ", "
                          << "Standing: " << m->standing.toStdString() << ", "
                          << "Nb Chambres: " << m->nb_chambres << ", "
                          << "Nb Toilettes: " << m->nb_toilettes << ", "
                          << "Prix: " << m->prix << ", "
                          << "Description: " << m->description.toStdString() << std::endl;

                // Affichage des photos
                QMapIterator<QString, QString> it(m->photos);
                while (it.hasNext()) {
                    it.next();
                    std::cout << "   Photo [" << it.key().toStdString()
                              << "] : " << it.value().toStdString() << std::endl;
                }

                c = c->suiv;
            }
        }
    }
}
