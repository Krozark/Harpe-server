///data base
//#include <ORM/backends/Sqlite3.hpp>
//orm::Sqlite3Bdd def("../../Harpe-website/Harpe-website/dev.db");

#include <ORM/backends/MySql.hpp>
orm::MySqlBdd def("root","toor","Harpe-website");

orm::Bdd& orm::Bdd::Default = def;

#include <ORM/core/Tables.hpp>
#include <ORM/backends/op.hpp>
#include <harpe-server/models.hpp>

#include <mgf/Driver.hpp>
#include <mgf/Spectrum.hpp>

#include <sstream>
#include <fstream>



int main(int argc,char* argv[])
{
    if(argc < 2)
    {
        std::cerr<<"Usage are:"<<argv[0]<<" <output>"<<std::endl;
        return 1;
    }
    
    orm::Bdd::Default.connect();

    std::list<orm::Cache<AnalysePeptideValidated>::type_ptr> pep_validates;
    AnalysePeptideValidated::query()
        .filter(std::string(),orm::op::exact,AnalysePeptideValidated::_modification_seq) //no modifications
        .filter(60,orm::op::gte,AnalysePeptideValidated::_score)
        .get(pep_validates);

    std::cout<<"Query return "<<pep_validates.size()<<" results."<<std::endl;

    std::fstream out(argv[1],std::fstream::out | std::fstream::trunc);

    for(auto& validated : pep_validates)
    {

        std::istringstream stream(validated->analyse->mgf_part);
        mgf::Driver driver(stream);
        mgf::Spectrum* spectrum = driver.next(0,~mgf::Spectrum::PrepareFlags::AddSpecialsPeaks);

        if (driver.isValid() and spectrum)
        {
            std::list<std::string> l = {validated->sequence};
            spectrum->getHeader().setSeq(l);

            out<<*spectrum<<std::endl;
        }
        else
            std::cerr<<"Mgf of AnalysePeptide.id="<<validated->analyse->getPk()<<" not valid"<<std::endl;

        delete spectrum;

    }
    out.close();
    orm::Bdd::Default.disconnect();

    return 0;
}
