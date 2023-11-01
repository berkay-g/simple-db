#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

struct Person
{
    std::string name;
    int id;
};

class Database
{
private:
    std::string filename;

    std::vector<struct Person> People;

    bool writeDatabase(const std::vector<Person>& people);

    std::vector<Person> readDatabase();

    void printDatabase(const std::vector<Person>& people);

    void clearDatabase();

    bool removePersonById(std::vector<Person>& people, int id);

public:
    Database(const std::string& filename);

    bool add(const std::string& name, const int& id);
    bool remove(const int& id);

    int size();

    bool change(const int& oldID, const int& newID);
    bool change(const int& id, const std::string& newName);

    std::string getName(int index);
    int getID(int index);

    int findIndex(std::string name);
    int findIndex(int id);

    void print();
};

