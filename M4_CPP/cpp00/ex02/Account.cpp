#include "Account.hpp"
#include <iostream>
#include <ctime>

// Un attribut normal appartient à un seul objet.
// Un attribut static appartient à la classe entière.

int Account::_nbAccounts = 0;
int Account:: _totalAmount = 0;
int Account:: _totalNbDeposits = 0;
int Account :: _totalNbWithdrawals = 0;

