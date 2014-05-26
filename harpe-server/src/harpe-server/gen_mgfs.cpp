///data base
//#include <ORM/backends/Sqlite3.hpp>
//orm::Sqlite3Bdd def("../../Harpe-website/Harpe-website/dev.db");

#include <ORM/backends/MySql.hpp>
orm::MySqlBdd def("root","toor","Harpe-website");

orm::Bdd& orm::Bdd::Default = def;

#include <ORM/core/Tables.hpp>
#include <harpe-server/models.hpp>

int main(int argc,char* argv[])
{
    orm::Bdd::Default.connect();

    auto pep_validates = AnalysePeptideValidated::all();
    std::cout<<"Size: "<<pep_validates.size()<<std::endl;

    orm::Bdd::Default.disconnect();

    return 0;
}
