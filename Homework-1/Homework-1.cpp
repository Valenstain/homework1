#include <iostream>
#include <string>
#include <optional>
#include <fstream>
#include <vector>
#include <tuple>
#include <algorithm>

/*
    �������� ��������� Person � 3 ������: �������, ���, ��������. ���� �������� ������ ���� ������������� ����, �.�. 
    �� � ���� ����� ���� ��������. ����������� �������� ������ ������ ��� ���� ���������. ����� ����������� ��������� < � == (����������� tie).
*/

struct Person
{
    std::string surname;
    std::string name;
    std::optional<std::string> middleName;

    friend std::ostream& operator << (std::ostream& os, const Person& person);

    bool operator < (const Person& person)
    {
        return std::tie(surname, name, middleName) < std::tie(person.surname, person.name, person.middleName);
    }

    bool operator == (const Person& person)
    {
        return std::tie(surname, name, middleName) == std::tie(person.surname, person.name, person.middleName);
    }
};

std::ostream& operator << (std::ostream& os, const Person& person)
{
    os << person.surname << "\t" << person.name;

    if (person.middleName.has_value())
        os << "\t" << person.middleName.value() << "\t";
    else
        os << "\t\t\t";

    return os;
}

/*
    �������� ��������� PhoneNumber � 4 ������:
    �         ��� ������ (����� �����)
    �         ��� ������ (����� �����)
    �         ����� (������)
    �         ���������� ����� (����� �����, ������������ ���)
    ��� ���� ��������� ����������� �������� ������. ����������, ����� ����� �������� ��������� � �������: 
    +7(911)1234567 12, ��� 7 � ��� ����� ������, 911 � ����� ������, 1234567 � �����, 12 � ���������� �����. 
    ���� ����������� ������ ���, �� ��� �������� �� ����.

*/

struct PhoneNumber
{
    int keyCountry;
    int keyCity;
    std::string number;
    std::optional<int> additionalNumber;

    friend std::ostream& operator << (std::ostream& os, const PhoneNumber& phoneNumber);
};

std::ostream& operator << (std::ostream& os, const PhoneNumber& phoneNumber)
{
    os << "+" << phoneNumber.keyCountry << "(" << phoneNumber.keyCity << ")" << phoneNumber.number;

    if (phoneNumber.additionalNumber.has_value())
        os << " " << phoneNumber.additionalNumber.value();

    return os;
}

/*
    �������� ����� PhoneBook, ������� ����� � ���������� ������� ����: ������� � ����� ��������. ����������� ����� ������ ������ ��������� 
    �������� ���� ifstream � ����� ������, ���������� �� �����. � ���� ������������ ���������� ���������� ������ �� ����� � ���������� ����������. 
    ����� PhoneBook ������ ��������� ������������� �������� ������, ��� ������ ���� ������ �� ���������� � �������.

    � ������ PhoneBook ���������� ����� SortByName, ������� ������ ����������� �������� ���������� �� ������� ����� � ���������� �������. 
    ���� ������� ����� �����������, �� ���������� ������ ����������� �� ������, ���� ����� ����� ���������, �� ���������� ������������ �� 
    ���������. ����������� ��������������� ������� sort.

    ���������� ����� SortByPhone, ������� ������ ����������� �������� ���������� �� ������� ���������. ����������� ��������������� ������� sort.

    ���������� ����� GetPhoneNumber, ������� ��������� ������� ��������, � ���������� ������ �� ������ � PhoneNumber. ������ ������ ���� ������, 
    ���� ������ ����� ���� ������� � �������� �������� � ������. ���� �� ������ �� ���� ������� � �������� ��������, �� � ������ ������ ���� ������ �not found�, 
    ���� ���� ������� ������ ������ ��������, �� � ������ ������ ���� �found more than 1�.  ��� ������� �� ��������� ���������� ����������� ��������������� 
    ������� for_each.

    ���������� ����� ChangePhoneNumber, ������� ��������� �������� � ����� ����� �������� �, ���� ������� ��������� �������� � ����������, �� ������ ��� ����� �������� 
    �� �����, ����� ������ �� ������. ����������� ��������������� ������� find_if.
*/

class PhoneBook
{
    std::vector<std::pair<Person, PhoneNumber>> listPhones;

public:

    PhoneBook(std::ifstream& file)
    {
        std::cout << file.is_open() << std::endl;
        if (file.is_open())
        {
            std::string line;

            while (std::getline(file, line))
            {
                std::string personStr = line.substr(0, line.find('+'));
                std::string phoneStr = line.substr(line.find('+'));

                // Person
                std::size_t prev = 0;
                std::size_t next = personStr.find(' ');
                std::string surname = personStr.substr(prev, next);

                prev = next + 1;
                next = personStr.find(' ', prev);
                std::string name = personStr.substr(prev, next - prev);

                prev = next + 1;
                next = personStr.find(' ', prev);
                std::optional<std::string> middleName;

                if (next != std::string::npos)
                    middleName = personStr.substr(prev, next - prev);

                // Phone
                prev = 1;
                next = phoneStr.find('(');
                int keyCountry = std::stoi(phoneStr.substr(prev, next));

                prev = next + 1;
                next = phoneStr.find(')', prev);
                int keyCity = std::stoi(phoneStr.substr(prev, next - prev));

                prev = next + 1;
                next = phoneStr.find(' ', prev);
                std::string number{};
                std::optional<int> additionalNumber;

                if (next != std::string::npos)
                {
                    number = phoneStr.substr(prev, next - prev);
                    additionalNumber = std::stoi(phoneStr.substr(next + 1));
                }
                else
                {
                    number = phoneStr.substr(prev);
                }

                listPhones.push_back( { Person{ surname, name, middleName }, PhoneNumber{ keyCountry, keyCity, number, additionalNumber } } );
            }

            file.close();
        }
    }

    void SortByPhone()
    {
        std::sort(listPhones.begin(), listPhones.end(), [](std::pair<Person, PhoneNumber>& p1, std::pair<Person, PhoneNumber>& p2) { return p1.second.keyCountry < p2.second.keyCountry; });
    }

    void SortByName()
    {
        std::sort(listPhones.begin(), listPhones.end(), [](std::pair<Person, PhoneNumber>& p1, std::pair<Person, PhoneNumber>& p2) { return p1.first.surname < p2.first.surname; });
    }

    std::pair<std::string, std::string> GetPhoneNumber(std::string surname)
    {
        std::string phoneStr{};
        int count{};

        for (std::pair<Person, PhoneNumber> phone : listPhones)
        {
            if (phone.first.surname == surname)
            {
                count++;
                phoneStr = (count == 1) ? ("+" + std::to_string(phone.second.keyCountry) + "(" + std::to_string(phone.second.keyCity) + ")" + phone.second.number + " " + (phone.second.additionalNumber.has_value() ? std::to_string(phone.second.additionalNumber.value()) : "")) : "";
            }
        }

        std::string str{};

        if (count > 1)
            str = "found more than 1";
        else if (count == 0)
            str = "not found";

        return std::make_pair(str, phoneStr);
    }

    void ChangePhoneNumber(Person person, PhoneNumber phone)
    {
        std::find_if(listPhones.begin(), listPhones.end(), [person, phone](std::pair<Person, PhoneNumber>& line) { if (line.first == person) { line.second = phone; return true; } return false; });
    }

    friend std::ostream& operator << (std::ostream& os, const PhoneBook& phoneBook);
};

std::ostream& operator << (std::ostream& os, const PhoneBook& phoneBook)
{
    for (size_t i = 0; i < phoneBook.listPhones.size(); i++)
    {
        std::cout << phoneBook.listPhones.at(i).first << phoneBook.listPhones.at(i).second << std::endl;
    }

    return os;
}

int main()
{
    // Exercise 1
    Person p1 = { "s", "n", "m"};
    Person p2 = { "s", "n" };

    std::cout << (p1 < p2) << std::endl;
    std::cout << (p1 == p2) << std::endl;

    // Exercise 2
    PhoneNumber phoneNumber = { 7, 911, "1234567", 12};
    std::cout << phoneNumber << std::endl;

    // Exercise 3
    std::ifstream file("PhoneBook.txt");
    PhoneBook book(file);
    std::cout << book;

    std::cout << "------SortByPhone-------" << std::endl;
    book.SortByPhone();
    std::cout << book;

    std::cout << "------SortByName--------" << std::endl;
    book.SortByName();
    std::cout << book;

    std::cout << "-----GetPhoneNumber-----" << std::endl;

    // ������ �������, ������� ��������� ������� � ������� ����� �������� �����    	��������, ���� ������ � �������
    auto print_phone_number = [&book](const std::string& surname) {
        std::cout << surname << "\t";
        auto answer = book.GetPhoneNumber(surname);

        if (std::get<0>(answer).empty())
            std::cout << std::get<1>(answer);
        else
            std::cout << std::get<0>(answer);

        std::cout << std::endl;
    };

    // ������ ������
    print_phone_number("Anaken");
    print_phone_number("Petrov");

    std::cout << "----ChangePhoneNumber----" << std::endl;
    book.ChangePhoneNumber(Person{ "Anaken", "Andrei", "Vadimovich" }, PhoneNumber{ 7, 123, "15344458", std::nullopt });
    book.ChangePhoneNumber(Person{ "Mironova", "Margarita", "Vladimirovna" }, PhoneNumber{ 16, 465, "9155448", 13 });
    std::cout << book;


    return 0;
}
