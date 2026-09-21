But du projet

Ex02 simule une petite banque. Tu ne dois pas écrire un programme interactif : il n’y a ni menu ni saisie utilisateur.

Le fichier tests.cpp joue déjà le rôle du programme principal. Il crée huit comptes, réalise des dépôts et des retraits, puis affiche leur état. Ton fichier Account.cpp doit fournir les définitions manquantes de la classe déclarée dans Account.hpp. Le log donné est la sortie de référence à reproduire, à l’exception de la date et de l’heure.
Les fichiers et leurs rôles
Fichier	Rôle
Account.hpp	Le contrat : attributs et méthodes que la classe Account doit posséder
Account.cpp	Ce que tu dois recréer : le fonctionnement réel de chaque méthode
tests.cpp	Le main fourni : il utilise la classe et déclenche les opérations
19920104_091532.log	La sortie attendue : il permet de déduire le comportement précis
Makefile	Compile les fichiers pour produire l’exécutable

📝 À noter — Un fichier .hpp déclare ce qui existe ; un .cpp définit comment cela fonctionne. Le compilateur a besoin des deux.
Ce que fait tests.cpp

Le programme fourni effectue cette séquence :

    Il crée huit comptes avec ces soldes initiaux : 42, 54, 957, 432, 1234, 0, 754 et 16576.

    Il affiche les statistiques globales, puis l’état de chaque compte.

    Il fait un dépôt sur chaque compte : 5, 765, 564, 2, 87, 23, 9 et 20.

    Il affiche de nouveau les statistiques et les comptes.

    Il tente un retrait sur chaque compte : 321, 34, 657, 4, 76, 275, 657 et 7654.

    Deux retraits sont refusés, car 321 > 47 pour le compte 0 et 275 > 23 pour le compte 5.

    Il affiche une dernière fois les statistiques et les comptes, puis les destructeurs ferment les comptes. Le log confirme ces étapes et les valeurs après chaque opération.

Ce que tu fais concrètement

Place dans le dossier ex02 :

text
ex02/
├── Account.hpp
├── Account.cpp
├── tests.cpp
└── Makefile

Ensuite, depuis ce dossier :

bash
make

Le Makefile doit compiler Account.cpp et tests.cpp, avec les options demandées dans le module :

bash
c++ -Wall -Wextra -Werror -std=c++98 Account.cpp tests.cpp -o account

Puis tu lances l’exécutable :

bash
./account

Tu verras un affichage de ce genre — le timestamp actuel changera à chaque lancement :

text
[20260921_152000] index:0;amount:42;created
[20260921_152000] index:1;amount:54;created
...
[20260921_152000] accounts:8;total:20049;deposits:0;withdrawals:0
...
[20260921_152000] index:0;p_amount:47;withdrawal:refused
...
[20260921_152000] index:0;amount:47;closed

Vérifier le résultat

Pour comparer ton programme au log, tu peux enregistrer sa sortie :

bash
./account > output.log

Puis examiner ce fichier :

bash
cat output.log

Les timestamps ne seront pas identiques à ceux du log de 1992 : compare donc surtout le reste de chaque ligne (index, amount, deposits, withdrawals, etc.). Le sujet indique explicitement que les timestamps différeront.

📝 À noter — La commande > redirige la sortie affichée dans le terminal vers un fichier. Ainsi, ./account > output.log garde toute la sortie dans output.log au lieu de l’afficher à l’écran.





__________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________













#include "Account.hpp"
#include <iostream>
#include <ctime>

/*
===========================================================================
1. VARIABLES STATIC DE LA CLASSE
===========================================================================

Question :
Comment compter les comptes et calculer les totaux de TOUS les comptes ?

Un attribut normal appartient à un seul objet.
Un attribut static appartient à la classe entière.

Le header contient :

    static int _nbAccounts;
    static int _totalAmount;
    static int _totalNbDeposits;
    static int _totalNbWithdrawals;

Ces variables doivent donc être définies une seule fois dans Account.cpp.
On leur donne leur valeur de départ : 0.
*/

int Account::_nbAccounts = 0;
int Account::_totalAmount = 0;
int Account::_totalNbDeposits = 0;
int Account::_totalNbWithdrawals = 0;


/*
===========================================================================
2. AFFICHER LE TIMESTAMP
===========================================================================

Question :
Pourquoi le log commence-t-il toujours par ceci ?

    [19920104_091532]

Le header nous donne une fonction privée :

    static void _displayTimestamp(void);

Cette fonction ne reçoit aucun argument.
Son rôle est seulement d'afficher la date et l'heure actuelles.

On l'utilisera avant chaque ligne du log.
*/

void Account::_displayTimestamp(void)
{
    std::time_t currentTime = std::time(NULL);

    /*
    localtime transforme l'heure brute en éléments lisibles :
    année, mois, jour, heure, minute et seconde.
    */
    std::tm *timeInfo = std::localtime(&currentTime);

    char timestamp[20];

    /*
    Format utilisé :
    %Y = année
    %m = mois
    %d = jour
    %H = heure
    %M = minute
    %S = seconde

    Résultat exemple :
    19920104_091532
    */
    std::strftime(
        timestamp,
        sizeof(timestamp),
        "%Y%m%d_%H%M%S",
        timeInfo
    );

    std::cout << "[" << timestamp << "] ";
}


/*
===========================================================================
3. CONSTRUCTEUR
===========================================================================

Question :
Que doit-on faire quand un compte est créé ?

Indices :

    Account(int initial_deposit);

Le compte reçoit donc un dépôt initial.

Le log montre :

    index:0;amount:42;created

Il faut donc :

- donner un index au compte ;
- enregistrer son solde initial ;
- mettre ses compteurs de dépôts et retraits à zéro ;
- mettre à jour les statistiques globales ;
- afficher le message de création.

Pourquoi utiliser _nbAccounts pour l'index ?

Premier compte :
    _nbAccounts vaut 0
    son index devient donc 0

Puis on augmente _nbAccounts.
Le compte suivant recevra l'index 1.
*/

Account::Account(int initial_deposit)
{
    // Le nombre actuel de comptes devient l'index du nouveau compte.
    _accountIndex = _nbAccounts;

    // Le compte commence avec le montant reçu en argument.
    _amount = initial_deposit;

    // Aucune opération n'a encore été effectuée sur ce compte.
    _nbDeposits = 0;
    _nbWithdrawals = 0;

    // Mise à jour des informations globales.
    _nbAccounts++;
    _totalAmount += initial_deposit;

    // Message attendu dans le log.
    _displayTimestamp();
    std::cout << "index:" << _accountIndex
              << ";amount:" << _amount
              << ";created" << std::endl;
}


/*
===========================================================================
4. DESTRUCTEUR
===========================================================================

Question :
Quand doit-on afficher "closed" ?

Le log termine chaque compte par une ligne comme :

    index:0;amount:47;closed

Le destructeur est appelé automatiquement lorsque l'objet est détruit.
C'est donc l'endroit naturel pour afficher cette ligne.
*/

Account::~Account(void)
{
    _displayTimestamp();
    std::cout << "index:" << _accountIndex
              << ";amount:" << _amount
              << ";closed" << std::endl;
}


/*
===========================================================================
5. DEPOT
===========================================================================

Question :
Que doit faire makeDeposit ?

La méthode est déclarée ainsi :

    void makeDeposit(int deposit);

Le log montre :

    p_amount:42
    deposit:5
    amount:47
    nb_deposits:1

Il faut donc :

- mémoriser l'ancien solde ;
- ajouter le dépôt au solde ;
- augmenter le nombre de dépôts du compte ;
- mettre à jour les statistiques globales ;
- afficher les informations.
*/

void Account::makeDeposit(int deposit)
{
    // Ancien solde, nécessaire pour afficher p_amount.
    int previousAmount = _amount;

    // Modification du compte.
    _amount += deposit;
    _nbDeposits++;

    // Modification des statistiques globales.
    _totalAmount += deposit;
    _totalNbDeposits++;

    _displayTimestamp();
    std::cout << "index:" << _accountIndex
              << ";p_amount:" << previousAmount
              << ";deposit:" << deposit
              << ";amount:" << _amount
              << ";nb_deposits:" << _nbDeposits << std::endl;
}


/*
===========================================================================
6. RETRAIT
===========================================================================

Question :
Que faire si le retrait est supérieur au solde ?

La méthode renvoie un bool :

    bool makeWithdrawal(int withdrawal);

Le log montre deux situations.

Retrait accepté :

    withdrawal:34;amount:785;nb_withdrawals:1

Retrait refusé :

    withdrawal:refused

La logique est donc :

- si withdrawal > _amount :
    afficher "refused"
    ne rien modifier
    retourner false

- sinon :
    diminuer le solde
    augmenter les compteurs
    afficher la nouvelle situation
    retourner true
*/

bool Account::makeWithdrawal(int withdrawal)
{
    int previousAmount = _amount;

    // Un compte ne peut pas retirer plus que son solde.
    if (withdrawal > _amount)
    {
        _displayTimestamp();
        std::cout << "index:" << _accountIndex
                  << ";p_amount:" << previousAmount
                  << ";withdrawal:refused" << std::endl;

        return false;
    }

    // Le retrait est accepté.
    _amount -= withdrawal;
    _nbWithdrawals++;

    // Mise à jour des statistiques globales.
    _totalAmount -= withdrawal;
    _totalNbWithdrawals++;

    _displayTimestamp();
    std::cout << "index:" << _accountIndex
              << ";p_amount:" << previousAmount
              << ";withdrawal:" << withdrawal
              << ";amount:" << _amount
              << ";nb_withdrawals:" << _nbWithdrawals << std::endl;

    return true;
}


/*
===========================================================================
7. CONSULTER LE SOLDE
===========================================================================

Question :
Comment lire le solde sans modifier le compte ?

Le header indique :

    int checkAmount(void) const;

La fonction doit donc simplement renvoyer _amount.

Le const garantit que cette fonction ne modifie pas l'objet.
*/

int Account::checkAmount(void) const
{
    return _amount;
}


/*
===========================================================================
8. AFFICHER L'ETAT D'UN COMPTE
===========================================================================

Question :
Comment afficher les informations d'un seul compte ?

Le log montre :

    index:0;amount:47;deposits:1;withdrawals:0

Cette méthode utilise les attributs propres à l'objet :

- _accountIndex
- _amount
- _nbDeposits
- _nbWithdrawals
*/

void Account::displayStatus(void) const
{
    _displayTimestamp();
    std::cout << "index:" << _accountIndex
              << ";amount:" << _amount
              << ";deposits:" << _nbDeposits
              << ";withdrawals:" << _nbWithdrawals << std::endl;
}


/*
===========================================================================
9. GETTERS STATIC
===========================================================================

Question :
Comment consulter les statistiques globales ?

Le header contient plusieurs méthodes static.
Elles ne concernent pas un compte précis.
Elles renvoient donc les variables static correspondantes.
*/

int Account::getNbAccounts(void)
{
    return _nbAccounts;
}

int Account::getTotalAmount(void)
{
    return _totalAmount;
}

int Account::getNbDeposits(void)
{
    return _totalNbDeposits;
}

int Account::getNbWithdrawals(void)
{
    return _totalNbWithdrawals;
}


/*
===========================================================================
10. AFFICHER LES STATISTIQUES GLOBALES
===========================================================================

Question :
Comment afficher le résumé de tous les comptes ?

Le log montre :

    accounts:8;total:20049;deposits:0;withdrawals:0

Cette méthode utilise uniquement les variables static.
Elle n'affiche pas le détail des comptes.
Le détail est affiché séparément par displayStatus().
*/

void Account::displayAccountsInfos(void)
{
    _displayTimestamp();
    std::cout << "accounts:" << _nbAccounts
              << ";total:" << _totalAmount
              << ";deposits:" << _totalNbDeposits
              << ";withdrawals:" << _totalNbWithdrawals << std::endl;
}


/*
===========================================================================
VISUALISATION FINALE
===========================================================================

Création :

    Account(42)

    _accountIndex  = 0
    _amount        = 42
    _nbDeposits    = 0
    _nbWithdrawals = 0

    _nbAccounts    = 1
    _totalAmount   = 42

Dépôt de 5 :

    ancien solde = 42
    nouveau solde = 47

    _nbDeposits         = 1
    _totalNbDeposits    = 1
    _totalAmount        = 47

Retrait de 10 :

    ancien solde = 47
    nouveau solde = 37

    _nbWithdrawals      = 1
    _totalNbWithdrawals = 1
    _totalAmount        = 37

Fin de vie :

    le destructeur affiche "closed"
*/









__________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________








Final :



// ************************************************************************** //
//                                                                            //
//                Account.cpp for GlobalBanksters United                     //
//                                                                            //
// ************************************************************************** //

#include "Account.hpp"
#include <iostream>
#include <ctime>

// Ces variables static sont partagees par tous les objets Account.
// Elles doivent etre definies une seule fois dans le fichier .cpp.
int Account::_nbAccounts = 0;
int Account::_totalAmount = 0;
int Account::_totalNbDeposits = 0;
int Account::_totalNbWithdrawals = 0;

// Affiche la date et l'heure dans le format attendu par le log.
// Exemple : [19920104_091532]
void Account::_displayTimestamp(void)
{
    std::time_t currentTime = std::time(NULL);
    std::tm *timeInfo = std::localtime(&currentTime);
    char timestamp[20];

    std::strftime(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S", timeInfo);
    std::cout << "[" << timestamp << "] ";
}

// Constructeur public : cree un compte avec un depot initial.
Account::Account(int initial_deposit)
{
    // _nbAccounts contient le nombre de comptes crees avant celui-ci.
    // Il sert donc a donner un index unique au nouveau compte.
    _accountIndex = _nbAccounts;

    // Le solde de depart correspond au depot fourni au constructeur.
    _amount = initial_deposit;

    // Au moment de la creation, aucune operation n'a encore ete faite.
    _nbDeposits = 0;
    _nbWithdrawals = 0;

    // Mise a jour des statistiques generales de la classe.
    _nbAccounts++;
    _totalAmount += initial_deposit;

    // Affichage demande par le fichier log.
    _displayTimestamp();
    std::cout << "index:" << _accountIndex
              << ";amount:" << _amount
              << ";created" << std::endl;
}

// Le destructeur est appele automatiquement quand un compte est detruit.
Account::~Account(void)
{
    _displayTimestamp();
    std::cout << "index:" << _accountIndex
              << ";amount:" << _amount
              << ";closed" << std::endl;
}

// Ajoute de l'argent sur ce compte.
void Account::makeDeposit(int deposit)
{
    // On conserve l'ancien solde uniquement pour l'afficher dans le log.
    int previousAmount = _amount;

    _amount += deposit;
    _nbDeposits++;

    // Mise a jour des compteurs globaux.
    _totalAmount += deposit;
    _totalNbDeposits++;

    _displayTimestamp();
    std::cout << "index:" << _accountIndex
              << ";p_amount:" << previousAmount
              << ";deposit:" << deposit
              << ";amount:" << _amount
              << ";nb_deposits:" << _nbDeposits << std::endl;
}

// Retire de l'argent si le solde est suffisant.
// Renvoie false si le retrait est refuse.
bool Account::makeWithdrawal(int withdrawal)
{
    int previousAmount = _amount;

    // Si le retrait depasse le solde, aucune valeur ne doit etre modifiee.
    if (withdrawal > _amount)
    {
        _displayTimestamp();
        std::cout << "index:" << _accountIndex
                  << ";p_amount:" << previousAmount
                  << ";withdrawal:refused" << std::endl;
        return false;
    }

    _amount -= withdrawal;
    _nbWithdrawals++;

    // Mise a jour des compteurs globaux apres un retrait accepte.
    _totalAmount -= withdrawal;
    _totalNbWithdrawals++;

    _displayTimestamp();
    std::cout << "index:" << _accountIndex
              << ";p_amount:" << previousAmount
              << ";withdrawal:" << withdrawal
              << ";amount:" << _amount
              << ";nb_withdrawals:" << _nbWithdrawals << std::endl;
    return true;
}

// Renvoie le solde actuel sans modifier le compte.
int Account::checkAmount(void) const
{
    return _amount;
}

// Affiche les informations d'un seul compte.
// Le const indique que cette fonction ne modifie pas l'objet.
void Account::displayStatus(void) const
{
    _displayTimestamp();
    std::cout << "index:" << _accountIndex
              << ";amount:" << _amount
              << ";deposits:" << _nbDeposits
              << ";withdrawals:" << _nbWithdrawals << std::endl;
}

// Renvoie le nombre total de comptes actuellement crees.
int Account::getNbAccounts(void)
{
    return _nbAccounts;
}

// Renvoie la somme des soldes de tous les comptes.
int Account::getTotalAmount(void)
{
    return _totalAmount;
}

// Renvoie le nombre total de depots effectues.
int Account::getNbDeposits(void)
{
    return _totalNbDeposits;
}

// Renvoie le nombre total de retraits acceptes.
int Account::getNbWithdrawals(void)
{
    return _totalNbWithdrawals;
}

// Affiche les statistiques globales de tous les comptes.
void Account::displayAccountsInfos(void)
{
    _displayTimestamp();
    std::cout << "accounts:" << _nbAccounts
              << ";total:" << _totalAmount
              << ";deposits:" << _totalNbDeposits
              << ";withdrawals:" << _totalNbWithdrawals << std::endl;
}
