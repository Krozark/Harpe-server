///data base
//#include <ORM/backends/Sqlite3.hpp>
//orm::Sqlite3DB def("../../Harpe-website/Harpe-website/dev.db");

#include <ORM/backends/MySql.hpp>
orm::MySqlDB def("root","toor","Harpe-website");

orm::DB& orm::DB::Default = def;

#include <ORM/core/Tables.hpp>
#include <ORM/backends/op.hpp>
#include <harpe-server/models.hpp>

#include <mgf/Driver.hpp>
#include <mgf/Spectrum.hpp>

#include <sstream>
#include <fstream>

int main(int argc,char* argv[])
{
    std::string filename;
    if(argc > 2)
    {
        filename = argv[1];
        filename+="-";
    }
    
    orm::DB::Default.connect();

    int score_min;
    std::cout<<"Entrez le score minimale à tenir en compte (<=0 pour ignorer)\n>";
    std::cin>>score_min;

    int score_max;
    std::cout<<"Entrez le score maximal à tenir en compte (<= 0 pour ignorer)\n>";
    std::cin>>score_max;

    int limit;
    std::cout<<"Entrez le nombre de spectres maximals (-1 pour ignorer)\n>";
    std::cin>>limit;

    filename+=std::to_string(score_min)+"-"+std::to_string(score_max);

    std::list<orm::Cache<AnalysePeptideValidated>::type_ptr> pep_validates;
    auto q = AnalysePeptideValidated::query();
    q.filter(orm::Q<AnalysePeptideValidated>(std::string(),orm::op::exact,AnalysePeptideValidated::_modification_seq)); //no modifications

    if(score_min > 0)
        q.filter(orm::Q<AnalysePeptideValidated>(score_min,orm::op::gt,AnalysePeptideValidated::_score));

    if(score_max > 0)
        q.filter(orm::Q<AnalysePeptideValidated>(score_max,orm::op::lte,AnalysePeptideValidated::_score));

     q.orderBy("?")
        .limit(limit)
        .get(pep_validates);

    std::cout<<"Query return "<<pep_validates.size()<<" results."<<std::endl;

    std::fstream f_learning(filename+"-learning.mgf",std::fstream::out | std::fstream::trunc);
    std::fstream f_test(filename+"-test.mgf",std::fstream::out | std::fstream::trunc);
    std::fstream f_validate(filename+"-validation.mgf",std::fstream::out | std::fstream::trunc);
    
    int i = 0;
    int max_learning = pep_validates.size()*0.70;
    int max_test = max_learning + pep_validates.size()*0.15;

    std::cout<<"Add "<<max_learning<<" spectrums in learning, "<<max_test-max_learning<<" spectrums in test and "<< pep_validates.size()-max_test<<" in validation for score >"<<score_min<<" and score <= "<<score_max<<std::endl;
    for(auto& validated : pep_validates)
    {
        std::istringstream stream(validated->analyse->mgf_part);
        mgf::Driver driver(stream);
        mgf::Spectrum* spectrum = driver.next(0,~mgf::Spectrum::PrepareFlags::AddSpecialsPeaks);

        if (driver.isValid() and spectrum)
        {
            std::list<std::string> l;
            std::string s = "";
            for(char c  : (std::string)validated->sequence)
            {
                s+=c;
                s+=+"-";
            }
            s.erase(s.end()-1);
            l.emplace_back(std::move(s));
            spectrum->getHeader().setSeq(l);

            if(i < max_learning)
                f_learning<<*spectrum<<std::endl;
            else if (i< max_test)
                f_test<<*spectrum<<std::endl;
            else
                f_validate<<*spectrum<<std::endl;
        }
        else
            std::cerr<<"Mgf of AnalysePeptide.id="<<validated->analyse->getPk()<<" not valid"<<std::endl;

        delete spectrum;
        ++i;
    }
    f_learning.close();
    f_test.close();
    f_validate.close();

    orm::DB::Default.disconnect();

    return 0;
}
