#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

class Autor {
  std::string nume;
  int anNastere{};
public:
  Autor(std::string nume_, int anNastere_) : nume(std::move(nume_)), anNastere(anNastere_) {}
  const std::string& getNume() const { return nume; }
  friend std::ostream& operator<<(std::ostream& os, const Autor& a) {
    os << "Autor{" << a.nume << ", n. " << a.anNastere << "}";
    return os;
  }
};

class Editura {
  std::string nume;
  std::string oras;
public:
  Editura(std::string nume_, std::string oras_) : nume(std::move(nume_)), oras(std::move(oras_)) {}
  const std::string& getNume() const { return nume; }
  friend std::ostream& operator<<(std::ostream& os, const Editura& e) {
    os << "Editura{" << e.nume << ", " << e.oras << "}";
    return os;
  }
};

class Carte {
  std::string titlu;
  std::string isbn;
  Autor autor;
  Editura editura;
  int anPublicare{};
  std::string gen;
  int exemplareTotal{1};
  int exemplareImprumutate{0};

public:
  Carte(std::string titlu_, std::string isbn_, Autor autor_, Editura editura_,
        int anPublicare_, std::string gen_, int exemplareTotal_ = 1)
    : titlu(std::move(titlu_)), isbn(std::move(isbn_)), autor(std::move(autor_)),
      editura(std::move(editura_)), anPublicare(anPublicare_), gen(std::move(gen_)),
      exemplareTotal(std::max(1, exemplareTotal_)) {}

  // cc/op=/dtor pentru o clasă (aici Carte)
  Carte(const Carte& other)
    : titlu(other.titlu), isbn(other.isbn), autor(other.autor), editura(other.editura),
      anPublicare(other.anPublicare), gen(other.gen),
      exemplareTotal(other.exemplareTotal), exemplareImprumutate(other.exemplareImprumutate) {}

  Carte& operator=(const Carte& other) {
    if (this != &other) {
      titlu = other.titlu;
      isbn = other.isbn;
      autor = other.autor;
      editura = other.editura;
      anPublicare = other.anPublicare;
      gen = other.gen;
      exemplareTotal = other.exemplareTotal;
      exemplareImprumutate = other.exemplareImprumutate;
    }
    return *this;
  }

  ~Carte() = default;

  const std::string& getISBN() const { return isbn; }
  const std::string& getGen() const { return gen; }

  int disponibile() const { return exemplareTotal - exemplareImprumutate; }
  bool imprumuta() { if (disponibile() <= 0) return false; exemplareImprumutate++; return true; }
  bool returneaza() { if (exemplareImprumutate <= 0) return false; exemplareImprumutate--; return true; }

  friend std::ostream& operator<<(std::ostream& os, const Carte& c) {
    os << "Carte{\"" << c.titlu << "\" (" << c.isbn << ") " << c.autor << ", " << c.editura
       << ", an=" << c.anPublicare << ", gen=" << c.gen
       << ", disp=" << c.disponibile() << "/" << c.exemplareTotal << "}";
    return os;
  }
};

class Cititor {
  int id{};
  std::string nume;
  std::unordered_map<std::string,int> active;
  int limita{5};
public:
  Cititor(int id_, std::string nume_, int limita_=5) : id(id_), nume(std::move(nume_)), limita(limita_) {}
  int getId() const { return id; }
  bool poateImprumuta() const { return (int)active.size() < limita; }
  void imprumuta(const std::string& isbn) { active[isbn]++; }
  bool returneaza(const std::string& isbn) {
    auto it = active.find(isbn);
    if (it==active.end()) return false;
    if (--it->second == 0) active.erase(it);
    return true;
  }
  friend std::ostream& operator<<(std::ostream& os, const Cititor& c) {
    os << "Cititor{" << c.id << ", " << c.nume << ", active=" << c.active.size() << "}";
    return os;
  }
};

class Biblioteca {
  std::unordered_map<std::string,Carte> carti;
  std::unordered_map<int,Cititor> cititori;
public:
  void adaugaCarte(const Carte& c) { carti.emplace(c.getISBN(), c); }
  void inscrieCititor(const Cititor& c) { cititori.emplace(c.getId(), c); }

  bool imprumutaCarte(int idCititor, const std::string& isbn) {
    auto itC = cititori.find(idCititor);
    auto itB = carti.find(isbn);
    if (itC==cititori.end() || itB==carti.end()) return false;
    if (!itC->second.poateImprumuta()) return false;
    if (!itB->second.imprumuta()) return false;
    itC->second.imprumuta(isbn);
    return true;
  }

  bool returneazaCarte(int idCititor, const std::string& isbn) {
    auto itC = cititori.find(idCititor);
    auto itB = carti.find(isbn);
    if (itC==cititori.end() || itB==carti.end()) return false;
    if (!itB->second.returneaza()) return false;
    return itC->second.returneaza(isbn);
  }

  friend std::ostream& operator<<(std::ostream& os, const Biblioteca& b) {
    os << "Biblioteca{carti=" << b.carti.size() << ", cititori=" << b.cititori.size() << "}";
    return os;
  }
};

// ====================== MAIN ======================
int main() {
  Autor a1{"Mihai Eminescu",1850};
  Editura e1{"Junimea","Iasi"};
  Carte c1{"Luceafarul","978-1",a1,e1,1883,"poezie",3};

  Cititor ct1{100,"Ana Pop"};

  Biblioteca b;
  b.adaugaCarte(c1);
  b.inscrieCititor(ct1);

  std::cout << a1 << "\n" << e1 << "\n" << c1 << "\n" << ct1 << "\n" << b << "\n";

  std::cout << (b.imprumutaCarte(100,"978-1") ? "Imprumut reusit" : "Esuat") << "\n";
  std::cout << (b.returneazaCarte(100,"978-1") ? "Returnare reusita" : "Esuat") << "\n";

  return 0;
}
