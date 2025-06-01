#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include"hachage.h"
Tablehachage::Tablehachage(int n){
    nmax=n;
    nb_courant=0;
    tab=new bucket*[nmax];
    for(int i=0;i<nmax;i++){
        tab[i]=nullptr;
    }
}
Tablehachage::~Tablehachage(){
    for(int i=0;i<nmax;i++){
        bucket *courant=tab[i];
        while(courant!=nullptr){
                bucket *suiv=courant->suiv;
            delete courant->m;
            delete courant;
            courant=suiv;

        }
    }

}
void Tablehachage::insertion(string &cl,Maison*mais){
   unsigned int i=f1(cl) ;
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
   M->suiv=tab[i];
   tab[i]=M;
   nb_courant++;
}
Maison*Tablehachage::get(string&cl){
    unsigned int i=f1(cl);
    if(tab[i]==nullptr){
        return nullptr;
    }
    bucket *temp=tab[i];
    while((temp!=nullptr )&&(temp->m->cle!=cl)){
        temp=temp->suiv;
    }
    if(!temp){
        return temp->m;

    }
    return nullptr;
    
}
void Tablehachage:: suppression(string& cl){
     unsigned int i=f1(cl);
    if(tab[i]==nullptr){
        cerr<<"Erreur:suppression impossible";
        exit(1);
    }
    bucket *p,*temp=tab[i];
    while((temp!=nullptr )&&(temp->m->cle!=cl)){
       p=temp;
        temp=temp->suiv;
    }
    if(!temp){
        p->suiv=temp->suiv;
        delete temp->m; 
        delete temp;
    }
    
}
 bool Tablehachage::contient(string& cle){
    unsigned int i=f1(cle);
    return(tab[i]!=nullptr);

    }
    int Tablehachage::size(){
        return nb_courant;
    }
    bool  Tablehachage::est_vide(){
       if(nb_courant==0){
        return true;
       }
       return false;
    }
    void Tablehachage:: charger(){
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
        insertion(cle, maison);
    }
}

    }
    int  Tablehachage:: taille_max(){
        return nmax;
    }
    void  Tablehachage::sauvegarder(){
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

    unsigned int  Tablehachage::f1(string& cle){
    unsigned int hash = 0;
    for (int i = 0; i < cle.length(); ++i) {
        hash = 31 * hash + cle[i].unicode();
    }
    return hash % nmax;
}

unsigned int Tablehachage::f2(QString &cle) {
    unsigned int hash = 0;
    for (int i = 0; i < cle.length(); ++i) {
        hash = 65599 * hash + cle[i].unicode();
    }
    return hash % nmax;
}

unsigned int Tablehachage::f3(QString &cle) {
    unsigned int hash = 0;
    for (int i = 0; i < cle.length(); ++i) {
        hash = hash ^ ((hash << 5) + cle[i].unicode() + (hash >> 2));
    }
    return hash % nmax;
}
