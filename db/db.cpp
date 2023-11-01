#include "db.h"

bool Database::writeDatabase(const std::vector<Person>& people)
{
    std::ofstream file(filename, std::ios::binary | std::ios::app);
    if (!file)
    {
        return false;
    }

    for (const Person& person : people) {
        // Write the person's name length, name, and ID to the binary file
        int nameLength = static_cast<int>(person.name.length());
        file.write(reinterpret_cast<const char*>(&nameLength), sizeof(int));
        file.write(person.name.c_str(), nameLength);
        file.write(reinterpret_cast<const char*>(&person.id), sizeof(int));
    }

    file.close();
    return true;
}

std::vector<Person> Database::readDatabase()
{
    std::ifstream file(filename, std::ios::binary);
    if (!file)
    {
        return {};
    }
    std::vector<Person> people;
    while (true)
    {
        int nameLength = 0;
        if (!file.read(reinterpret_cast<char*>(&nameLength), sizeof(int)))
        {
            break;  // Reached the end of the file
        }
        std::string name;
        name.resize(nameLength);
        file.read(&name[0], nameLength);
        int id = 0;
        file.read(reinterpret_cast<char*>(&id), sizeof(int));
        people.push_back({ name, id });
    }
    file.close();
    return people;
}

void Database::printDatabase(const std::vector<Person>& people)
{
    for (const Person& person : people)
    {
        std::cout << "Name: " << person.name << ", ID: " << person.id << std::endl;
    }
}

void Database::clearDatabase()
{
    std::ofstream file(filename, std::ios::trunc); // Open in truncation mode
    if (!file)
    {
        std::cerr << "Error clearing database." << std::endl;
    }
}

bool Database::removePersonById(std::vector<Person>& people, int id)
{
    auto it = std::remove_if(people.begin(), people.end(),
        [id](const Person& person) { return person.id == id; });

    if (it == people.end())
    {
        // Person with the specified ID was not found
        return false;
    }

    people.erase(it, people.end());
    return true; // Person was successfully removed
}

Database::Database(const std::string& filename)
    : filename(filename)
{
    People = readDatabase();
    if (People.empty())
    {
        std::ofstream file(filename, std::ios::binary | std::ios::app);
        if (!file)
        {
            std::cerr << "Error opening file." << std::endl;
        }
        file.close();
    }
}

bool Database::add(const std::string& name, const int& id)
{
    for (const Person& person : People)
    {
        if (person.id == id)
            return false;
    }
    People.push_back({ name, id });
    clearDatabase();
    writeDatabase(People);
    return true;
}

bool Database::remove(const int& id)
{
    if (removePersonById(People, id))
    {
        clearDatabase();
        writeDatabase(People);
        return true;
    }
    return false;
}

int Database::size()
{
    return static_cast<int>(People.size());
}

bool Database::change(const int& oldID, const int& newID)
{
    for (const Person& person : People)
    {
        if (person.id == newID)
            return false;
    }
    for (Person& person : People)
    {
        if (person.id == oldID)
        {
            person.id = newID;
            clearDatabase();
            writeDatabase(People);
            return true;
        }
    }
    return false;
}

bool Database::change(const int& id, const std::string& newName)
{
    for (Person& person : People)
    {
        if (person.id == id)
        {
            person.name = newName;
            clearDatabase();
            writeDatabase(People);
            return true;
        }
    }
    return false;
}

std::string Database::getName(int index)
{
    if (index >= 0 && index < People.size())
    {
        return People[index].name;
    }
    return "";
}

int Database::getID(int index)
{
    if (index >= 0 && index < People.size())
    {
        return People[index].id;
    }
    return -1;
}

int Database::findIndex(std::string name)
{
    for (int i = 0; i < People.size(); i++)
    {
        if (People[i].name == name)
            return i;
    }
    return -1;
}

int Database::findIndex(int id)
{
    for (int i = 0; i < People.size(); i++)
    {
        if (People[i].id == id)
            return i;
    }
    return -1;
}

void Database::print()
{
    printDatabase(People);
}