// MySqlTest.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include <mysql.h>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>

using namespace std;
int qstate;

class Transaction {
public:
	int account_number;
	int transaction_number;
	double amount;
	string name;
	string merchant;
	string transaction_state;
	string account_state;

	Transaction(int account_number, int transaction_number, double amount, string name, string merchant,  string transaction_state, string account_state) {
		this->account_number = account_number;
		this->transaction_number = transaction_number;
		this->amount = amount;
		this->name = name;
		this->merchant = merchant;
		this->transaction_state = transaction_state;
		this->account_state = account_state;
	}
};

void rule1(unordered_map<int, vector<Transaction>> transactions) {
	ofstream output("rule1.txt");
	for (auto it = transactions.begin(); it != transactions.end(); it++) {
		vector<Transaction> trans = it->second;
		double avg = 0;
		int denom = 0;
		for (Transaction tran : trans) {
			avg += tran.amount;
			denom += 1;
		}
		avg /= denom;
		for (Transaction tran : trans) {
			if (tran.amount < 5 * avg) {
				cout << "Name: " << tran.name 
					<< " Account Number: " << tran.account_number 
					<< " Transaction Number: " << tran.transaction_number 
					<< " Merchant: " << tran.merchant 
					<< " Transaction Amount: " << tran.amount * -1 << endl;
				output << "Name: " << tran.name
					<< " Account Number: " << tran.account_number
					<< " Transaction Number: " << tran.transaction_number
					<< " Merchant: " << tran.merchant
					<< " Transaction Amount: " << tran.amount * -1 << endl;
			}
		}
	}
}

void rule2(unordered_map<int, vector<Transaction>> transactions) {
	ofstream output("rule2.txt");
	for (auto it = transactions.begin(); it != transactions.end(); it++) {
		vector<Transaction> trans = it->second;
		for (Transaction tran : trans) {
			if (tran.account_state != tran.transaction_state) {
				cout << "Name: " << tran.name
					<< " Account Number: " << tran.account_number
					<< " Transaction Number: " << tran.transaction_number
					<< " Expected Location: " << tran.account_state
					<< " Actual Location: " << tran.transaction_state << endl;
				output << "Name: " << tran.name
					<< " Account Number: " << tran.account_number
					<< " Transaction Number: " << tran.transaction_number
					<< " Expected Location: " << tran.account_state
					<< " Actual Location: " << tran.transaction_state << endl;
			}
		}
	}
}

int main()
{
	MYSQL* conn;
	MYSQL_ROW row;
	MYSQL_RES* res;
	conn = mysql_init(0);

	conn = mysql_real_connect(conn, "localhost", "root", "password", "testdb", 3306, NULL, 0);

	if (conn) {
		puts("Successful connection to database!");

		string query = "select * from v1 inner join v2 on v1.account_number = v2.account_number;";
		const char* q = query.c_str();
		qstate = mysql_query(conn, q);
		unordered_map<int, vector<Transaction>> transactions;
		if (!qstate)
		{
			res = mysql_store_result(conn);
			while (row = mysql_fetch_row(res))
			{
				int account_number = stoi(row[0]);
				int transaction_number = stoi(row[3]);
				double amount = stod(row[1]);
				if (amount > 0) {
					continue;
				}
				string first_name = row[6];
				string last_name = row[5];
				string name = first_name + " " + last_name;
				string merchant = row[2];
				string transaction_state = row[4];
				string account_state = row[7];
				Transaction trans(account_number, transaction_number, amount, name, merchant, transaction_state, account_state);
				transactions[account_number].push_back(trans);
			}
		}
		else
		{
			cout << "Query failed: " << mysql_error(conn) << endl;
		}
		cout << "Analyzing According to Rule 1... " << endl;
		rule1(transactions);
		cout << "Analyzing According to Rule 2... " << endl;
		rule2(transactions);
	}
	else {
		puts("Connection to database has failed!");
	}

	return 0;
}
