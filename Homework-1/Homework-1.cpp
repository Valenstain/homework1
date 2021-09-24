#include <iostream>
#include <string>
#include <optional>
#include <fstream>
#include <vector>
#include <tuple>
#include <algorithm>

/*
    Создайте структуру Person с 3 полями: фамилия, имя, отчество. Поле отчество должно быть опционального типа, т.к. 
    не у всех людей есть отчество. Перегрузите оператор вывода данных для этой структуры. Также перегрузите операторы < и == (используйте tie).
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
    Создайте структуру PhoneNumber с 4 полями:
    ·         код страны (целое число)
    ·         код города (целое число)
    ·         номер (строка)
    ·         добавочный номер (целое число, опциональный тип)
    Для этой структуры перегрузите оператор вывода. Необходимо, чтобы номер телефона выводился в формате: 
    +7(911)1234567 12, где 7 – это номер страны, 911 – номер города, 1234567 – номер, 12 – добавочный номер. 
    Если добавочного номера нет, то его выводить не надо.

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
    Создайте класс PhoneBook, который будет в контейнере хранить пары: Человек – Номер телефона. Конструктор этого класса должен принимать 
    параметр типа ifstream – поток данных, полученных из файла. В теле конструктора происходит считывание данных из файла и заполнение контейнера. 
    Класс PhoneBook должен содержать перегруженный оператор вывода, для вывода всех данных из контейнера в консоль.

    В классе PhoneBook реализуйте метод SortByName, который должен сортировать элементы контейнера по фамилии людей в алфавитном порядке. 
    Если фамилии будут одинаковыми, то сортировка должна выполняться по именам, если имена будут одинаковы, то сортировка производится по 
    отчествам. Используйте алгоритмическую функцию sort.

    Реализуйте метод SortByPhone, который должен сортировать элементы контейнера по номерам телефонов. Используйте алгоритмическую функцию sort.

    Реализуйте метод GetPhoneNumber, который принимает фамилию человека, а возвращает кортеж из строки и PhoneNumber. Строка должна быть пустой, 
    если найден ровно один человек с заданном фамилией в списке. Если не найден ни один человек с заданной фамилией, то в строке должна быть запись «not found», 
    если было найдено больше одного человека, то в строке должно быть «found more than 1».  Для прохода по элементам контейнера используйте алгоритмическую 
    функцию for_each.

    Реализуйте метод ChangePhoneNumber, который принимает человека и новый номер телефона и, если находит заданного человека в контейнере, то меняет его номер телефона 
    на новый, иначе ничего не делает. Используйте алгоритмическую функцию find_if.
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

    // лямбда функция, которая принимает фамилию и выводит номер телефона этого    	человека, либо строку с ошибкой
    auto print_phone_number = [&book](const std::string& surname) {
        std::cout << surname << "\t";
        auto answer = book.GetPhoneNumber(surname);

        if (std::get<0>(answer).empty())
            std::cout << std::get<1>(answer);
        else
            std::cout << std::get<0>(answer);

        std::cout << std::endl;
    };

    // вызовы лямбды
    print_phone_number("Anaken");
    print_phone_number("Petrov");

    std::cout << "----ChangePhoneNumber----" << std::endl;
    book.ChangePhoneNumber(Person{ "Anaken", "Andrei", "Vadimovich" }, PhoneNumber{ 7, 123, "15344458", std::nullopt });
    book.ChangePhoneNumber(Person{ "Mironova", "Margarita", "Vladimirovna" }, PhoneNumber{ 16, 465, "9155448", 13 });
    std::cout << book;


    return 0;
}
